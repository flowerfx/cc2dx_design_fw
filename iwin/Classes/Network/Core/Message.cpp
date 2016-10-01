#include "Message.h"
#include "DataInputStream.h"
#include "DataOutputStream.h"
#include "cocos2d.h"
namespace iwincore {
	Message::Message(int commandId, unsigned char* content, int len, bool isEncrypted)
	{
		_dis = nullptr;
		_dos = nullptr;
		_commandId = commandId;
		putShort(commandId);
		_isEncrypted = isEncrypted;
	}

	Message::Message(int commandId)
	{
		_dis = nullptr;
		_dos = nullptr;
		_commandId = commandId;
		putShort(commandId);
		_isEncrypted = true;
	}

	Message::Message(Message * clone)
	{
		_dis = nullptr;
		_dos = nullptr;
		if (clone->_dos)
		{
			_dos = new DataWriter(clone->_dos);
		}
		if (clone->_dis)
		{
			_dis = new DataReader(clone->_dis);
		}
		_commandId = clone->_commandId;
		//putShort(_commandId);
		_isEncrypted = clone->_isEncrypted;
	}

	Message::Message(unsigned char* buffer, unsigned int len)
	{
		_dos = nullptr;
		_dis = new DataReader(buffer, len);
		_commandId = _dis->readShort();
		_isEncrypted = true;
	}

	Message::~Message()
	{
		if (_dos) delete _dos;
		if (_dis) delete _dis;
	}

	int Message::getCommandId()
	{
		return _commandId;
	}

	bool Message::isEncrypted()
	{
		return _isEncrypted;
	}

	DataWriter* Message::getDos()
	{
		if (!_dos) _dos = new DataWriter();
		return _dos;
	}

	DataReader* Message::getDis()
	{
		return _dis;
	}

	void Message::putInt(int value)
	{
		getDos()->writeInt32(value);
	}

	void Message::putString(std::string value)
	{
		getDos()->writeUTF(value);
	}

	void Message::putShort(s16 value)
	{
		getDos()->writeShort(value);
	}

	void Message::putByte(ubyte value)
	{
		getDos()->writeByte(value);
	}

	void Message::putBool(bool value)
	{
		getDos()->writeByte((unsigned char)(value?1:0));
	}
	
	int Message::readInt()
	{
		return getDis()->readInt32();
			
	}

	s16 Message::readShort()
	{
		return getDis()->readShort();
	}

	s64 Message::readLong()
	{
		return getDis()->readLong64();
	}
	bool Message::readBool()
	{
		return (getDis()->readByte() != 0);
	}

	ubyte Message::readByte()
	{
		return getDis()->readByte();
	}

	unsigned char* Message::readBuffer(int len)
	{
		ubyte* bytes= new ubyte[len];
		getDis()->readBytes(bytes, len);
		return bytes;
	}

	unsigned char* Message::getBufferBinary()
	{
		ubyte* bytes = nullptr;
		getDos()->_getBytes(bytes);
		return bytes;
	}

	unsigned int Message::getBufferSize()
	{
		return getDos()->getSize();
	}

	string Message::readString()
	{
		return getDis()->readUTF();
	}

	bool Message::isEndRead()
	{
		return getDis()->available() == 0;
	}
}