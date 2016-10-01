#pragma once

#include <iostream>
#include "DataReader.h"
#include "DataWriter.h"

#include "Constants.h"
namespace iwincore
{
	class Message
	{
	private:
		int				_commandId;
		DataReader*		_dis;
		DataWriter*		_dos;
		//ByteBuffer* _buffer;
		bool			_isEncrypted;


		unsigned char*			_msgContent;


		DataWriter*	getDos();
		DataReader*	getDis();
	public:
		Message(int commandId, unsigned char* content, int len, bool isEncrypted);
		Message(int commandId);
		Message(unsigned char* buffer, unsigned int len);
		Message(Message * clone);
		virtual ~Message();

		int		getCommandId();
		unsigned char*	getBufferBinary();
		unsigned int	getBufferSize();
		bool	isEncrypted();


		void	putInt(int value);
		void	putString(std::string value);
		void	putShort(s16 value);
		void	putByte(ubyte value);
		void	putBool(bool value);

		int		readInt();
		short		readShort();
		bool		readBool();
		s64		readLong();
		ubyte readByte();
		std::string		readString();
		unsigned char* readBuffer(int len);
		bool     isEndRead();

	};
}