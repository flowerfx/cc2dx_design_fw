#include "RKString.h"
#include "../Common/RKMacros.h"
namespace RKUtils
{
// if _RKSTRING_AUTOTEST is defined, this piece of code 
// launches autotests in RKStringTest and RKStringUTFTest 
// automatically during static initialization
#ifdef _RKSTRING_AUTOTEST
struct __RKStringTestsLauncher
{
	__RKStringTestsLauncher() { void _TestRKString(); _TestRKString(); void _TestRKStringUTF(); _TestRKStringUTF(); }
} __testsLauncher;
#  define _SELFCHECK { _SanityCheck(); }
#else
#  define _SELFCHECK {}
#endif

const RKString RKString::EmptyString;
const char* const RKString::WhitespaceCharacters = " \n\t";

// sanity check cheks internal invariants
void RKString::_SanityCheck() const
{
	const uint32 length = Length();
	RKASSERT(length == RKString_Length(GetString()), "Length check failed");
	if (_ssoSize != SSO_USE_HEAP_MARK)
	{
		RKASSERT(_ssoSize < SSO_BUFFER_SIZE, "SSO size larger than SSO buffer, internal error");
	}
	else
	{
  	RKASSERT(_heapStorage.size >= SSO_BUFFER_SIZE, "Heap size smaller than SSO buffer, should be stored in SSO, not necessary an error, but should be avoided");
		RKASSERT(_heapStorage.size < _heapStorage.capacity, "heap size larger than heap buffer, internal error");
		RKASSERT(_heapStorage.bufferPtr <= _heapStorage.stringPtr, "Heap buffer pointer larger than string pointer ")
	}
}

RKString::RKString()
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
}

RKString::RKString(const RKString& str)
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
	_Assign(str.GetString(), str.Length());
}

RKString::RKString(RKString&& str)
{
  memcpy(this, &str, sizeof *this);

  //  Trick the destructor
  str._ssoSize = 0;
}

RKString::RKString(const char* pString)
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
	Assign(pString);
}

RKString::RKString(const std::string& str)
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
	_Assign(str.c_str(), str.size());
}

RKString::RKString(const wchar_t* pString)
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
	Assign(pString);
}

RKString::RKString(const std::wstring& str)
	: _ssoSize(0)
{
	_ssoStorage[0] = 0;
	_AssignUTF(str.c_str(), str.size());
}

RKString::~RKString()
{
//	_SELFCHECK;

	if (_ssoSize == SSO_USE_HEAP_MARK)
	{
		RKDELETEARRAY(_heapStorage.bufferPtr);
	}
}

// returns true if provided pointer pointing to
// part of this string, either sso buffer or dynamic buffer
bool RKString::_IsPartOfThisString(const char* ptr) const
{
	const bool partOfInstance = (ptr >= (char*)this) && (ptr < (char*)this + sizeof(*this));
	if (partOfInstance)
	{
		return true;
	}
	if (_ssoSize == SSO_USE_HEAP_MARK)
	{
		const bool partOfBuffer = (ptr >= _heapStorage.bufferPtr) && (ptr < _heapStorage.bufferPtr + _heapStorage.capacity);
		return partOfBuffer;
	}
	else
	{
		return false;
	}
}	

// returns size of buffer to allocate
uint32 RKString::_DetermineNewDynamicBufferSize(uint32 currentCapacity, uint32 requiredCapacity, bool strict)
{
	if (!strict)
	{
		uint32 result;
		if (requiredCapacity > currentCapacity)
		{
			if (currentCapacity != 0)
			{
				result = currentCapacity * 2;
				while (requiredCapacity > result)
				{
					result *= 2;
				}
			}
			else
			{
				result = requiredCapacity;
			}
		}
		else
		{
			if (requiredCapacity != 0)
			{
				result = currentCapacity;
				while (requiredCapacity <= result / 4)
				{
					result /= 2;
				}
			}
			else
			{
				result = 0;
			}
		}
		return result;
	}
	else
	{
		return requiredCapacity;
	}
}

