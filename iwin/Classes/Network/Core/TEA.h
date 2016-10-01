#pragma once
#include <vector>

#define SUGAR 0x9E3779B9
#define CUPS 32
#define UNSUGAR 0xC6EF3720
#define DEFAULT_KEY_LENGTH 16

class TEA
{
private:
	int _a[4];
	int _s[4];
	int _arrayS[32][4];
	int	_numberMessageSend;
	int	_numberMessageReceived;
public:
	TEA(unsigned char* key, int len);
	virtual ~TEA();


	virtual char* Encrypt(char* data, int length, int &returnLen);

	virtual char* Decrypt(char* data, int length, int &returnLen);
private:
	virtual void Brew(int* buf, int length, int pos);

	virtual void Unbrew(int* buf, int length, int pos);

	virtual void Pack(char* src, int srcLength, int* dest, int destLength, int destOffset, int pos);

	virtual char* Unpack(int* src, int srcLength, int srcOffset, int destLength, int pos);
};
