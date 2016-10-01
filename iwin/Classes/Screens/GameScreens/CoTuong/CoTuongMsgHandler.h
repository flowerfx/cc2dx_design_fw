#ifndef __COTUONG_MSG_HANDLER_H__
#define __COTUONG_MSG_HANDLER_H__
#include "Network/Core/IMessagehandler.h"
#include "Network/Core/Message.h"

class CoTuongMsgHandler :public IMessagehandler
{
private:
	static CoTuongMsgHandler* _instance;
public:
	static CoTuongMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new CoTuongMsgHandler();
		return _instance;
	}
	CoTuongMsgHandler();
	virtual ~CoTuongMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};

#endif //__COTUONG_MSG_HANDLER_H__