// main allocation function
// returns writable zero-terminated buffer of requested length
// if keepCurrentContent is true, then current content will be preserved
// if strictBufferSize is true, then buffer of exact size 'stringLength+1' will be allocated
char* RKString::_AcquireWritableStringBuffer(uint32 stringLength, bool keepCurrentContent, bool strictBufferSize)
{
	if (stringLength < SSO_BUFFER_SIZE)
	{
		if (_ssoSize == SSO_USE_HEAP_MARK)
		{
			char* bufferPtr = _heapStorage.bufferPtr;
			if (keepCurrentContent)
			{
				memcpy(_ssoStorage, _heapStorage.stringPtr, std::min(stringLength, _heapStorage.size));
			}
			RKDELETEARRAY(bufferPtr);
		}

		_ssoSize = stringLength;
		_ssoStorage[stringLength] = 0;
		return _ssoStorage;
	}
	else
	{
		if (_ssoSize != SSO_USE_HEAP_MARK)
		{
			const uint32 capacity = _DetermineNewDynamicBufferSize(0, stringLength + 1, strictBufferSize);
			RKASSERT(capacity > stringLength, "Invalid _DetermineNewDynamicBufferSize function");
			char* buffer = RKNEWARRAY(char, capacity);

			if (keepCurrentContent)
			{
				memcpy(buffer, _ssoStorage, _ssoSize);
			}

			_ssoSize = SSO_USE_HEAP_MARK;
			_heapStorage.capacity = capacity;
			_heapStorage.bufferPtr = buffer;
			_heapStorage.stringPtr = buffer;
		}
		else
		{
			if (stringLength >= _heapStorage.capacity)
			{
				uint32 newCapacity = _DetermineNewDynamicBufferSize(_heapStorage.capacity, stringLength + 1, strictBufferSize);
				RKASSERT(newCapacity > stringLength, "Invalid _DetermineNewDynamicBufferSize function");
				char* newBuffer = RKNEWARRAY(char, newCapacity);

				if (keepCurrentContent)
				{
					memcpy(newBuffer, _heapStorage.stringPtr, _heapStorage.size);
				}

				RKDELETEARRAY(_heapStorage.bufferPtr);
				_heapStorage.capacity = newCapacity;
				_heapStorage.bufferPtr = newBuffer;
				_heapStorage.stringPtr = newBuffer;
			}
			else
			{
				const uint32 newCapacity = _DetermineNewDynamicBufferSize(_heapStorage.capacity, stringLength + 1, strictBufferSize);
				RKASSERT(newCapacity > stringLength, "Invalid _DetermineNewDynamicBufferSize function");
				RKASSERT(newCapacity <= _heapStorage.capacity, "Invalid _DetermineNewDynamicBufferSize function");

				if (newCapacity == _heapStorage.capacity)
				{
					const uint32 currentStringOffset = _heapStorage.stringPtr - _heapStorage.bufferPtr;
					if ((_heapStorage.capacity - currentStringOffset) <= stringLength)
					{
						for (uint32 i = 0; i < std::min(stringLength, _heapStorage.size); ++i)
						{
							_heapStorage.bufferPtr[i] = _heapStorage.stringPtr[i];
						}
						_heapStorage.stringPtr = _heapStorage.bufferPtr;
					}
				}
				else
				{
					char* newBuffer = RKNEWARRAY(char, newCapacity);

					if (keepCurrentContent)
					{
						memcpy(newBuffer, _heapStorage.stringPtr, std::min(stringLength, _heapStorage.size));
					}

					RKDELETEARRAY(_heapStorage.bufferPtr);
					_heapStorage.capacity = newCapacity;
					_heapStorage.bufferPtr = newBuffer;
					_heapStorage.stringPtr = newBuffer;
				}
			}
		}

		_heapStorage.size = stringLength;
		_heapStorage.stringPtr[_heapStorage.size] = 0;
		return _heapStorage.stringPtr;
	}
}

RKString RKString::MakeFormatted(const char* format, ...)
{
	RKString str;
	va_list arglist;
	va_start(arglist, format);
	str._AssignFormatted(format, arglist);
	va_end(arglist);
	return str;
}

void RKString::AssignFormatted(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	_AssignFormatted(format, arglist);
	va_end(arglist);
}

void RKString::_AssignFormatted(const char* pFormat, va_list arglist)
{
	RKASSERT(pFormat, "Invalid argument, NULL received");

	if (_IsPartOfThisString(pFormat))
	{
		RKString dst;
		dst._AssignFormatted(RKString(pFormat).GetString(), arglist);
		Assign(dst);
		return;
	}
  
  _SELFCHECK;

	int resultingStringSize = vsnprintf(NULL, 0, pFormat, arglist);
	RKASSERT(resultingStringSize >= 0, "Internal sprintf error")
	char* buffer = _AcquireWritableStringBuffer(resultingStringSize, false);
	int writtenBytes = vsnprintf(buffer, resultingStringSize + 1, pFormat, arglist);
	RKASSERT(writtenBytes == resultingStringSize, "sprintf calls results mismatch");
  
  _SELFCHECK;
}

