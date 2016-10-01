#ifndef __DATA_READER_H__
#define __DATA_READER_H__

#include <string>
#include "Common/Common.h"
#include "NetworkMacros.h"
using namespace std;

class DataReader{
private:
	ubyte* data;
	int size;
	int index;
	bool clone_from_other;
public:
	DataReader(ubyte* data, int size);
	DataReader(DataReader * clone);
	~DataReader();
	void setBytes(ubyte* data, int size);
	int available();
	int readBytes(ubyte*& out, int len);
	ubyte readByte();
	bool readBool();
	s16 readShort();
	int readInt32();
	s64 readLong64();
	float readFloat();
	string readUTF();
    
    static void reverse(ubyte* arr, int len);
	
};

#endif