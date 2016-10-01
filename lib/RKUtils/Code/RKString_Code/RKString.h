#ifndef __RKSTRING_H_
#define __RKSTRING_H_
	/*------------------------------------------------------------------------
	RKString class

	1. Constructors:
	can be created from any following type of string:
	RKString (copy ctor)
	const char*
	std::string
	const wchar_t*
	std::wstring

	2. Make/Assign functions:
	Make* and Assign* functions are paired.
	For each Assign* function there is a corresponding Make* function
	Assign* function assigns some value to string
	Make* function returns new string with value assigned

	2.1. MakeSubstring/Assign, Make*FromUCS/UTF, AssignUCS/UTF, isntance.Substring:
	Assigns string or substring

	Accepts one argument, string as a pointer to:
	char (utf8 or askii), wchar_t, uint16 (UCS) or uint32 (UTF32)

	If called with two arguments, then second argument represents
	count of elements to be assigned, in case of wide-char strings
	it's not count of bytes, but count of characters

	If called with three arguments, then second argument represents
	start position and third argument represents count of elements
	to be assigned
	In such case assigns 'count' string elements starting from 'start' position

	2.2. MakeFormatted/AssignFormatted
	Assigns sprintf formatted string

	Accepts variable count of arguments
	First arguments is a format string, the rest are sprintf arguments

	2.3. MakeWithValue/AssignValue
	Assigns numeric value

	Accepts int / uint or float values

	For decimal values two additional arguments can be provided

	First additional argument is base, can be 8, 10 or 16
	Second additional argument is a boolean flag that specify
	whether or not base prefix should be printed (0x for base 16, 0 for base 8)

	2.4. MakeJoined/AssignJoined
	Concatenates values of string array with separator between elements

	Accepts string array as RKList<RKString> or RKString* as a first argument
	Accepts separator string as a second argument

	2.5. MakeRepeated/AssignRepeated, isntance.Repeated
	Creates string by repeating string N times
	Optionally placing separator between strings

	Accepts string to be repeated as a first argument
	Accepts count of repeats as a second argument
	Optionally accepts separator as a third argument

	2.6. MakeFilled/Fill
	MakeFilled creates string of specified length by filling it with specified character

	Fill resizes string to specified length and fill it with specified character
	Then count of characters to fill can be omitted, in such case preserve current length

	3. Append/Prepend/Insert, AppendChar/PrependChar/InsertChar
	Appends, prepends or inserts string, substring or character.

	Each function accepts:
	RKString, char, 'const char*' or 'const char*' substring

	Insert functions also accept position to insert to as a first argument

	4. Base string functions
	uint32 Length() - returns length of string       (length value is cached inside of string, so it's fast)
	bool   Empty()  - returns true if string is empty (a bit faster than Length() == 0 check)
	char*  GetString()   - returns pointer to contained string
	uint32 ComputeHash() - calculates and returns hash value as uint32

	5. Base UTF support functions:
	uint32 LengthUTF()  - returns count of UTF32 codes encoded into UTF8 string
	bool   IsASCII()    - returns true if string contains only ASCII codes
	bool   IsValidUTF() - returns true if string is a valid UTF8 string and contains only valid UTF32 codes

	6. Prefix/suffix checks
	bool StartsWith() - accepts string and returns true if this string starts with provided string
	bool EndsWith()   - accepts string and returns true if this string starts with provided string

	7. Search functions
	7.1. FindFirst/FindLast functions
	FindFirst/FindLast function searches for a first/last occurrence of substring

	Accepts substring to search as a first argument
	Optionally accepts one or two arguments specifying range of search (see following)

	Returns index of substring occurrence found or -1 if substring was not found

	Search range is specified by two indices:
	from - position to start from
	to   - end of subregion to search, search function never access element with index 'to'
	You can use negative values to spcify range, in such case
	-1 means length
	-2 means length-1
	-3 means length-3
	and so on
	Search range values can be swapped (eg 'from' can be > than 'to')

	7.2. Contains/Count
	Contains returns true if string contains specified substring

	Count returns count non overlapping occurrences of substring in string (searched from left to right)

	8. Comparison, Compare/ICompare
	Compare() compares function lexicographically and returns:
	-1 if this string less than provided string
	0 if strings are equal
	1 if this string greater than provided string

	ICompare does the same thing but performs case-insensitive comparison,
	WARNING: case-insensitive UTF is not supported!

	9. Special inplace editing functions
	ToLowerCase() - modifies string inplace, lowercase all characters, DOES NOT SUPPORT UTF! (asserted)
	ToUpperCase() - modifies string inplace, uppercase all characters, DOES NOT SUPPORT UTF! (asserted)
	Reverse() - reverse string inplace, DOES NOT SUPPORT UTF! (asserted)
	Fill()    - resizes string and fill it with a character (see MakeFilled)
	Clear()   - assigns empty string

	10. Strips
	10.1. Numeric strips:
	StripLeft(N)  - strips N symbols from left side of the string
	StripRight(N) -	strips N symbols from right side of the string
	Strip(N, M) -
	strips N symbols from left side of the string
	and M symbols from right side of the string

	10.2. Whitespace strips:
	Strip()      - strip all whitespace characters from both sides of string
	StripLeft () - strip all whitespace characters from left side of string
	StripRight() - strip all whitespace characters from right side of string

	These function can optionally accept one argument, containing
	characters to be considered whitespace characters as a string

	It's safe to strip UTF strings, but stripping non-ASCII characters is not supported!

	NOTE: Front strips are efficient and don't require reallocation

	11. Replace/Remove
	11.1. ReplaceFirst/ReplaceLast:
	ReplaceFirst/ReplaceLast accepts two strings, 'pattern' an 'replacement',
	and replace first/last occurrence of 'pattern' substring with 'replacement' string

	Also it optionally accepts third argument that specify count of replacement to be made
	In such case it replaces first/last N occurrences of 'pattern' substring

	By default only one replacement made

	Returns true if any replacements had been made

	11.2. ReplaceAll
	Accepts two arguments and replaces all occurrences of
	'pattern' substring with 'replacement' substring

	Returns true if any replacements had been made

	11.3. ReplaceRegion
	Replaces part of string with another string
	Accepts three arguments:
	start - position of replacement, can be [0..length], length inclusive (when start==length and count=0 it append string)
	count - count of symbols to replace, can be 0
	replacement - replacement string

	11.3. Remove
	Works like ReplaceRegion with an empty replacement string

	12. Parts querying, return new string
	Substring(start, count) - returns substring starting form 'start' position and containing 'count' elements
	Left(N)  - returns first N characters as a string
	Right(N) - returns last N characters as a string
	Repeated(N) - returns self string repeated N times with optional separator (see AssignRepeated)

	13. Casts
	int32  ToInt([base])  - returns parsed int value,
	optionally accepts base argument, by default autodetects base by prefix
	asserts if can't be converted
	uint32 ToUInt([base]) - returns parsed int value,
	optionally accepts base argument, by default autodetects base by prefix
	asserts if can't be converted
	float ToFloat() - returns parsed float value, asserts if can't be converted

	bool ParseValue(out, [base]) const -
	tries to parse as an int value, returns true if successful
	output stored in first argument
	optionally accepts base argument, by default autodetects base by prefix

	bool ParseValue(out, [base]) const
	tries to parse as an unsigned int value, returns true if successful
	output stored in first argument
	optionally accepts base argument, by default autodetects base by prefix

	bool ParseValue(out) const;
	tries to parse as float value, returns true if successful
	output stored in first argument

	14. Splits
	SplitFirst/SplitLast -
	creates an array of strings by splitting string with provided separator

	have version that returns resulting array and a version
	that accepts output array ref as a first argument

	By default uses space as a separator

	Accepts optional argument maxSplitCount
	in such case makes no more than maxSplitCount splits
	and returns not more than maxSplitCount+1 parts
	by default is unlimited

	Accepts optional boolean flag keepEmptyParts:
	if true all empty parts will be kept
	by default ignores empty parts

	Accepts optional boolean flag keepSeparator:
	if true parts will contain separator itself
	by default removes separators

	Split equal to SplitFirst with unlimited splits count

	15. UTF8 casts/iteration
	Provides four iterators and four pairs of begin/end-like functions

	IteratorUTF32 (and functions BeginUTF32 / EndUTF32)
	IteratorUCS   (and functions BeginUCS   / EndUCS)
	IteratorWCHAR (and functions BeginWCHAR / EndWCHAR)
	IteratorTruncatedASCII (and functions BeginTruncatedASCII / EndTruncatedASCII)

	These read-only iterators can be used to traverse and decode UTF string.
	When traversed as TruncatedASCII all UTF symbols will be replaced with '#' symbol.

	CreateDecodedBufferUTF32/UCS/WCHAR/ASCII
	- allocates and returns buffer of matching type containing
	decoded UTF values (possibly truncated)
	WARNING! you'll have to delete this buffer yourself!

	FillDecodedBufferUTF32/UCS/WCHAR/ASCII
	- accepts pointer to buffer as a first argument and buffer size as a second argument
	returns true if target buffer had enough space to convert all string
	WARNING buffer size is not a size in bytes, but a count of elements

	16. Memory-related, potentially dangerous operations
	_Squeeze() - reallocates buffer to use least possible amount of memory
	_Reserve() - allocates buffer of specified size
	_GetDynamicBufferSize() - returns size of dynamic buffer allocated (if any)

	17. Operators:
	supported operators: ==, !=, <, =, +=

	18. Public static fields:
	RKString EmptyString - empty string
	const char* WhitespaceCharacters - default set of whitespace characters for stripping: " \n\t";

	TODO:
	1. Splits and especially SplitRight are very inefficient, need to preallocate buffer
	2. Add matching Append/Prepend/Insert calls for every Assign call, add AssignChar, MakeCharString?
	3. +=(wchar) does unnecessary copy
	4. Add RKString RKString::Slice(from, to), like substring but not substring =)
	5. ToLower/ToUpper/Strip/Reverse UTF support
	6. Is Finalize acutally necessary? Maybe get rid of it?
	7. LengthUTF/IsASCII can be faster, don't have to decode symbols there
	------------------------------------------------------------------------*/