void RKString::_AssignLongValue(long long value, int base, bool useBasePrefix)
{
	switch (base)
	{
	case 8:
		if (useBasePrefix)
		{
			AssignFormatted("0%llo", value);
		}
		else
		{
			AssignFormatted("%llo", value);
		}
		break;
	case 10:
		AssignFormatted("%lld", value);
		break;
	case 16:
		if (useBasePrefix)
		{
			AssignFormatted("0x%llx", value);
		}
		else
		{
			AssignFormatted("%llx", value);
		}
		break;
	default:
		RKASSERT(false, "Provided base value is not supproted");
	}
}

template <typename OUT_TYPE, typename INTERNAL_TYPE>
bool _ParseDecimal(const char* stringData, uint32 stringLength, 
	uint32 ignorePrefixCount, uint32 ignoreSuffixCount, 
	int base, OUT_TYPE& result, 
	INTERNAL_TYPE (*strtolFunction)(const char*, char**, int))
{
	stringData += ignorePrefixCount;
	stringLength -= ignorePrefixCount + ignoreSuffixCount;

	char* endPtr = NULL;
	INTERNAL_TYPE parsedValue = strtolFunction(stringData, &endPtr, base);

	const bool readSuccessfully = endPtr != stringData;
	const bool nothingRemains = endPtr == stringData + stringLength;

	if (readSuccessfully && nothingRemains)
	{
		result = parsedValue;
		return true;
	}
	else
	{
		return false;
	}	
}

bool RKString::ParseValue(int& out, int base) const
{
	return _ParseDecimal<int, long>(GetString(), Length(), 
		_GetStripSymbolsCountLeft(WhitespaceCharacters), _GetStripSymbolsCountRight(WhitespaceCharacters),
		base, out,
		strtol);
}

bool RKString::ParseValue(unsigned int& out, int base) const
{
	return _ParseDecimal<unsigned int, unsigned long>(GetString(), Length(), 
		_GetStripSymbolsCountLeft(WhitespaceCharacters), _GetStripSymbolsCountRight(WhitespaceCharacters),
		base, out,
		strtoul);
}

bool RKString::ParseValue(float& out) const
{
	const char* stringData = GetString();
	uint32 stringLength = Length();
	int prefixSize = _GetStripSymbolsCountLeft(WhitespaceCharacters);
	int suffixSize = _GetStripSymbolsCountRight(WhitespaceCharacters);	

	stringData += prefixSize;
	stringLength -= prefixSize + suffixSize;

	char* endPtr = NULL;
	double parsedValue = strtod(stringData, &endPtr);

	const bool readSuccessfully = endPtr != stringData;
	const bool nothingRemains = endPtr == stringData + stringLength;

	if (readSuccessfully && nothingRemains)
	{
		out = float(parsedValue);
		return true;
	}
	else
	{
		return false;
	}	
}

void RKString::_AssignJoined(const RKString* partsArray, uint32 partsCount, const char* pSeparator, uint32 separatorLength)
{
	RKASSERT(pSeparator, "Invalid argument, NULL received");

	if (partsCount == 0)
	{
		_Assign("", 0);
		return;
	}	

	if (_IsPartOfThisString(pSeparator))
	{
		RKString dst;
		dst._AssignJoined(partsArray, partsCount, pSeparator, separatorLength);
		Assign(dst);
		return;
	}
	
	_SELFCHECK;

	uint32 totalSize = 0;
	for (uint32 i = 0; i < partsCount; ++i)
	{
		if (&partsArray[i] == this)
		{
			RKString dst;
			dst._AssignJoined(partsArray, partsCount, pSeparator, separatorLength);
			Assign(dst);
			return;
		}
		totalSize += partsArray[i].Length();
	}
	totalSize += (partsCount - 1) * separatorLength;

	char* const buffer = _AcquireWritableStringBuffer(totalSize, false);
	char* ptr = buffer;

	for (uint32 i = 0; i < partsCount; ++i)
	{
		const uint32 partLength = partsArray[i].Length();
		memcpy(ptr, partsArray[i].GetString(), partLength);
		ptr += partLength;
		if (i != (partsCount-1))
		{
			memcpy(ptr, pSeparator, separatorLength);
			ptr += separatorLength;
		}
	}

	RKASSERT(ptr - buffer == totalSize, "Internal error");

	_SELFCHECK;
}

