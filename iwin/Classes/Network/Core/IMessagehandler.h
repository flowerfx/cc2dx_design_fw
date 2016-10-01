#ifndef __IMESSAGE_HANDLER_H__
#define __IMESSAGE_HANDLER_H__
#include "Message.h"
class IMessagehandler
{
public:
	virtual ~IMessagehandler() {};
	//this function to process the message
	virtual void onMessage(iwincore::Message* msg) = 0;
	//this function to save the message;
	virtual int onSaveMessage(iwincore::Message* msg);
	//
	virtual void onConnectOk() = 0;
	virtual void onConnectFail() = 0;
	virtual void onDisConnect() = 0;

};

#endif