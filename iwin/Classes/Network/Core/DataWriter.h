#ifndef __DATA_WRITER_H__
#define __DATA_WRITER_H__

#include <string>
#include "NetworkMacros.h"

using namespace std;

class DataWriter
{
private:
	ubyte* data;
	int size;
	int capacity;
	void incCount(int num);
	void reverse(ubyte* arr, int len);
	bool clone_from_other;
public:
	DataWriter();
	DataWriter(DataWriter * clone);
	virtual ~DataWriter();
	int _getBytes(ubyte*& out);
	void writeBytes(ubyte* in, int len);
	void writeByte(ubyte b);
	void writeBool(bool b);
	void writeShort(s16 s);
	void writeInt32(int i);
	void writeLong64(s64 l);
	void writeFloat(float f);
	int writeUTF(char* str);
	int writeUTF(string st);
	int getSize();
};

#endif // __DATA_WRITER_H__