void RKString::_AssignRepeated(const char* pBaseStr, uint32 baseLength, int count, const char* pSeparator, uint32 separatorLength)
{
	RKASSERT(pSeparator, "Invalid argument, NULL received");

	if (count <= 0)
	{
		_Assign("", 0);
		return;
	}

	if (_IsPartOfThisString(pBaseStr) || _IsPartOfThisString(pSeparator))
	{
		RKString dst;
		dst._AssignRepeated(pBaseStr, baseLength, count, pSeparator, separatorLength);
		Assign(dst);
		return;
	}

	_SELFCHECK;

	uint32 totalSize = count * baseLength + (count - 1) * separatorLength;
	
	char* const buffer = _AcquireWritableStringBuffer(totalSize, false);
	char* ptr = buffer;

	for (int i = 0; i < count; ++i)
	{
		memcpy(ptr, pBaseStr, baseLength);
		ptr += baseLength;
		if (i != (count-1))
		{
			memcpy(ptr, pSeparator, separatorLength);
			ptr += separatorLength;
		}
	}

	RKASSERT(ptr - buffer == totalSize, "Internal error");

	_SELFCHECK;
}

void RKString::_Assign(const char* pString, uint32 count)
{
	RKASSERT(pString, "Invalid argument, NULL received");
	_SELFCHECK;

	if (_IsPartOfThisString(pString))
	{
		if (pString == GetString() && count == Length())
		{
			return;
		}
		RKString dst;
		dst.Assign(pString, count);
		Assign(dst);
		return;
	}

	char* dst = _AcquireWritableStringBuffer(count, false);
	memcpy(dst, pString, count);

	_SELFCHECK;
}

void RKString::_Append(const char* pString, uint32 count)
{
	RKASSERT(pString, "Invalid argument, NULL received");
	_SELFCHECK;

	if (_IsPartOfThisString(pString))
	{
		RKString copy;
		copy._Assign(pString, count);
		_Append(copy.GetString(), count);
		return;
	}

	const uint32 initialLength = Length();
	char* dst = _AcquireWritableStringBuffer(initialLength + count, true);
	memcpy(dst + initialLength, pString, count);

	_SELFCHECK;
}

// symbolsToStrip is a string containing symbols to match
// returns count of matching symbols from left to right
int RKString::_GetStripSymbolsCountLeft(const char* symbolsToStrip) const
{
	RKASSERT(symbolsToStrip, "Invalid argument");
	RKASSERT(RKString(symbolsToStrip).IsASCII(), "Stripping UTF characters not supported")
	const uint32 length = Length();
	const char* pString = GetString();
	for (uint32 i = 0; i < length; ++i)
	{
		if (strchr(symbolsToStrip, pString[i]) == NULL)
		{
			return i;
		}
	}
	return length;
}

// symbolsToStrip is a string containing symbols to match
// returns count of matching symbols from right to left
int RKString::_GetStripSymbolsCountRight(const char* symbolsToStrip) const
{
	RKASSERT(symbolsToStrip, "Invalid argument");
	const uint32 length = Length();
	const char* pString = GetString();
	for (uint32 i = 0; i < length; ++i)
	{
		if (strchr(symbolsToStrip, pString[length - i - 1]) == NULL)
		{
			return i;
		}
	}
	return length;
}

bool RKString::_StartsWith(const char* pString, uint32 count) const
{
	RKASSERT(pString, "Invalid argument");
	if (Length() < count) {
		return false;
	}
	const char* ptr = GetString();
	return memcmp(ptr, pString, count) == 0;
}

bool RKString::_EndsWith(const char* pString, uint32 count) const
{
	RKASSERT(pString, "Invalid argument");
	uint32 thisLength = Length();
	if (thisLength < count) {
		return false;
	}
	const char* ptr = GetString();
	return memcmp(ptr + thisLength - count, pString, count) == 0;
}

// region values can be negative
// negative values should be looped, eg -1 becomes length, -2 becomes length-2, etc
// this function normalizes range values and replaces
// negative values with matching positive values
void RKString::_FindHelperNormalizeFromTo(int& from, int& to) const
{
	uint32 currentLength = Length();

	if (from < 0)
	{
		from += currentLength + 1;
	}

	if (to < 0)
	{
		to += currentLength + 1;
	}

	from = std::min(currentLength, uint32(std::max(0, from)));
	to = std::min(currentLength, uint32(std::max(0, to)));
	if (to < from)
	{
		std::swap(to, from);
	}
}

int RKString::_FindFirst(const char* pString, uint32 count, int from, int to) const
{
	RKASSERT(pString, "Invalid argument");
	_FindHelperNormalizeFromTo(from, to);

	if (uint32(to - from) < count)
	{
		return -1;
	}

	const char* currentString = GetString();
	for (uint32 start = from; start <= (to - count); ++start)
	{
		if (memcmp(currentString + start, pString, count) == 0)
		{
			return start;
		}
	}

	return -1;
}