#include <string>
#include <algorithm>
#include <iostream>
#include "RKStringFunctions.h"
#include "RKStringUTF.h"
#include "../RKTool_Code/RKList.h"
#include "../Common/RKTypes.h"
#include "../Common/RK.h"
using namespace std;
namespace RKUtils
{
#undef max
#undef min
#ifdef GAME_DEBUG
	//#  define _RKSTRING_AUTOTEST
#endif
	class _RKStringTestClass; // forward declaration of a class that contains unit tests for RKString

	class RKString
	{
		friend class _RKStringTestClass; // unit tests require access to some private members
	public:
		// ctors/dtor
		RKString();
		RKString(const RKString& str);
		RKString(RKString&& str);
		RKString(const char* pString);
		RKString(const std::string & str);
		RKString(const wchar_t* pString);
		RKString(const std::wstring& str);
		~RKString();

		// factories
		static RKString MakeSubstring(const char*    pString, int count)								     { RKString str; str.Assign(pString, count); return str; }
		static RKString MakeSubstring(const wchar_t* pString, int count)								     { RKString str; str.Assign(pString, count); return str; }
		static RKString MakeSubstring(const char*    pString, int start, int count)			     { RKString str; str.Assign(pString, start, count); return str; }
		static RKString MakeSubstring(const wchar_t* pString, int start, int count)			     { RKString str; str.Assign(pString, start, count); return str; }
		static RKString MakeFromUCS(const uint16*  pString)												         { RKString str; str.AssignUCS(pString); return str; }
		static RKString MakeFromUTF32(const uint32*  pString)                                { RKString str; str.AssignUTF32(pString); return str; }
		static RKString MakeSubstringFromUCS(const uint16* pString, int count)             { RKString str; str.AssignUCS(pString, count); return str; }
		static RKString MakeSubstringFromUTF32(const uint32* pString, int count)             { RKString str; str.AssignUTF32(pString, count); return str; }
		static RKString MakeSubstringFromUCS(const uint16* pString, int start, int count)  { RKString str; str.AssignUCS(pString, start, count); return str; }
		static RKString MakeSubstringFromUTF32(const uint32* pString, int start, int count)  { RKString str; str.AssignUTF32(pString, start, count); return str; }

