#ifndef __RKSTRING_UTF_H_
#define __RKSTRING_UTF_H_

#include "../Common/RKTypes.h"
#include "../Common/RKMacros.h"
#include <iterator>
namespace RKUtils
{
#undef max
#undef min
	enum RKStringUTF_RC
	{
		RKStringUTF_SUCCESS = 0,
		RKStringUTF_OVERLONG_CODE, // only for decoding, value parsed
		RKStringUTF_ZERO_CODE,     // value parsed/written, but it's zero
		RKStringUTF_INVALID_CODE,  // impossible to parse code or code out of utf8 range
		RKStringUTF_BUFFER_OVERRUN,// reached end of buffer
	};

	// returns source size of next utf code [1 .. 4]
	int RKStringUTF_NextCodeLength(const char* buffer, uint32 bufferLength);
	// returns count of bytes utf8-encoded, can return 0 if it is an invalid utf code and can't be encoded into utf8
	int RKStringUTF_EncodedLength(uint32 utf32Code);

	RKStringUTF_RC RKStringUTF_DecodeUTF8(const char* buffer, uint32 bufferLength, uint32& outputUtf32, uint32& outputOffset);
	RKStringUTF_RC RKStringUTF_EncodeUTF8(uint32 utf32Code, char* outBuffer, uint32 outBufferLength, uint32& outputOffset);

	template <int size> class RKStringUTF_UnsignedTypeForSizeHelper;
	template <> class RKStringUTF_UnsignedTypeForSizeHelper < 1 > { public: typedef uint8  UnsignedType; };
	template <> class RKStringUTF_UnsignedTypeForSizeHelper < 2 > { public: typedef uint16 UnsignedType; };
	template <> class RKStringUTF_UnsignedTypeForSizeHelper < 4 > { public: typedef uint32 UnsignedType; };

	template <typename OUTPUT>
	class RKStringUTF_Iterator : public std::iterator < std::forward_iterator_tag, OUTPUT >
	{
	public:
		RKStringUTF_Iterator()
			: _dataPtr(0)
			, _dataSize(0)
			, _currentValueUTF8Length(0)
		{
		}

		RKStringUTF_Iterator(const char* utf8DataPtr, uint32 utf8DataSize)
			: _dataPtr(utf8DataPtr)
			, _dataSize(utf8DataSize)
			, _currentValueUTF8Length(0)
		{
			RKASSERT(_dataPtr, "Invalid argument, NULL is not expected");
			RKSTATICASSERT(sizeof(UNSIGNED_OUTPUT) == sizeof(OUTPUT), "Incorrect unsigned output type provided");
		}

		RKStringUTF_Iterator operator++ ()
		{
			_PrepareCurrentValue();
			_dataPtr += _currentValueUTF8Length;
			_dataSize -= _currentValueUTF8Length;

			// reset current value
			_currentValueUTF8Length = 0;
			return *this;
		}

		OUTPUT operator*()
		{
			_PrepareCurrentValue();
			if (IsCurrentCodeTruncated())
			{
				return TruncatedCodeSubstitution;
			}
			else
			{
				UNSIGNED_OUTPUT out = (UNSIGNED_OUTPUT)_currentValueDecodedUTF32;
				return ((OUTPUT&)out);
			}
		}

		uint32 CurrentCodeLength()
		{
			_PrepareCurrentValue();
			return _currentValueUTF8Length;
		}

		bool IsCurrentCodeValid()
		{
			_PrepareCurrentValue();
			return _currentValueParserRC == RKStringUTF_SUCCESS;
		}

		bool IsCurrentCodeTruncated()
		{
			_PrepareCurrentValue();
			const UNSIGNED_OUTPUT out = (UNSIGNED_OUTPUT)_currentValueDecodedUTF32;
			return out != _currentValueDecodedUTF32;
		}

		RKStringUTF_Iterator operator++ (int) { RKStringUTF_Iterator temp(*this); operator++(); return temp; }
		bool operator == (const RKStringUTF_Iterator& other) const { return !this->operator!=(other); }
		bool operator != (const RKStringUTF_Iterator& other) const { return _dataPtr != other._dataPtr || _dataSize != other._dataSize; }

		static const OUTPUT InvalidCodeSubstitution = '?';
		static const OUTPUT TruncatedCodeSubstitution = '#';

	private:
		typedef typename RKStringUTF_UnsignedTypeForSizeHelper<sizeof(OUTPUT)>::UnsignedType UNSIGNED_OUTPUT;

		void _PrepareCurrentValue()
		{
			if (_currentValueUTF8Length) // exit if already prepared
			{
				return;
			}

			RKASSERT(_dataSize != 0, "Using invalid iterator");
			_currentValueParserRC = RKStringUTF_DecodeUTF8(_dataPtr, _dataSize, _currentValueDecodedUTF32, _currentValueUTF8Length);
			if (!IsCurrentCodeValid())
			{
				_currentValueDecodedUTF32 = InvalidCodeSubstitution;
			}
			RKASSERT(_currentValueUTF8Length <= _dataSize, "Internal decoding error");
		}

		const char* _dataPtr;
		uint32 _dataSize;
		uint32 _currentValueDecodedUTF32;
		uint32 _currentValueUTF8Length;
		RKStringUTF_RC _currentValueParserRC;
	};
}
#endif // __RKSTRING_UTF_H_