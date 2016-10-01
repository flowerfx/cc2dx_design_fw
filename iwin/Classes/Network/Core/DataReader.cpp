#include "DataReader.h"
#include "RKBaseLog/debug.h"
using namespace RKUtils;
DataReader::DataReader(ubyte* data, int size)
{
	setBytes(data, size);
}

DataReader::DataReader(DataReader * clone)
{
	int lengh = sizeof(clone->data) / sizeof(ubyte);
	data = new ubyte[clone->size];
	memcpy(data, clone->data, clone->size);
	this->size = clone->size;
	this->index = clone->index;
	clone_from_other = true;
}

DataReader::~DataReader(){
	if (clone_from_other)
	{
		delete data;
	}
	data = nullptr;
}

void DataReader::setBytes(ubyte* data, int size)
{
	this->data = data;
	this->size = size;
	this->index = 0;
	clone_from_other = false;
}

int DataReader::available()
{
	return size - index;
}

int DataReader::readBytes(ubyte*& out, int len)
{
	if(index >= size){
		return 0;
	}
	if(index + len >= size){
		len = size - index;
	}
	if(out==nullptr){
		out = new ubyte[len];
	}
	ubyte* tmp = data + index;
	memcpy(out, tmp, len);
	index += len;
	tmp = nullptr;
	return len;
}

void DataReader::reverse(ubyte* arr, int len)
{
	int n = len>>1;
	for(int i=0; i<n; ++i){
		arr[i] ^= arr[len-i-1];
		arr[len-i-1] ^= arr[i];
		arr[i] ^= arr[len-i-1];
	}
}

ubyte DataReader::readByte()
{
	ubyte b;
	ubyte*buf = nullptr;
	readBytes(buf, 1);
	if (buf == nullptr)
	{
		PASSERT2(buf != nullptr, "read failed!");
		return 0;
	}
	memcpy(&b, buf, 1);
	SAFE_DELETE_ARRAY(buf);
	return b;
}

bool DataReader::readBool()
{
	ubyte b = readByte();
	return (b!=0);
}

s16 DataReader::readShort()
{
	s16 s;
	ubyte* buf = nullptr;
	readBytes(buf, 2);
	if (buf == nullptr)
	{
		PASSERT2(buf != nullptr, "read failed!");
		return 0;
	}
	reverse(buf, 2);
	memcpy(&s, buf, 2);
	SAFE_DELETE_ARRAY(buf);
	/*if (buf)
	{
		delete buf;
		buf = nullptr;
	}*/
	return s;
}

int DataReader::readInt32()
{
	int i;
	ubyte* buf = nullptr;
	readBytes(buf, 4);
	if (buf == nullptr)
	{
		PASSERT2(buf != nullptr, "read failed!");
		return 0;
	}
	reverse(buf, 4);
	memcpy(&i, buf, 4);
	SAFE_DELETE_ARRAY(buf);
	return i;
}

s64 DataReader::readLong64()
{
	s64 l;
	ubyte* buf = nullptr;
	readBytes(buf, 8);
	if (buf == nullptr)
	{
		PASSERT2(buf != nullptr, "read failed!");
		return 0;
	}
	reverse(buf, 8);
	memcpy(&l, buf, 8);
	SAFE_DELETE_ARRAY(buf);
	return l;
}

float DataReader::readFloat()
{
	float f;
	ubyte* buf = nullptr;
	readBytes(buf, 4);
	if (buf == nullptr)
	{
		PASSERT2(buf != nullptr, "read failed!");
		return 0;
	}
	reverse(buf, 4);
	memcpy(&f, buf, 4);
	SAFE_DELETE_ARRAY(buf);
	return f;
}

string DataReader::readUTF()
{
	int size = readShort();
	if (size >= 0)
	{
		ubyte* buff = new ubyte[size];
		readBytes(buff, size);
		string st((char*)buff, size);
		SAFE_DELETE_ARRAY(buff);
		return st;
	}
	else
	{
		PASSERT2(false, "wrong size");
		return "";
	}
}