		static RKString MakeFormatted(const char* format, ...);
		static RKString MakeWithValue(int          value, int base = 10, bool useBasePrefix = true)					  { RKString str; str.AssignValue(value, base, useBasePrefix); return str; }
		static RKString MakeWithValue(unsigned int value, int base = 10, bool useBasePrefix = true)						{ RKString str; str.AssignValue(value, base, useBasePrefix); return str; }
		static RKString MakeWithValue(float        value)																											{ RKString str; str.AssignValue(value); return str; }
		static RKString MakeJoined(const RKList<RKString>& parts, const char* pSeparator)     { RKString str; str.AssignJoined(parts, pSeparator); return str; }
		static RKString MakeJoined(const RKList<RKString>& parts, const RKString& separator)  { RKString str; str.AssignJoined(parts, separator); return str; }
		static RKString MakeJoined(const RKString* partsArray, uint32 partsCount, const char* pSeparator)     { RKString str; str.AssignJoined(partsArray, partsCount, pSeparator); return str; }
		static RKString MakeJoined(const RKString* partsArray, uint32 partsCount, const RKString& separator)  { RKString str; str.AssignJoined(partsArray, partsCount, separator); return str; }
		static RKString MakeRepeated(const char* pBaseStr, int count, const char* pSeparator = "")            { RKString str; str.AssignRepeated(pBaseStr, count, pSeparator); return str; }
		static RKString MakeRepeated(const char* pBaseStr, int count, const RKString& separator)              { RKString str; str.AssignRepeated(pBaseStr, count, separator); return str; }
		static RKString MakeFilled(char value, uint32 size)																									{ RKString str; str.Fill(value, size); return str; }