int RKString::_FindLast(const char* pString, uint32 count, int from, int to) const
{
	RKASSERT(pString, "Invalid argument");
	_FindHelperNormalizeFromTo(from, to);

	if (uint32(to - from) < count)
	{
		return -1;
	}

	const char* currentString = GetString();
	for (int start = to - count; start >= from; --start)
	{
		if (memcmp(currentString + start, pString, count) == 0)
		{
			return start;
		}
	}

	return -1;
}

uint32 RKString::_Count(const char* pString, uint32 stringLength) const
{
	RKASSERT(pString, "Invalid argument");
	RKASSERT(stringLength > 0, "Invalid argument");
	
	const uint32 thisLength = Length();
	uint32 foundCount = 0;

	int start = 0;
	while (true)
	{
		start = _FindFirst(pString, stringLength, start, thisLength);
		if (start >= 0)
		{
			start += stringLength;
			++foundCount;
		}
		else
		{
			return foundCount;
		}
	};
}

void RKString::Reverse()
{
	_SELFCHECK;

	const uint32 size = Length();
	char* buffer = _AcquireWritableStringBuffer(size, true);
	std::reverse(buffer, buffer + size);

	_SELFCHECK;
}

void RKString::Fill(char value, int size)
{
	_SELFCHECK;

	if (size == -1)
	{
		size = Length();
	}
	char* buffer = _AcquireWritableStringBuffer(size, false);
	memset(buffer, value, size);

	_SELFCHECK;
}


void RKString::Strip(int leftCount, int rightCount)
{
	_SELFCHECK;

	leftCount = std::max(0, leftCount);
	rightCount = std::max(0, rightCount);

	if (uint32(leftCount + rightCount) >= Length())
	{
		_Assign("", 0);
		return;
	}

	if (_ssoSize != SSO_USE_HEAP_MARK)
	{
		_ssoSize -= (leftCount + rightCount);
		if (leftCount > 0) 
		{ // shift left
			for (uint32 i = 0; i < _ssoSize; ++i)
			{
				_ssoStorage[i] = _ssoStorage[leftCount + i];
			}
		}
		_ssoStorage[_ssoSize] = 0;
	}
	else
	{
		_heapStorage.size -= (leftCount + rightCount);
		_heapStorage.stringPtr += leftCount;
		_heapStorage.stringPtr[_heapStorage.size] = 0;

		// optimize size if necessary
		_AcquireWritableStringBuffer(Length(), true);
	}

	_SELFCHECK;
}

void RKString::_ReplaceRegion(uint32 start, uint32 count, const char* pReplacement, uint32 replacementLength)
{
	RKASSERT(pReplacement, "Invalid argument");
	_SELFCHECK;

	if (_IsPartOfThisString(pReplacement))
	{
		RKString copy;
		copy._Assign(pReplacement, replacementLength);
		_ReplaceRegion(start, count, copy.GetString(), replacementLength);
		return;
	}

	const uint32 initialLength = Length();
	const uint32 suffixLength = initialLength - start - count;

	RKASSERT(start <= initialLength, "Invalid argument");
	RKASSERT(count <= (initialLength - start), "Invalid argument");

	const int lengthDelta = replacementLength - count;

	if (lengthDelta < 0) // shift prefix/suffix shrinking string
	{
		char* initialBuffer = _AcquireWritableStringBuffer(initialLength, true);
		const uint32 prefixLength = start;

		if (prefixLength < suffixLength && _ssoSize == SSO_USE_HEAP_MARK)
		{
			const uint32 prefixOffset = -lengthDelta;
			// shifting prefix forward
			for (int i = prefixLength - 1; i >= 0; --i)
			{
				initialBuffer[i + prefixOffset] = initialBuffer[i];
			}
			_heapStorage.stringPtr += prefixOffset;
		}
		else
		{
			// shifting suffix backward
			for (uint32 i = 0; i < suffixLength; ++i)
			{
				initialBuffer[start + replacementLength + i] = initialBuffer[start + count + i];
			}
		}
	}

	char* buffer = _AcquireWritableStringBuffer(initialLength + lengthDelta, true);

	if (lengthDelta > 0) // shift suffix forward expanding string
	{
		for (int i = suffixLength - 1; i >= 0; --i)
		{
			buffer[start + replacementLength + i] = buffer[start + count + i];
		}
	}

	memcpy(buffer + start, pReplacement, replacementLength);

	_SELFCHECK;
}

