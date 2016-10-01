#ifndef __PHOM_MSG_HANDLER_H__
#define __PHOM_MSG_HANDLER_H__
#include "Network/Core/IMessagehandler.h"
#include "Network/Core/Message.h"

class PhomScreen;

class PhomMsgHandler :public IMessagehandler
{
private:
	static PhomMsgHandler* _instance;
public:
	static PhomMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new PhomMsgHandler();
		return _instance;
	}
	PhomMsgHandler();
	virtual ~PhomMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;


	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}

	PhomScreen*	GetPhomScreen();


};

#endif //__TIENLEN_MSG_HANDLER_H__

