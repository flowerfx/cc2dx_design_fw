#include "MobileClient.h"
#include "cocos2d.h"
#include "DataOutputStream.h"
#include "DataInputStream.h"
#include "JsonMessage.h"
#include "TEA.h"



USING_NS_CC;
#define PROTOCOL_VERSION 12
using namespace iwincore;
MobileClient::MobileClient()
{
	_tea = nullptr;
	msghandler = new GlobalMessageHandler();
	isConnect = false;
}


MobileClient::~MobileClient()
{
	if (_tea)
	{
		delete _tea;
        _tea = nullptr;
	}
	if (msghandler)
	{
		delete msghandler;
	}
	if (_socket)
	{
		delete _socket;
	}
}

void MobileClient::init(const char* server_ip, const char* server_port)
{
	_socket = new network::WebSocket();
	if (!_socket->init(*this, StringUtils::format("%s:%s", server_ip, server_port).c_str()) )
	{
		CC_SAFE_DELETE(_socket);
	}
	isConnect=false;
	
}

void MobileClient::CloseSocket()
{
	if (_socket)
	{
		delete _socket;
		_socket = nullptr;
	}
}

void MobileClient::onOpen(cocos2d::network::WebSocket* ws)
{
	CCLOG("Socket Open");
	DataWriter* data = new DataWriter();
	data->writeInt32(PROTOCOL_VERSION);
	unsigned char* bytes = new unsigned char;
	_send(bytes, data->getSize());
	isConnect = true;
}

void MobileClient::onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)
{
	CCLOG("Socket Message");
	iwincore::Message* msg = _decode(data);
	if (msg->getCommandId() == 500) {
		unsigned char* keyTea = msg->readBuffer(32);
		_tea = new TEA(keyTea, 32);
		if (msghandler != nullptr)
		{
			msghandler->onConnectOk();
		}
	}
	else
	{
		if (msghandler != nullptr)
		{
			msghandler->onMessage(msg);
		}
	}
	CC_SAFE_DELETE(msg);
}

void MobileClient::onClose(cocos2d::network::WebSocket* ws)
{
	CCLOG("Socket Close");
	isConnect = false;
	if (msghandler)
	{
		msghandler->onDisConnect();
	}
}

void MobileClient::onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error)
{
	CCLOG("Socket Error");
}

void MobileClient::_send(const unsigned char* binaryMsg, unsigned int len)
{
	CCASSERT(_socket, "socket cannot be null");
	_socket->send(binaryMsg, len);
}



void MobileClient::sendString(std::string text)
{
	CCLOG("%s", text.c_str());
	iwincore::Message* msg = new iwincore::Message(2800);
	msg->putString(text);
	sendMessage(msg);
}

void MobileClient::sendMessage(iwincore::Message* msg)
{
	int encryptLen = 0;
	int len = msg->getBufferSize();
	char* buffer_test = (char*)msg->getBufferBinary();
    unsigned char* encryptData = nullptr;
    if(_tea)
    {
        encryptData = (unsigned char*)_tea->Encrypt(buffer_test, len, encryptLen);
    }
    else{
        encryptData = new ubyte [2];
        encryptData[0] = 1;
    }
	DataWriter* buffer = new DataWriter();
	// Write message length = data length + 1 byte x
	buffer->writeInt32(encryptLen);
	buffer->writeBool(msg->isEncrypted() ? 1 : 0);
	buffer->writeBytes(encryptData, encryptLen);
	unsigned char* data = nullptr;
	int size = buffer->getSize();
	buffer->_getBytes(data);
	_socket->send((unsigned char*)data, size);
	CC_SAFE_DELETE_ARRAY(data);
	CC_SAFE_DELETE_ARRAY(encryptData);
	//fix leak mem
	CC_SAFE_DELETE(msg);
	CC_SAFE_DELETE(buffer);
}

iwincore::Message* MobileClient::_decode(cocos2d::network::WebSocket::Data data)
{
	DataReader* dataview = new DataReader((unsigned char*)data.bytes, data.len);
	int dataLen = dataview->readInt32();
	bool isEncrypted = dataview->readBool();
	unsigned char* msgContent = nullptr;
	int contentLen = dataLen - 1;
	iwincore::Message* msg;
	dataview->readBytes(msgContent, contentLen);
	if (isEncrypted)
	{
		int decryptLen;
		char* decryptContent = _tea->Decrypt((char*)msgContent, contentLen, decryptLen);
		msg = new iwincore::Message((unsigned char*)decryptContent, decryptLen);
	}
	else
	{
		msg = new iwincore::Message(msgContent, contentLen);
	}
	return msg;
	
}