bool RKString::_ReplaceFirst(const char* pSubstring, uint32 substringLength, const char* pReplacement, uint32 replacementLength, uint32 maxReplacementCount)
{
	RKASSERT(substringLength > 0, "Invalid argument");

	if (_IsPartOfThisString(pReplacement))
	{
		RKString replacementCopy;
		replacementCopy._Assign(pReplacement, replacementLength);
		return _ReplaceFirst(pSubstring, substringLength, replacementCopy.GetString(), replacementLength, maxReplacementCount);
	}

	if (_IsPartOfThisString(pSubstring))
	{
		RKString substringCopy;
		substringCopy._Assign(pSubstring, substringLength);
		return _ReplaceFirst(substringCopy.GetString(), substringLength, pReplacement, replacementLength, maxReplacementCount);
	}

	int start = 0;
	for (uint32 i = 0; i < maxReplacementCount; ++i)
	{
		int found = _FindFirst(pSubstring, substringLength, start, -1);
		if (found != -1)
		{
			_ReplaceRegion(found, substringLength, pReplacement, replacementLength);
			start = found + replacementLength;
		}
		else
		{
			break;
		}
	}
	return start != 0;
}

bool RKString::_ReplaceLast(const char* pSubstring, uint32 substringLength, const char* pReplacement, uint32 replacementLength, uint32 maxReplacementCount)
{
	RKASSERT(substringLength > 0, "Invalid argument");

	if (_IsPartOfThisString(pReplacement))
	{
		RKString replacementCopy;
		replacementCopy._Assign(pReplacement, replacementLength);
		return _ReplaceLast(pSubstring, substringLength, replacementCopy.GetString(), replacementLength, maxReplacementCount);
	}

	if (_IsPartOfThisString(pSubstring))
	{
		RKString substringCopy;
		substringCopy._Assign(pSubstring, substringLength);
		return _ReplaceLast(substringCopy.GetString(), substringLength, pReplacement, replacementLength, maxReplacementCount);
	}

	const uint32 initialLength = Length();
	int start = initialLength;
	for (uint32 i = 0; i < maxReplacementCount; ++i)
	{
		int found = _FindLast(pSubstring, substringLength, start, 0);
		if (found != -1)
		{
			_ReplaceRegion(found, substringLength, pReplacement, replacementLength);
			start = found;
		}
		else
		{
			break;
		}
	}
	return start != initialLength;
}

void RKString::_SplitFirst(RKList<RKString>& out, const char* pSeparator, uint32 separatorLength, uint32 maxSplitCount, bool keepEmptyParts, bool keepSeparator) const
{
	RKASSERT(pSeparator > 0, "Invalid argument, NULL received");
	RKASSERT(separatorLength > 0, "Invalid argument, empty separator");
	RKASSERT(out.IsEmpty(), "Target array should be empty");
	// TODO: preallocate out array

	const char* const sourceString = GetString();
	const uint32 sourceStringLength = Length();

	if (maxSplitCount == 0)
	{
		out.Append(RKString());
		out[out.Size() - 1]._Assign(sourceString, sourceStringLength);
		return;
	}

	int start = 0;
	for (uint32 i = 0; i < maxSplitCount;)
	{
		int found = _FindFirst(pSeparator, separatorLength, start, -1);
		if (found != -1)
		{
			uint32 partLength = found - start;
			const bool shouldBeSkipped = (partLength == 0) && !keepEmptyParts;
			if (!shouldBeSkipped)
			{
				if (keepSeparator)
				{
					partLength += separatorLength;
				}

				out.Append(RKString());
				out[out.Size() - 1]._Assign(sourceString + start, partLength);
				++i;
			}

			start = found + separatorLength;
		}
		else
		{
			break;
		}
	}

	if (!keepEmptyParts) // if cycle ended early, we have to remove all trailing separators from suffix
	{
		while (memcmp(sourceString + start, pSeparator, separatorLength) == 0)
		{
			start += separatorLength;
		}
	}

	if ((start < int(sourceStringLength)) || (keepEmptyParts && (start == sourceStringLength)))
	{
		out.Append(RKString());
		out[out.Size() - 1]._Assign(sourceString + start, sourceStringLength - start);
	}
}

