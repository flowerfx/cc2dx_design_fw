#include "RKStringUTF.h"
// #ifdef WINAPI_FAMILY
#include <algorithm>
// #endif
#include "../Common/RKAssert.h"
namespace RKUtils
{
#define TOP_BIT_MASK 0x80 // eq 1000000
#define ADDITIONAL_BYTE_MASK 0x3f // eq 00111111
#define IS_ADDITIONAL_BYTE(b) ((b & (~ADDITIONAL_BYTE_MASK)) == TOP_BIT_MASK) // match 10??????

	static inline int _RKStringUTF_ParseHeaderByte(uint8 topByte)
	{
		if ((topByte & TOP_BIT_MASK) == 0)
		{
			return 1;
		}

		if ((topByte >> 5) == 0x06) // match 110?????
		{
			return 2;
		}

		if ((topByte >> 4) == 0x0e) // match 1110????
		{
			return 3;
		}

		if ((topByte >> 3) == 0x1e) // match 11110???
		{
			return 4;
		}

		/* 5 and 6 bytes codes had been excluded from utf8 standard
		if ((topByte >> 2) == 0x3e) // match 111110??
		{
		return 5;
		}

		if ((topByte >> 1) == 0x7e) // match 1111110?
		{
		return 6;
		}
		*/

		// invalid code header, return -1
		return -1;
	}

	static inline int _RKStringUTF_GetUTF8RepresentationBytesCount(uint32 utf32Code)
	{
		if (utf32Code <= 0x7f) return 1;
		if (utf32Code <= 0x07ff) return 2;
		if (utf32Code <= 0xffff) return 3;
		if (utf32Code <= 0x1fffff) return 4;
		return 0; // invalid code, can't be represeted by utf8
	}

	static inline bool _RKStringUTF_IsBannedUTF32Code(uint32 utf32Code)
	{
		return ((utf32Code >= 0xD800) && (utf32Code <= 0xDFFF)) ||
			(utf32Code == 0xFFFE) || (utf32Code == 0xFFFF);
	}

	int RKStringUTF_EncodedLength(uint32 utf32Code)
	{
		if (_RKStringUTF_IsBannedUTF32Code(utf32Code))
		{
			return 0;
		}
		else
		{
			return _RKStringUTF_GetUTF8RepresentationBytesCount(utf32Code);
		}
	}

	int RKStringUTF_NextCodeLength(const char* bufferChar, uint32 bufferLength)
	{
		RKASSERT(bufferChar, "Invalid argument, NULL received");
		const uint8* buffer = (const uint8*)bufferChar;

		int codeLength = _RKStringUTF_ParseHeaderByte(*buffer);
		if (codeLength >= 0)
		{
			codeLength = std::min(uint32(codeLength), bufferLength);
			for (int i = 1; i < codeLength; ++i)
			{
				if (!IS_ADDITIONAL_BYTE(buffer[i]))
				{
					return i;
				}
			}

			return codeLength;
		}
		else
		{
			return 1;
		}
	}

