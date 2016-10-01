#include "DataInputStream.h"
#include <iostream>


DataInputStream::DataInputStream( unsigned char* buffer, int size )
{
	_buffer = buffer;
	_size = size;
	_cursor = 0;
}

DataInputStream::~DataInputStream()
{

}

int DataInputStream::getSize()
{
	return _size;
}

int DataInputStream::readInt()
{
	int value;
	_read(value);
	return value;
}

bool DataInputStream::readBool()
{
	bool value;
	_read(value);
	return value;
}

s16 DataInputStream::readShort()
{
	s16 value;
	_read(value);
	return value;
}

unsigned char* DataInputStream::readBuffer(int len, int& returnLen)
{
	if (len == -1) len = _size - _cursor;

	if (_cursor + len > _size)
	{
		return nullptr;
	}

	unsigned char* value = new unsigned char[len];
	memcpy(value, _buffer + _cursor, len);
	_cursor += len;
	returnLen = len;
	return value;
}

template <typename T>
void DataInputStream::_read(T& value)
{
	if (_cursor + sizeof(T) > _size)
	{
		return;
	}
	char* finalData;
	char* originalData;
	T finalValue;

	finalValue = *(reinterpret_cast<T*>(_buffer + _cursor));
	originalData = reinterpret_cast<char*>(&finalValue);
	finalData = reinterpret_cast<char*>(&value);
	for (int i = 0; i < sizeof(T); ++i) {
		finalData[i] = originalData[(sizeof(T) - i) - 1];
	}
	_cursor += sizeof(T);
}

