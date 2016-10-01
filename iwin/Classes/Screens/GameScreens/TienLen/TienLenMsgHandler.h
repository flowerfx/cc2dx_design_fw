#ifndef __TIENLEN_MSG_HANDLER_H__
#define __TIENLEN_MSG_HANDLER_H__
#include "../../../Network/Core/IMessagehandler.h"
#include "../../../Network/Core/Message.h"

class TienLenMsgHandler :public IMessagehandler
{
private:
	static TienLenMsgHandler* _instance;
public:
	static TienLenMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new TienLenMsgHandler();
		return _instance;
	}
	TienLenMsgHandler();
	virtual ~TienLenMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;


	void onPostSetupGameType() {
	}

	void onSomeOneFinish(ubyte roomID10, ubyte boardID10, int whoFinish,
		ubyte finishPosition, int dMoney, int dExp);

	void onStopGame(ubyte roomID11, ubyte boardID11, int whoShowCards,
		std::vector<char> cardsShow);

	void moveCommand(iwincore::Message* msg);

	void skipCommand(iwincore::Message* msg);

	void finishCommand(iwincore::Message* msg);

	void forceFinishCommand(iwincore::Message* msg);
	void moveErrorCommand(iwincore::Message* msg);

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};

#endif //__TIENLEN_MSG_HANDLER_H__