	RKStringUTF_RC RKStringUTF_DecodeUTF8(const char* bufferChar, uint32 bufferLength, uint32& outputUtf32, uint32& outputOffset)
	{
		RKASSERT(bufferChar || (bufferLength == 0), "Invalid argument, NULL received");
		const uint8* buffer = (const uint8*)bufferChar;

		if (bufferLength == 0)
		{
			outputOffset = 0;
			return RKStringUTF_BUFFER_OVERRUN;
		}

		int bytesCount = _RKStringUTF_ParseHeaderByte(*buffer);
		if (bytesCount > 0)
		{
			outputOffset = bytesCount;

			// special case for ascii
			if (bytesCount == 1)
			{
				outputUtf32 = *bufferChar;
				return outputUtf32 != 0 ? RKStringUTF_SUCCESS : RKStringUTF_ZERO_CODE;
			}

			// handle buffer overrun and zero-terminator
			if (uint32(bytesCount) > bufferLength)
			{
				outputOffset = bufferLength;
				return RKStringUTF_BUFFER_OVERRUN;
			}
			for (int i = 1; i < bytesCount; ++i)
			{
				if (!IS_ADDITIONAL_BYTE(buffer[i]))
				{
					outputOffset = i;
					return RKStringUTF_INVALID_CODE;
				}
			}

			switch (bytesCount)
			{
			case 2:
			{
				uint8 byte1 = buffer[0] & 0x1f; // mask 00011111
				uint8 byte2 = buffer[1] & ADDITIONAL_BYTE_MASK;
				outputUtf32 = (uint32(byte1) << 6) | byte2;
			}
			break;
			case 3:
			{
				uint8 byte1 = buffer[0] & 0x0f; // mask 00001111
				uint8 byte2 = buffer[1] & ADDITIONAL_BYTE_MASK;
				uint8 byte3 = buffer[2] & ADDITIONAL_BYTE_MASK;
				outputUtf32 = (uint32(byte1) << 12) | (uint32(byte2) << 6) | byte3;
			}
			break;
			case 4:
			{
				uint8 byte1 = buffer[0] & 0x07; // mask 00000111
				uint8 byte2 = buffer[1] & ADDITIONAL_BYTE_MASK;
				uint8 byte3 = buffer[2] & ADDITIONAL_BYTE_MASK;
				uint8 byte4 = buffer[3] & ADDITIONAL_BYTE_MASK;
				outputUtf32 = (uint32(byte1) << 18) | (uint32(byte2) << 12) | (uint32(byte3) << 6) | byte4;
			}
			break;
			default:
				RKASSERT(false, "Internal UTF8 decoding error, unexpected header value");
				return RKStringUTF_INVALID_CODE;
			}

			if (outputUtf32 != 0)
			{
				if (!_RKStringUTF_IsBannedUTF32Code(outputUtf32))
				{
					bool overlong = _RKStringUTF_GetUTF8RepresentationBytesCount(outputUtf32) != outputOffset;
					return overlong ? RKStringUTF_OVERLONG_CODE : RKStringUTF_SUCCESS;
				}
				else
				{
					return RKStringUTF_INVALID_CODE;
				}
			}
			else
			{
				// zeros in stream are not supported
				return RKStringUTF_ZERO_CODE;
			}
		}
		else
		{
			outputOffset = 1;
			return RKStringUTF_INVALID_CODE;
		}

		RKASSERT(false, "Unreachable code");
		return RKStringUTF_INVALID_CODE;
	}

	RKStringUTF_RC RKStringUTF_EncodeUTF8(uint32 utf32Code, char* outBufferChar, uint32 outBufferLength, uint32& outputOffset)
	{
		int bytes = _RKStringUTF_GetUTF8RepresentationBytesCount(utf32Code);
		if (bytes >= 0 && uint32(bytes) > outBufferLength)
		{
			outputOffset = 0;
			return RKStringUTF_BUFFER_OVERRUN;
		}

		uint8* outBuffer = (uint8*)outBufferChar;
		outputOffset = bytes;

		switch (bytes) {
		case 0:
			return RKStringUTF_INVALID_CODE;
		case 1:
			outBuffer[0] = utf32Code;
			return utf32Code != 0 ? RKStringUTF_SUCCESS : RKStringUTF_ZERO_CODE;
		case 2:
			outBuffer[0] = (utf32Code >> 6) | 0xC0; // prepend with 110xxxxx
			outBuffer[1] = (utf32Code & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			return RKStringUTF_SUCCESS;
		case 3:
			if (_RKStringUTF_IsBannedUTF32Code(utf32Code)) // banned codes exist only as triplets
			{
				outputOffset = 0;
				return RKStringUTF_INVALID_CODE;
			}
			outBuffer[0] = (utf32Code >> 12) | 0xE0; // prepend with 1110xxxx
			outBuffer[1] = ((utf32Code >> 6) & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			outBuffer[2] = (utf32Code        & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			return RKStringUTF_SUCCESS;
		case 4:
			outBuffer[0] = (utf32Code >> 18) | 0xF0; // prepend with 11110xxx
			outBuffer[1] = ((utf32Code >> 12) & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			outBuffer[2] = ((utf32Code >> 6) & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			outBuffer[3] = (utf32Code         & ADDITIONAL_BYTE_MASK) | TOP_BIT_MASK; // make 10xxxxxx
			return RKStringUTF_SUCCESS;
		default:
			RKASSERT(false, "Internal error, should never happen");
			outputOffset = 0;
			return RKStringUTF_INVALID_CODE;
		}
	}
}