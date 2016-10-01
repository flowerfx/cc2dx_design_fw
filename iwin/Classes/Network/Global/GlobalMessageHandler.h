#ifndef __GLOBAL_MESSAGE_HANDLER_H__
#define  __GLOBAL_MESSAGE_HANDLER_H__
#include "../Core/Message.h"
#include "Network/JSONMessageHandler.h"
#include "../Core/IMessagehandler.h"

class GlobalMessageHandler:public IMessagehandler
{
private:
	JSONMessageHandler* jsonMessageHandler;
	GlobalLogicHandler* globalLogicHandler;
	std::string createOnlineInfo(ubyte room, ubyte board, ubyte gameId);
public:
	GlobalMessageHandler();
	virtual ~GlobalMessageHandler();
	virtual void onMessage(iwincore::Message* msg);
	virtual void onConnectOk();
	virtual void onConnectFail();
	virtual void onDisConnect();
	
};

#endif // __GLOBAL_MESSAGE_HANDLER_H__