void RKString::_SplitLast(RKList<RKString>& out, const char* pSeparator, uint32 separatorLength, uint32 maxSplitCount, bool keepEmptyParts, bool keepSeparator) const
{
	RKASSERT(pSeparator > 0, "Invalid argument, NULL received");
	RKASSERT(separatorLength > 0, "Invalid argument, empty separator");
	RKASSERT(out.IsEmpty(), "Target array should be empty");
	// TODO: preallocate out array

	const char* const sourceString = GetString();
	const uint32 sourceStringLength = Length();

	if (maxSplitCount == 0)
	{
		out.Append(RKString());
		out[out.Size() - 1]._Assign(sourceString, sourceStringLength);
		return;
	}

	int start = sourceStringLength;
	for (uint32 i = 0; i < maxSplitCount;)
	{
		int found = _FindLast(pSeparator, separatorLength, start, 0);
		if (found != -1)
		{
			uint32 partWithSeparatorLength = start - found;
			const bool shouldBeSkipped = ((partWithSeparatorLength - separatorLength) == 0) && !keepEmptyParts;
			if (!shouldBeSkipped)
			{
				int partStart = found;
				int partLength = partWithSeparatorLength;
				if (!keepSeparator)
				{
					partLength -= separatorLength;
					partStart += separatorLength;
				}
				
				out.Append(RKString());
				out[out.Size() - 1]._Assign(sourceString + partStart, partLength);
				++i;
			}

			start = found;
		}
		else
		{
			break;
		}
	}

	if (!keepEmptyParts) // if cycle ended early, we have to remove all trailing separators from suffix
	{
		while (((start - int(separatorLength)) >= 0) && (memcmp(sourceString + start - separatorLength, pSeparator, separatorLength) == 0))
		{
			start -= separatorLength;
		}
	}

	if ((start > 0) || ((start == 0) && keepEmptyParts))
	{
		out.Append(RKString());
		out[out.Size() - 1]._Assign(sourceString, start);
	}

	// TODO: get rid of reverse
	std::reverse(&out[0], &out[0] + out.Size());
}

uint32	decodeNextUnicodeCharacter(const char** utf8_buffer)
{
  uint32	uc;
  char	c;
  
  // Security considerations:
  //
  // If we hit a zero byte, we want to return 0 without stepping
  // the buffer pointer past the 0.
  //
  // If we hit an "overlong sequence"; i.e. a character encoded
  // in a longer multibyte string than is necessary, then we
  // need to discard the character.  This is so attackers can't
  // disguise dangerous characters or character sequences --
  // there is only one valid encoding for each character.
  //
  // If we decode characters { 0xD800 .. 0xDFFF } or { 0xFFFE,
  // 0xFFFF } then we ignore them; they are not valid in UTF-8.
  
  // This isn't actually an invalid character; it's a valid char that
  // looks like an inverted question mark.
#ifndef RK_INVALID_UNICODE
#define RK_INVALID_UNICODE 0x0FFFD
#endif
  
#define FIRST_BYTE(mask, shift)		\
uc = (c & (mask)) << (shift);
  
#define NEXT_BYTE(shift)						\
c = **utf8_buffer;						\
if (c == 0) return 0; /* end of buffer, do not advance */	\
if ((c & 0xC0) != 0x80) return RK_INVALID_UNICODE; /* standard check */	\
(*utf8_buffer)++;						\
uc |= (c & 0x3F) << shift;
  
  c = **utf8_buffer;
  if (c == 0) return 0;	// End of buffer.  Do not advance.
  
  (*utf8_buffer)++;
  if ((c & 0x80) == 0) return (uint32) c;	// Conventional 7-bit ASCII.
  
  // Multi-byte sequences.
  if ((c & 0xE0) == 0xC0)
  {
    // Two-byte sequence.
    FIRST_BYTE(0x1F, 6);
    NEXT_BYTE(0);
    if (uc < 0x80)
      return RK_INVALID_UNICODE;	// overlong
    return uc;
  }
  else if ((c & 0xF0) == 0xE0)
  {
    // Three-byte sequence.
    FIRST_BYTE(0x0F, 12);
    NEXT_BYTE(6);
    NEXT_BYTE(0);
    if (uc < 0x800) return RK_INVALID_UNICODE;	// overlong
    if (uc >= 0x0D800 && uc <= 0x0DFFF) return RK_INVALID_UNICODE;	// not valid ISO 10646
    if (uc == 0x0FFFE || uc == 0x0FFFF) return RK_INVALID_UNICODE;	// not valid ISO 10646
    return uc;
  }
  else if ((c & 0xF8) == 0xF0)
  {
    // Four-byte sequence.
    FIRST_BYTE(0x07, 18);
    NEXT_BYTE(12);
    NEXT_BYTE(6);
    NEXT_BYTE(0);
    if (uc < 0x010000) return RK_INVALID_UNICODE;	// overlong
    return uc;
  }
  else if ((c & 0xFC) == 0xF8)
  {
    // Five-byte sequence.
    FIRST_BYTE(0x03, 24);
    NEXT_BYTE(18);
    NEXT_BYTE(12);
    NEXT_BYTE(6);
    NEXT_BYTE(0);
    if (uc < 0x0200000) return RK_INVALID_UNICODE;	// overlong
    return uc;
  }
  else if ((c & 0xFE) == 0xFC)
  {
    // Six-byte sequence.
    FIRST_BYTE(0x01, 30);
    NEXT_BYTE(24);
    NEXT_BYTE(18);
    NEXT_BYTE(12);
    NEXT_BYTE(6);
    NEXT_BYTE(0);
    if (uc < 0x04000000) return RK_INVALID_UNICODE;	// overlong
    return uc;
  }
  else
  {
    // Invalid.
    return RK_INVALID_UNICODE;
  }
}

