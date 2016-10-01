#ifndef __BAICAO_MSG_HANDLER_H__
#define __BAICAO_MSG_HANDLER_H__
#include "../../../Network/Core/IMessagehandler.h"
#include "../../../Network/Core/Message.h"

class BaiCaoMsgHandler :public IMessagehandler
{
private:
	static BaiCaoMsgHandler* _instance;
public:
	static BaiCaoMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new BaiCaoMsgHandler();
		return _instance;
	}
	BaiCaoMsgHandler();
	virtual ~BaiCaoMsgHandler();

	virtual void onMessage(iwincore::Message* msg)	   override;
	virtual int  onSaveMessage(iwincore::Message* msg) override;

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};
#endif
/// __BAICAO_MSG_HANDLER_H__
