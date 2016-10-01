#ifndef __MOBILE_CLIENT_H__
#define __MOBILE_CLIENT_H__
#include "Message.h"
#include "IMessagehandler.h"
#include "../network/WebSocket.h"
#include "Network/Global/GlobalMessageHandler.h"


//class iwincore::Message;
class TEA;

class MobileClient :public cocos2d::network::WebSocket::Delegate
{
private:
	cocos2d::network::WebSocket* _socket;
	
	TEA*	_tea;
	
	void _send(const unsigned char* binaryMsg, unsigned int len);

	iwincore::Message* _decode(cocos2d::network::WebSocket::Data data);

public:
				MobileClient();
	virtual		~MobileClient();
	bool			isConnect;
	IMessagehandler* msghandler;
	void init(const char* server_ip, const char* server_port);
	void CloseSocket();

	virtual void onOpen(cocos2d::network::WebSocket* ws)override;
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data)override;
	virtual void onClose(cocos2d::network::WebSocket* ws)override;
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error)override;
	
	void sendString(std::string text);
	void sendMessage(iwincore::Message* msg);
};

#endif // __MOBILE_CLIENT_H__