		// assignments
		void Assign(const RKString& str)											     { _Assign(str.GetString(), str.Length()); }
		void Assign(const char*    pString)											 { _Assign(pString, _ZeroTerminatedBufferLength(pString)); }
		void Assign(const wchar_t* pString)                       { _AssignUTF(pString, _ZeroTerminatedBufferLength(pString)); }
		void AssignUCS(const uint16*  pString)                       { _AssignUTF(pString, _ZeroTerminatedBufferLength(pString)); }
		void AssignUTF32(const uint32*  pString)                       { _AssignUTF(pString, _ZeroTerminatedBufferLength(pString)); }
		void Assign(const char*    pString, int count)            { _Assign(pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void Assign(const wchar_t* pString, int count)            { _AssignUTF(pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void AssignUCS(const uint16*  pString, int count)            { _AssignUTF(pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void AssignUTF32(const uint32*  pString, int count)            { _AssignUTF(pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void Assign(const char*    pString, int start, int count) { _Assign(pString + std::max(0, start), _StringCharacterCountToProcess(pString, start, count)); }
		void Assign(const wchar_t* pString, int start, int count) { _AssignUTF(pString + std::max(0, start), _StringCharacterCountToProcess(pString, start, count)); }
		void AssignUCS(const uint16*  pString, int start, int count) { _AssignUTF(pString + std::max(0, start), _StringCharacterCountToProcess(pString, start, count)); }
		void AssignUTF32(const uint32*  pString, int start, int count) { _AssignUTF(pString + std::max(0, start), _StringCharacterCountToProcess(pString, start, count)); }

		void AssignFormatted(const char* pFormat, ...);
		void AssignValue(int          value, int base = 10, bool useBasePrefix = true)               { _AssignLongValue(value, base, useBasePrefix); }
		void AssignValue(unsigned int value, int base = 10, bool useBasePrefix = true)               { _AssignLongValue((unsigned long)value, base, useBasePrefix); }
		void AssignValue(float        value)                                                         { AssignFormatted("%g", value); }
		void AssignJoined(const RKList<RKString>& parts, const char* pSeparator)     { if (parts.IsEmpty()) _Assign("", 0); else _AssignJoined(&parts[0], parts.Size(), pSeparator, RKString_Length(pSeparator)); }
		void AssignJoined(const RKList<RKString>& parts, const RKString& separator)  { if (parts.IsEmpty()) _Assign("", 0); else _AssignJoined(&parts[0], parts.Size(), separator.GetString(), separator.Length()); }
		void AssignJoined(const RKString* partsArray, uint32 partsCount, const char* pSeparator)     { _AssignJoined(partsArray, partsCount, pSeparator, RKString_Length(pSeparator)); }
		void AssignJoined(const RKString* partsArray, uint32 partsCount, const RKString& separator)  { _AssignJoined(partsArray, partsCount, separator.GetString(), separator.Length()); }
		void AssignRepeated(const RKString& base, int count, const char* pSeparator = "")            { _AssignRepeated(base.GetString(), base.Length(), count, pSeparator, RKString_Length(pSeparator)); }
		void AssignRepeated(const RKString& base, int count, const RKString& separator)              { _AssignRepeated(base.GetString(), base.Length(), count, separator.GetString(), separator.Length()); }
		void AssignRepeated(const char* pBaseStr, int count, const char* pSeparator = "")            { _AssignRepeated(pBaseStr, RKString_Length(pBaseStr), count, pSeparator, RKString_Length(pSeparator)); }
		void AssignRepeated(const char* pBaseStr, int count, const RKString& separator)              { _AssignRepeated(pBaseStr, RKString_Length(pBaseStr), count, separator.GetString(), separator.Length()); }


		// appends
		void Append(const RKString& str)											 { _Append(str.GetString(), str.Length()); }
		void Append(const char* pString)											 { _Append(pString, RKString_Length(pString)); }
		void Append(const char* pString, int count)            { _Append(pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void Append(const char* pString, int start, int count) { _Append(pString + std::max(0, start), _StringCharacterCountToProcess(pString, start, count)); }
		void AppendChar(char cSingleChar)											 { _Append(&cSingleChar, 1); }

		// insert
		void Insert(int position, const RKString& str)                       { _Insert(position, str.GetString(), str.Length()); }
		void Insert(int position, const char* pString)                       { _Insert(position, pString, RKString_Length(pString)); }
		void Insert(int position, const char* pString, int count)            { _Insert(position, pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void Insert(int position, const char* pString, int start, int count) { _Insert(position, pString + std::max(0, start), _StringCharacterCountToProcess(pString, 0, count)); }
		void InsertChar(int position, char cSingleChar)											 { _Insert(position, &cSingleChar, 1); }

		// prepend
		void Prepend(const RKString& str)                       { _Insert(0, str.GetString(), str.Length()); }
		void Prepend(const char* pString)                       { _Insert(0, pString, RKString_Length(pString)); }
		void Prepend(const char* pString, int count)            { _Insert(0, pString, _StringCharacterCountToProcess(pString, 0, count)); }
		void Prepend(const char* pString, int start, int count) { _Insert(0, pString + std::max(0, start), _StringCharacterCountToProcess(pString, 0, count)); }
		void PrependChar(char cSingleChar)			                { _Insert(0, &cSingleChar, 1); }


		// base functions
		uint32      Length()  const														 { return _ssoSize == SSO_USE_HEAP_MARK ? _heapStorage.size : _ssoSize; }
		bool        IsEmpty() const														 { return _ssoSize == 0; }
		const char*	GetString() const													 { return _ssoSize == SSO_USE_HEAP_MARK ? _heapStorage.stringPtr : _ssoStorage; }
		uint32      ComputeHash() const                        { return RKString_CreateHash(GetString()); }

		// base utf functions
		uint32 LengthUTF() const;
		bool   IsASCII() const;
		bool   IsValidUTF() const;

		// prefix/suffix checks
		bool StartsWith(const RKString& str) const             { return _StartsWith(str.GetString(), str.Length()); }
		bool StartsWith(const char* pString) const						 { return _StartsWith(pString, RKString_Length(pString)); }
		bool EndsWith(const RKString& str) const               { return _EndsWith(str.GetString(), str.Length()); }
		bool EndsWith(const char* pString) const						   { return _EndsWith(pString, RKString_Length(pString)); }

		// search functions, uses cycled from/to, eg -1 <==> length, -2 <==> length-1 and so all
		int  FindFirst(const RKString& str, int from = 0, int to = -1) const { return _FindFirst(str.GetString(), str.Length(), from, to); }
		int  FindFirst(const char* pString, int from = 0, int to = -1) const { return _FindFirst(pString, RKString_Length(pString), from, to); }
		int  FindLast(const RKString& str, int from = -1, int to = 0) const { return _FindLast(str.GetString(), str.Length(), from, to); }
		int  FindLast(const char* pString, int from = -1, int to = 0) const { return _FindLast(pString, RKString_Length(pString), from, to); }
		bool Contains(const RKString& str) const                            { return  FindFirst(str) != -1; }
		bool Contains(const char* pString) const                            { return  FindFirst(pString) != -1; }
		uint32 Count(const RKString& str) const                            { return _Count(str.GetString(), str.Length()); }
		uint32 Count(const char* pString) const                            { return _Count(pString, RKString_Length(pString)); }

		// comparison
		int Compare(const RKString& str) const                { return RKString_Compare(GetString(), str.GetString()); }
		int Compare(const char* pString) const                { return RKString_Compare(GetString(), pString); }
		int ICompare(const RKString& str) const                { return RKString_ICompare(GetString(), str.GetString()); }
		int ICompare(const char* pString) const                { return RKString_ICompare(GetString(), pString); }

		// some special editing functions
		void ToLowerCase();
		void ToUpperCase();
		void Reverse();
		void Fill(char value, int size = -1);
		void Clear()																											 { _Assign("", 0); }

		// strips
		void Strip(int leftCount, int rightCount);
		void StripLeft(int count)														             { Strip(count, 0); }
		void StripRight(int count)														             { Strip(0, count); }
		void Strip(const char* symbolsToStrip = WhitespaceCharacters) { Strip(_GetStripSymbolsCountLeft(symbolsToStrip), _GetStripSymbolsCountRight(symbolsToStrip)); }
		void StripLeft(const char* symbolsToStrip = WhitespaceCharacters) { StripLeft(_GetStripSymbolsCountLeft(symbolsToStrip)); }
		void StripRight(const char* symbolsToStrip = WhitespaceCharacters) { StripRight(_GetStripSymbolsCountRight(symbolsToStrip)); }

		// replace
		bool ReplaceFirst(const RKString& substring, const RKString& replacement, uint32 maxReplacementCount = 1) { return _ReplaceFirst(substring.GetString(), substring.Length(), replacement.GetString(), replacement.Length(), maxReplacementCount); }
		bool ReplaceFirst(const RKString& substring, const char*    pReplacement, uint32 maxReplacementCount = 1) { return _ReplaceFirst(substring.GetString(), substring.Length(), pReplacement, RKString_Length(pReplacement), maxReplacementCount); }
		bool ReplaceFirst(const char*    pSubstring, const RKString& replacement, uint32 maxReplacementCount = 1) { return _ReplaceFirst(pSubstring, RKString_Length(pSubstring), replacement.GetString(), replacement.Length(), maxReplacementCount); }
		bool ReplaceFirst(const char*    pSubstring, const char*    pReplacement, uint32 maxReplacementCount = 1) { return _ReplaceFirst(pSubstring, RKString_Length(pSubstring), pReplacement, RKString_Length(pReplacement), maxReplacementCount); }
		bool ReplaceLast(const RKString& substring, const RKString& replacement, uint32 maxReplacementCount = 1) { return _ReplaceLast(substring.GetString(), substring.Length(), replacement.GetString(), replacement.Length(), maxReplacementCount); }
		bool ReplaceLast(const RKString& substring, const char*    pReplacement, uint32 maxReplacementCount = 1) { return _ReplaceLast(substring.GetString(), substring.Length(), pReplacement, RKString_Length(pReplacement), maxReplacementCount); }
		bool ReplaceLast(const char*    pSubstring, const RKString& replacement, uint32 maxReplacementCount = 1) { return _ReplaceLast(pSubstring, RKString_Length(pSubstring), replacement.GetString(), replacement.Length(), maxReplacementCount); }
		bool ReplaceLast(const char*    pSubstring, const char*    pReplacement, uint32 maxReplacementCount = 1) { return _ReplaceLast(pSubstring, RKString_Length(pSubstring), pReplacement, RKString_Length(pReplacement), maxReplacementCount); }
		bool ReplaceAll(const RKString& substring, const RKString& replacement)																 { return _ReplaceFirst(substring.GetString(), substring.Length(), replacement.GetString(), replacement.Length(), -1); }
		bool ReplaceAll(const RKString& substring, const char*    pReplacement)																 { return _ReplaceFirst(substring.GetString(), substring.Length(), pReplacement, RKString_Length(pReplacement), -1); }
		bool ReplaceAll(const char*    pSubstring, const RKString& replacement)																 { return _ReplaceFirst(pSubstring, RKString_Length(pSubstring), replacement.GetString(), replacement.Length(), -1); }
		bool ReplaceAll(const char*    pSubstring, const char*    pReplacement)																 { return _ReplaceFirst(pSubstring, RKString_Length(pSubstring), pReplacement, RKString_Length(pReplacement), -1); }
		void ReplaceRegion(int start, int count, const RKString& replacement)																 { return _ReplaceRegion(std::max(0, start), _StringCharacterCountToProcess(GetString(), start, count), replacement.GetString(), replacement.Length()); }
		void ReplaceRegion(int start, int count, const char*    pReplacement)																 { return _ReplaceRegion(std::max(0, start), _StringCharacterCountToProcess(GetString(), start, count), pReplacement, RKString_Length(pReplacement)); }
		void Remove(int start, int count)                                                                   { return _ReplaceRegion(std::max(0, start), _StringCharacterCountToProcess(GetString(), start, count), "", 0); }

		// querying string parts
		RKString Substring(int start, int count) const                  { return MakeSubstring(GetString(), start, count); }
		RKString Left(int count) const                                  { return MakeSubstring(GetString(), 0, count); }
		RKString Right(int count) const                                 { return MakeSubstring(GetString(), Length() - count, count); }
		RKString Repeated(int count, const char* pSeparator = "") const { return MakeRepeated(GetString(), count, pSeparator); }
		RKString Repeated(int count, const RKString& separator)   const { return MakeRepeated(GetString(), count, separator); }

		// casts, base=0 means autodetection based on prefix (0x?? for hex, 0?? for oct)
		int          ToInt(int base = 0) const { int          out = 0; bool success = ParseValue(out, base); RKASSERT(success, "String conversion failed"); return out; }
		unsigned int ToUInt(int base = 0) const { unsigned int out = 0; bool success = ParseValue(out, base); RKASSERT(success, "String conversion failed"); return out; }
		float        ToFloat() const            { float        out = 0; bool success = ParseValue(out);       RKASSERT(success, "String conversion failed"); return out; }
		bool ParseValue(int&          out, int base = 0) const;
		bool ParseValue(unsigned int& out, int base = 0) const;
		bool ParseValue(float& out) const;

		// splits
		void SplitFirst(RKList<RKString>& out, const char* pSeparator = " ", int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitFirst(out, pSeparator, RKString_Length(pSeparator), maxSplitCount, keepEmptyParts, keepSeparator); }
		void SplitFirst(RKList<RKString>& out, const RKString& separator, int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitFirst(out, separator.GetString(), separator.Length(), maxSplitCount, keepEmptyParts, keepSeparator); }
		void SplitLast(RKList<RKString>& out, const char* pSeparator = " ", int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitLast(out, pSeparator, RKString_Length(pSeparator), maxSplitCount, keepEmptyParts, keepSeparator); }
		void SplitLast(RKList<RKString>& out, const RKString& separator, int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitLast(out, separator.GetString(), separator.Length(), maxSplitCount, keepEmptyParts, keepSeparator); }
		void Split(RKList<RKString>& out, const char* pSeparator = " ", bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitFirst(out, pSeparator, RKString_Length(pSeparator), -1, keepEmptyParts, keepSeparator); }
		void Split(RKList<RKString>& out, const RKString& separator, bool keepEmptyParts = false, bool keepSeparator = false) const { _SplitFirst(out, separator.GetString(), separator.Length(), -1, keepEmptyParts, keepSeparator); }
		RKList<RKString> SplitFirst(const char* pSeparator = " ", int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; SplitFirst(out, pSeparator, maxSplitCount, keepEmptyParts, keepSeparator); return out; }
		RKList<RKString> SplitFirst(const RKString& separator, int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; SplitFirst(out, separator, maxSplitCount, keepEmptyParts, keepSeparator); return out; }
		RKList<RKString> SplitLast(const char* pSeparator = " ", int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; SplitLast(out, pSeparator, maxSplitCount, keepEmptyParts, keepSeparator); return out; }
		RKList<RKString> SplitLast(const RKString& separator, int maxSplitCount = 1, bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; SplitLast(out, separator, maxSplitCount, keepEmptyParts, keepSeparator); return out; }
		RKList<RKString> Split(const char* pSeparator = " ", bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; Split(out, pSeparator, keepEmptyParts, keepSeparator); return out; }
		RKList<RKString> Split(const RKString& separator, bool keepEmptyParts = false, bool keepSeparator = false) const					   { RKList<RKString> out; Split(out, separator, keepEmptyParts, keepSeparator); return out; }

		// UTF8 casts/iteration
		typedef RKStringUTF_Iterator<uint32>  IteratorUTF32;
		typedef RKStringUTF_Iterator<uint16>  IteratorUCS;
		typedef RKStringUTF_Iterator<wchar_t> IteratorWCHAR;
		typedef RKStringUTF_Iterator<char>    IteratorTruncatedASCII;

		IteratorUTF32 BeginUTF32() const { return _Begin<uint32>(); }
		IteratorUTF32 EndUTF32() const   { return _End  <uint32>(); }
		IteratorUCS   BeginUCS() const   { return _Begin<uint16>(); }
		IteratorUCS   EndUCS() const     { return _End  <uint16>(); }
		IteratorWCHAR BeginWCHAR() const { return _Begin<wchar_t>(); }
		IteratorWCHAR EndWCHAR() const   { return _End  <wchar_t>(); }
		IteratorTruncatedASCII BeginASCII() const { return _Begin<char>(); }
		IteratorTruncatedASCII EndASCII() const   { return _End  <char>(); }

		uint32*  CreateDecodedBufferUTF32() const																					 { return _CreateDecodedBuffer<uint32>(); }
		uint16*  CreateDecodedBufferUCS()   const																					 { return _CreateDecodedBuffer<uint16>(); }
		wchar_t* CreateDecodedBufferWCHAR() const																					 { return _CreateDecodedBuffer<wchar_t>(); }
		char*    CreateDecodedBufferASCII() const																					 { return _CreateDecodedBuffer<char>(); }
		bool     FillDecodedBufferUTF32(uint32* buffer, uint32 bufferElementsCount) const { return _FillDecodedBuffer<uint32>(buffer, bufferElementsCount); }
		bool     FillDecodedBufferUCS(uint16* buffer, uint32 bufferElementsCount) const { return _FillDecodedBuffer<uint16>(buffer, bufferElementsCount); }
		bool     FillDecodedBufferWCHAR(wchar_t* buffer, uint32 bufferElementsCount) const { return _FillDecodedBuffer<wchar_t>(buffer, bufferElementsCount); }
		bool     FillDecodedBufferASCII(char*   buffer, uint32 bufferElementsCount) const { return _FillDecodedBuffer<char>(buffer, bufferElementsCount); }
		wchar_t*  ToUnicode() const;

		// allocation-related function
		// use them only if you know what you're doing
		void _Squeeze();
		void _Reserve(uint32 minimumBufferSize);
		uint32 _GetDynamicBufferSize() const                 { return _ssoSize == SSO_USE_HEAP_MARK ? _heapStorage.capacity : 0; }

		// operators, comparison
		bool operator ==(const char*    pString) const				 { return Compare(pString) == 0; }
		bool operator ==(const wchar_t* pString) const;
		bool operator ==(const RKString&    str) const				 { return str.Length() == Length() && operator==(str.GetString()); }
		bool operator ==(const std::string& str) const				 { return Length() == str.size() && operator==(str.c_str()); }
		bool operator ==(const std::wstring&str) const				 { return operator==(str.c_str()); }
		bool operator !=(const RKString&    str) const				 { return !operator==(str); }
		bool operator !=(const char*    pString) const				 { return !operator==(pString); }
		bool operator !=(const wchar_t* pString) const				 { return !operator==(pString); }
		bool operator !=(const std::string& str) const				 { return !operator==(str); }
		bool operator !=(const std::wstring&str) const				 { return !operator==(str); }
		bool operator  <(const RKString&    str) const				 { return Compare(str) < 0; } // needed for std::map

		// operators, assign/append
		RKString& operator  =(const char* pString)						 { Assign(pString); return *this; }
		RKString& operator  =(const wchar_t* pString)					 { Assign(pString); return *this; }
		RKString& operator  =(const RKString& str)						 { _Assign(str.GetString(), str.Length()); return *this; }
		RKString& operator  =(const std::string& str)          { _Assign(str.c_str(), (uint32)str.size()); return *this; }
		RKString& operator  =(const std::wstring& str)         { _AssignUTF(str.c_str(), (uint32)str.size()); return *this; }
		RKString& operator +=(const char* pString)						 { Append(pString); return *this; }
		RKString& operator +=(const wchar_t* pString)					 { Append(RKString(pString)); return *this; }
		RKString& operator +=(const RKString& str)						 { _Append(str.GetString(), str.Length()); return *this; }
		RKString& operator +=(const std::string& str)          { _Append(str.c_str(), (uint32)str.size()); return *this; }
		RKString& operator +=(const std::wstring& str)         { Append(RKString(str)); return *this; }

		// public static fields
		static const RKString EmptyString;
		static const char* const WhitespaceCharacters;

	private:
		// helpers
		template <typename T> static uint32 _StringCharacterCountToProcess(T* pString, int requestedStart, int requestedCount);
		template <typename T> static uint32 _ZeroTerminatedBufferLength(T* buffer);
		void _SanityCheck() const;
		bool _IsPartOfThisString(const char* ptr) const;

		// main modification functions
		uint32 _DetermineNewDynamicBufferSize(uint32 currentCapacity, uint32 requiredCapacity, bool strict);
		char* _AcquireWritableStringBuffer(uint32 stringLength, bool keepCurrentContent, bool strictBufferSize = false);
		void _Assign(const char* pString, uint32 count);
		void _Append(const char* pString, uint32 count);
		void _Insert(int position, const char* pString, uint32 count) { _ReplaceRegion(position, 0, pString, count); }

		// misc assigns
		void _AssignFormatted(const char* pFormat, va_list arglist);
		void _AssignLongValue(long long value, int base, bool useBasePrefix);

		// queries
		int _GetStripSymbolsCountLeft(const char* symbolsToStrip) const;
		int _GetStripSymbolsCountRight(const char* symbolsToStrip) const;

		bool _StartsWith(const char* pString, uint32 count) const;
		bool _EndsWith(const char* pString, uint32 count) const;

		// find/replace
		void _FindHelperNormalizeFromTo(int& from, int& to) const;
		int  _FindFirst(const char* pString, uint32 count, int from, int to) const;
		int  _FindLast(const char* pString, uint32 count, int from, int to) const;
		uint32 _Count(const char* pString, uint32 stringLength) const;

		void _ReplaceRegion(uint32 start, uint32 count, const char* pReplacement, uint32 replacementLength);
		bool _ReplaceFirst(const char* pSubstring, uint32 substringLength, const char* pReplacement, uint32 replacementLength, uint32 maxReplacementCount);
		bool _ReplaceLast(const char* pSubstring, uint32 substringLength, const char* pReplacement, uint32 replacementLength, uint32 maxReplacementCount);

		// split/join
		void _AssignJoined(const RKString* partsArray, uint32 partsCount, const char* pSeparator, uint32 separatorLength);
		void _AssignRepeated(const char* pBaseStr, uint32 baseLength, int count, const char* pSeparator, uint32 separatorLength);
		void _SplitFirst(RKList<RKString>& out, const char* pSeparator, uint32 separatorLength, uint32 maxSplitCount, bool keepEmptyParts, bool keepSeparator) const;
		void _SplitLast(RKList<RKString>& out, const char* pSeparator, uint32 separatorLength, uint32 maxSplitCount, bool keepEmptyParts, bool keepSeparator) const;

		// utf internals
		template <typename T> RKStringUTF_Iterator<T> _Begin() const { return RKStringUTF_Iterator<T>(GetString(), Length()); }
		template <typename T> RKStringUTF_Iterator<T> _End() const   { return RKStringUTF_Iterator<T>(GetString() + Length(), 0); }
		template <typename T> bool _FillDecodedBuffer(T* buffer, uint32 bufferElementsCount) const;
		template <typename T> T* _CreateDecodedBuffer() const;
		template <typename T> void _AssignUTF(T* buffer, uint32 bufferElementsCount);

		// implementation details
		static const int SSO_BUFFER_SIZE = 16;
		static const unsigned char SSO_USE_HEAP_MARK = -1;

		struct _HeapStorage
		{
			uint32 size;
			uint32 capacity;
			char*  stringPtr;
			char*  bufferPtr;
		};

		uint8 _ssoSize;

		union
		{
			char _ssoStorage[SSO_BUFFER_SIZE];
			_HeapStorage _heapStorage;
		};
	};

	RKString operator +(const RKString& pStringA, const RKString& pStringB);
	RKString operator +(const char* pStringA, const RKString& pStringB);
	RKString operator +(const RKString& pStringA, const char* pStringB);
	bool     operator ==(const char* pStringA, const RKString& pStringB);
	bool     operator !=(const char* pStringA, const RKString& pStringB);
	std::ostream&   operator << (std::ostream&  o, const RKString& str);
	std::wostream&  operator << (std::wostream&  o, const RKString& str);

	// this is a tricky function
	// if user asks to assign N characters from some string
	// to actually write min(N, length) characters
	// however we can't call strlen on this buffer, because
	// it can be not zero-terminated
	//
	// this function returns true count of characters to process
	template <typename T>
	uint32 RKString::_StringCharacterCountToProcess(T* pString, int requestedStart, int requestedCount)
	{
		if (requestedCount <= 0)
		{
			return 0;
		}

		int result = requestedCount;

		// determining if ptr length is smaller than requestedStart + requestedCount
		// can't call strlen here because ptr is not necessary zero-terminated
		for (int i = 0; i < (requestedStart + requestedCount); ++i)
		{
			if (pString[i] == 0)
			{
				result = i - requestedStart;
				break;
			}
		}

		if (requestedStart < 0)
		{
			result += requestedStart;
		}

		return std::max(0, result);
	}

	// template members implementation
	template <typename T>
	uint32 RKString::_ZeroTerminatedBufferLength(T* buffer)
	{
		uint32 length = 0;
		while (*buffer)
		{
			++buffer;
			++length;
		}
		return length;
	}

	template <typename T>
	bool RKString::_FillDecodedBuffer(T* buffer, uint32 bufferElementsCount) const
	{
		uint32 idx = 0;
		RKStringUTF_Iterator<T> it = _Begin<T>();
		RKStringUTF_Iterator<T> end = _End<T>();
		for (; it != end && idx < (bufferElementsCount - 1); ++it, ++idx)
		{
			buffer[idx] = *it;
		}
		buffer[idx] = 0;
		return it == end;
	}

	template <typename T>
	T* RKString::_CreateDecodedBuffer() const
	{
		uint32 outSize = LengthUTF() + 1;
		T* out = RKNEWARRAY(T, outSize);
		bool success = _FillDecodedBuffer<T>(out, outSize);
		RKASSERT(success, "Unable to decode UTF string, looks like an internal error");
		return out;
	}

	template <typename T>
	void RKString::_AssignUTF(T* buffer, uint32 bufferElementsCount)
	{
		const uint32 maxUtf8BufferLength = bufferElementsCount * 4;
		char* utf8Buffer = RKNEWARRAY(char, maxUtf8BufferLength);
		uint32 utf8BufferIndex = 0;

		for (uint32 i = 0; i < bufferElementsCount; ++i)
		{
			const uint32 utf32Code = buffer[i];
			uint32 writtenBytes = 0;
			RKStringUTF_RC rc = RKStringUTF_EncodeUTF8(utf32Code, utf8Buffer + utf8BufferIndex, maxUtf8BufferLength - utf8BufferIndex, writtenBytes);
			if (rc == RKStringUTF_SUCCESS)
			{
				utf8BufferIndex += writtenBytes;
			}
			else
			{
				utf8Buffer[utf8BufferIndex] = RKStringUTF_Iterator<uint8>::InvalidCodeSubstitution;
				++utf8BufferIndex;
			}
		}

		_Assign(utf8Buffer, utf8BufferIndex);
		RKDELETEARRAY(utf8Buffer);
	}
}
#endif // __RKSTRING_H_
