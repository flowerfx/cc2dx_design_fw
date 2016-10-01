#pragma once
#include "Common/Common.h"
class DataOutputStream 
{
private:
	unsigned char*			_buffer;
			int				_size;
			int				_cursor;
			bool			_bigEndian;


			void	_resizeBuffer(int newSize);
			template <typename T>
			unsigned int _write(T buffer, unsigned int length);
			template <typename T>
			unsigned int _write(T value);
public:
	DataOutputStream();
	virtual ~DataOutputStream();
		
			void	writeInt(int value);
			void	writeShort(s16 value);
			void	writeByte(ubyte value);
			void	writeBuffer(unsigned char* buffer, int len);
			unsigned char*	peek();
			int				tell();
};