#include "TEA.h"
#include <iostream>
#include <cstring>
#include "Common/Common.h"

TEA::TEA(unsigned char* keyFull, int len)
{
	for (int i = 0; i < 4; i++)
	{
		_s[i] = 0;
	}
	_a[0] = 0;
	unsigned char* key = new unsigned char[16];
	for (int i = 0; i < CUPS; i++) {
		int end = i + DEFAULT_KEY_LENGTH;
		if (end >= len) {
			int numCopy = (CUPS - end + DEFAULT_KEY_LENGTH);
			std::memcpy(key, keyFull + i, numCopy);
			std::memcpy(key + numCopy, keyFull, DEFAULT_KEY_LENGTH - numCopy);
		}
		else {
			std::memcpy(key, keyFull + i, DEFAULT_KEY_LENGTH);
		}
		for (int off = 0, j = 0; j < 4; j++) {
			_arrayS[i][j] = ((key[off++] & 0xff))
				| ((key[off++] & 0xff) << 8)
				| ((key[off++] & 0xff) << 16)
				| ((key[off++] & 0xff) << 24);
		}
	}
	delete key;
	_numberMessageReceived = 0;
	_numberMessageSend = 0;
}



TEA::~TEA()
{

}


char* TEA::Encrypt(char* data, int length, int& returnLen){
	int pos = _numberMessageSend++ % CUPS;
	int paddedSize = ((length >> 3) + (((length % 8) == 0) ? 0 : 1)) << 1;
	int* buffer = new int[paddedSize + 1];
	buffer[0] = length;
	this->Pack(data, length, buffer, paddedSize + 1, 1, pos);
	this->Brew(buffer, paddedSize + 1, pos);
	returnLen = (paddedSize + 1) << 2;
	return this->Unpack(buffer, paddedSize + 1, 0, (paddedSize + 1) << 2, pos);
}

char* TEA::Decrypt(char* data, int length, int& returnLen){
	int pos = _numberMessageReceived++ % 4;
	if (length % 4 == 0)
	{
		if ((length >> 2) % 2 == 1)
		{
			int* buffer = new int[length >> 2];
			this->Pack(data, length, buffer, length >> 2, 0, pos);
			this->Unbrew(buffer, length >> 2, pos);
			returnLen = buffer[0];
			return this->Unpack(buffer, length >> 2, 1, buffer[0], pos);
		}
	}
	return nullptr;
}

void TEA::Brew(int* buf, int length, int pos){
	if (length % 2 == 1)
	{
		int i, v0, v1, sum, n;
		i = 1;
		while (i < length)
		{
			n = CUPS;
			v0 = buf[i];
			v1 = buf[i + 1];
			sum = 0;
			while (n-- > 0)
			{
				sum += SUGAR;
				v0 += ((v1 << 4) + _s[0] ^ v1) + (sum ^ ((int)((unsigned int)v1 >> 5))) + _s[1];
				v1 += ((v0 << 4) + _s[2] ^ v0) + (sum ^ ((int)((unsigned int)v0 >> 5))) + _s[3];
			}
			buf[i] = v0;
			buf[i + 1] = v1;
			i += 2;
		}
	}
}

void TEA::Unbrew(int* buf, int length, int pos){
	if (length % 2 == 1)
	{
		int i, v0, v1, sum, n;
		i = 1;
		sum = 0;
		while (i <length)
		{
			n = CUPS;
			v0 = buf[i];
			v1 = buf[i + 1];
			sum = UNSUGAR;
			while (n-- > 0)
			{
				v1 -= ((v0 << 4) + _s[2] ^ v0) + (sum ^ ((int)((unsigned int)v0 >> 5))) + _s[3];
				v0 -= ((v1 << 4) + _s[0] ^ v1) + (sum ^ ((int)((unsigned int)v1 >> 5))) + _s[1];
				sum -= SUGAR;
			}
			buf[i] = v0;
			buf[i + 1] = v1;
			i += 2;
		}
	}
}

void TEA::Pack(char* src, int srcLength, int* dest, int destLength, int destOffset, int pos){
	if (destOffset + (srcLength >> 2) <= destLength)
	{
		int i = 0, shift = 24;
		int j = destOffset;
		dest[j] = 0;
		while (i < srcLength)
		{
			if (j >= destLength)
			{
				int a = 0;
				a = 1;
			}
			dest[j] |= ((src[i] & 0xff) << shift);
			if (shift == 0)
			{
				shift = 24;
				j++;
				if (j < destLength)
				{
					dest[j] = 0;
				}
			}
			else
			{
				shift -= 8;
			}
			i++;
		}
	}
}

char* TEA::Unpack(int* src, int srcLength, int srcOffset, int destLength, int pos){
	if (destLength <= (srcLength - srcOffset) << 2)
	{
		char* dest = new char[destLength];
		int i = srcOffset;
		int count = 0;
		for (int j = 0; j < destLength; j++)
		{
			ubyte tmp = (unsigned char)((src[i] >> (24 - (count << 3))) & 0xff);
			dest[j] = tmp;
			count++;
			if (count == 4)
			{
				count = 0;
				i++;
			}
		}
		return dest;
	}
	return nullptr;
}