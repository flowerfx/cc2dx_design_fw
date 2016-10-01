#include "DataOutputStream.h"
#include <iostream>

#define PREBUFFER_SIZE 1024

DataOutputStream::DataOutputStream()
{
	_buffer = new unsigned char[PREBUFFER_SIZE];
	_size = PREBUFFER_SIZE;
	_cursor = 0;
	_bigEndian = true;
}

DataOutputStream::~DataOutputStream()
{
	if(_buffer) delete(_buffer);
}


void DataOutputStream::writeInt(int value)
{
	_write(value);
}

void DataOutputStream::writeShort(s16 value)
{
	_write(value);
}

void DataOutputStream::writeByte(ubyte value)
{
	_write(value);
}

void DataOutputStream::writeBuffer(unsigned char* buffer, int len)
{
	_write(buffer, len);
}

void DataOutputStream::_resizeBuffer(int newSize)
{
	unsigned char* newBuffer = new unsigned char[newSize];
	memcpy(newBuffer, _buffer, _size);
	delete _buffer;
	_buffer = newBuffer;
	_size = newSize;
}

template <typename T>
unsigned int DataOutputStream::_write(T buffer,
	unsigned int length) {
	size_t pos = static_cast<size_t>(this->_cursor);
	size_t size = static_cast<size_t>(length);

	if (pos + size > _size) {
		_resizeBuffer(pos + size);
	}

	memcpy(this->_buffer+pos, static_cast<T>(buffer), size);
	this->_cursor += size;
	return size;
}

template <typename T>
unsigned int DataOutputStream::_write(T value) {
	T finalValue = value;
	uint8_t *originalData = reinterpret_cast<uint8_t*>(&value);
	uint8_t *finalData = reinterpret_cast<uint8_t*>(&finalValue);

	if (_bigEndian && sizeof(T) > 1) {
		for (int i = 0; i < sizeof(T); ++i) {
			finalData[i] = originalData[(sizeof(T) - i) - 1];
		}
		originalData = finalData;
	}

	return this->_write((unsigned char*)&value, sizeof(T));
}

unsigned char* DataOutputStream::peek()
{
	return _buffer;
}

int DataOutputStream::tell()
{
	return _cursor;
}