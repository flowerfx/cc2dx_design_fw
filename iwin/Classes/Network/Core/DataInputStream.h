#pragma once

#include "Common/Common.h"
class DataInputStream 
{
private:
	unsigned char*			_buffer;
	unsigned int			_size;
	int						_cursor;

				template <typename T>
				void _read(T& value);

public:
	DataInputStream( unsigned char* buffer, int size );
	virtual		~DataInputStream();

	int				readInt();
	bool			readBool();
	short			readShort();
	unsigned char*	readBuffer(int len, int& returnLen);
	int				getSize();
};