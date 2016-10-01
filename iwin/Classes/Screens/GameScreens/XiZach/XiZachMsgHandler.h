#ifndef __XIZACH_MSG_HANDLER_H__
#define __XIZACH_MSG_HANDLER_H__
#include "../../../Network/Core/IMessagehandler.h"
#include "../../../Network/Core/Message.h"

class XiZachMsgHandler :public IMessagehandler
{
private:
	static XiZachMsgHandler* _instance;
public:
	static XiZachMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new XiZachMsgHandler();
		return _instance;
	}
	XiZachMsgHandler();
	virtual ~XiZachMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};

#endif //__TIENLEN_MSG_HANDLER_H__