wchar_t* RKString::ToUnicode() const
{
  wchar_t* pUnicodeString = NULL;
  
  if (!IsEmpty())
  {
    pUnicodeString = RKNEWARRAY(wchar_t, Length()+1);
    int characterIndex = 0;
    const char*	text = &GetString()[0];
    
    while (uint16 code = (uint16)decodeNextUnicodeCharacter(&text))
    {
      if (code != RK_INVALID_UNICODE)
        pUnicodeString[characterIndex++] = (wchar_t)code;
      else
        pUnicodeString[characterIndex++] = (wchar_t)32;
    }
    
    pUnicodeString[characterIndex] = 0;
  }
  
  return pUnicodeString;
}

uint32 RKString::LengthUTF() const
{
	int length = 0;
	IteratorUTF32 it  = BeginUTF32();
	IteratorUTF32 end = EndUTF32();
	for (; it != end; ++it)
	{
		++length;
	}
	return length;
}

bool RKString::IsASCII() const
{
	IteratorUTF32 it  = BeginUTF32();
	IteratorUTF32 end = EndUTF32();
	for (; it != end; ++it)
	{
		if (it.CurrentCodeLength() > 1)
		{
			return false;
		}
	}
	return true;
}

bool RKString::IsValidUTF() const
{
	IteratorUTF32 it  = BeginUTF32();
	IteratorUTF32 end = EndUTF32();
	for (; it != end; ++it)
	{
		if (!it.IsCurrentCodeValid())
		{
			return false;
		}
	}
	return true;
}

void RKString::ToLowerCase()
{
	_SELFCHECK;
	RKASSERT(IsASCII(), "UTF strings ToLower/ToUpper not supported");

	char* pStr = _AcquireWritableStringBuffer(Length(), true);
  while (*pStr)
  {
    *pStr = RKString_ToLowerCase(*pStr);
    pStr++;
  }

	_SELFCHECK;
}

void RKString::ToUpperCase()
{
	_SELFCHECK;
	RKASSERT(IsASCII(), "UTF strings ToLower/ToUpper not supported");

  char* pStr = _AcquireWritableStringBuffer(Length(), true);
  while (*pStr)
  {
    *pStr = RKString_ToUpperCase(*pStr);
    pStr++;
  }

	_SELFCHECK;
}

void RKString::_Squeeze()
{
	_SELFCHECK;

	_AcquireWritableStringBuffer(Length(), true, true);

	_SELFCHECK;
}

void RKString::_Reserve(uint32 minimumBufferSize)
{
	_SELFCHECK;

	_AcquireWritableStringBuffer(Length(), true);

	_SELFCHECK;
}

bool RKString::operator==(const wchar_t* pString) const
{
	IteratorUTF32 it  = BeginUTF32();
	IteratorUTF32 end = EndUTF32();
	uint32 idx = 0;
	uint32 stringLength = wcslen(pString);
	for (; idx < stringLength && it != end; ++idx, ++it)
	{
		if (!it.IsCurrentCodeValid())
		{
			return false;
		}
		if (*it != pString[idx])
		{
			return false;
		}
	}
	return it == end && idx == stringLength;
}

RKString operator +(const RKString& pStringA, const RKString& pStringB)
{
  RKString tmp = pStringA;
  tmp.Append(pStringB);
  return tmp;
}

RKString operator +(const char* pStringA, const RKString& pStringB)
{
	RKString tmp = pStringA;
	tmp.Append(pStringB);
	return tmp;
}

RKString operator +(const RKString& pStringA, const char* pStringB)
{
	RKString tmp = pStringA;
	tmp.Append(pStringB);
	return tmp;
}

bool operator ==(const char* pStringA, const RKString& pStringB)
{
	return pStringB == pStringA;
}

bool operator !=(const char* pStringA, const RKString& pStringB)
{
	return pStringB != pStringA;
}

std::ostream& operator << (std::ostream& o, const RKString& str)
{
	o << str.GetString();
	return o;
}

std::wostream& operator << (std::wostream&  o, const RKString& str)
{
	RKString::IteratorWCHAR it  = str.BeginWCHAR();
	RKString::IteratorWCHAR end = str.EndWCHAR();
	for (; it != end; ++it)
	{
		o << *it;
	}
	return o;
}
}