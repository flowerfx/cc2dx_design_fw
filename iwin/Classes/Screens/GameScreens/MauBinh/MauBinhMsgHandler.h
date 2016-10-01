#ifndef __MAUBINH_MSG_HANDLER_H__
#define __MAUBINH_MSG_HANDLER_H__
#include "Network/Core/IMessagehandler.h"
#include "Network/Core/Message.h"

class MauBinhMsgHandler :public IMessagehandler
{
private:
	static MauBinhMsgHandler* _instance;
public:
	static MauBinhMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new MauBinhMsgHandler();
		return _instance;
	}
	MauBinhMsgHandler();
	virtual ~MauBinhMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};

#endif //__TIENLEN_MSG_HANDLER_H__

