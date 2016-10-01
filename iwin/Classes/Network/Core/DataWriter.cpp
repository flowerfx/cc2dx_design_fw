#include "DataWriter.h"
#define MAX_SIZE 8*1024
#define DEF_SIZE 2*1024
#define INC_SIZE 1024

DataWriter::DataWriter()
{
	size = 0;
	capacity = DEF_SIZE;
	data = new ubyte[capacity];
	clone_from_other = true;
}

DataWriter::DataWriter(DataWriter * clone)
{
	size = clone->size;
	capacity = clone->capacity;
	data = new ubyte[capacity];
	memcpy(data, clone->data, capacity);
	clone_from_other = true;
}

DataWriter::~DataWriter()
{
	SAFE_DELETE_ARRAY(data);
}

void DataWriter::incCount(int num)
{
	if(size + num >= capacity){
		capacity = capacity + INC_SIZE;
		ubyte* newData = new ubyte[capacity];
		memcpy(newData, data, size);
		ubyte*tmp = data;
		data = newData;
		SAFE_DELETE_ARRAY(tmp);
	}
	size += num;
}

void DataWriter::reverse(ubyte* arr, int len)
{
	int n = len>>1;
	for(int i=0; i<n; ++i){
		arr[i] ^= arr[len-i-1];
		arr[len-i-1] ^= arr[i];
		arr[i] ^= arr[len-i-1];
	}
}

int DataWriter::_getBytes(ubyte*& out)
{
	if(out == NULL){
		out = new ubyte[size];
	}
	memcpy(out, data, size);
	return size;
}

void DataWriter::writeBytes(ubyte* in, int len)
{
	int index = size;
	incCount(len);
	ubyte* tmp = data + index;
	memcpy(tmp, in, len);
	tmp = NULL;
}

void DataWriter::writeByte(ubyte b)
{
	ubyte* buf = new ubyte[1];
	buf[0] = b;
	writeBytes(buf, 1);
	SAFE_DELETE_ARRAY(buf);
}

void DataWriter::writeBool(bool b)
{
	ubyte* buf = new ubyte[1];
	if(b) {
		buf[0] = 1;
	} else {
		buf[0] = 0;
	}
	writeBytes(buf, 1);
	SAFE_DELETE_ARRAY(buf);
}

void DataWriter::writeShort(s16 s)
{
	reverse((ubyte*)&s, 2);
	writeBytes((ubyte*)&s, 2);
}

void DataWriter::writeInt32(int i)
{
	reverse((ubyte*)&i, 4);
	writeBytes((ubyte*)&i, 4);
}

void DataWriter::writeLong64(s64 l)
{
	reverse((ubyte*)&l, 8);
	writeBytes((ubyte*)&l, 8);
}

void DataWriter::writeFloat(float f)
{
	ubyte*buf = new ubyte[4];
	memcpy(buf, &f, 4);
	reverse(buf, 4);
	writeBytes(buf, 4);
	SAFE_DELETE_ARRAY(buf);
}

int DataWriter::writeUTF(char* str)
{
	int utflen = (int)strlen(str);
	int size = utflen + 2;
	char* out = new char[size];
	out[0] = (ubyte) ((utflen >> 8) & 0xFF);
	out[1] = (ubyte) ((utflen >> 0) & 0xFF);
	char* tmp = out + 2;
	memcpy(tmp, str, utflen);
	writeBytes((ubyte*)out, size);
	tmp = NULL;
	SAFE_DELETE_ARRAY(out);
	return size;
}

int DataWriter::writeUTF(string st)
{
	int utflen = (int)st.size();
	int size = utflen + 2;
	char* out = new char[size];
	out[0] = (ubyte) ((utflen >> 8) & 0xFF);
	out[1] = (ubyte) ((utflen >> 0) & 0xFF);
	char* tmp = out + 2;
	std::copy(st.begin(), st.end(), tmp);
	writeBytes((ubyte*)out, size);
	tmp = NULL;
	SAFE_DELETE_ARRAY(out);
	return size;
}
int DataWriter::getSize()
{
	return size;
}

