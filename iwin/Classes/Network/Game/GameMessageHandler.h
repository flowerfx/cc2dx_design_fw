#ifndef __GAME_MESSAGE_HANDLER_H__
#define __GAME_MESSAGE_HANDLER_H__

#include "Network/Core/IMessagehandler.h"

class GameLogicHandler;

class GameMessageHandler : public IMessagehandler
{

private:
	static GameMessageHandler* s_instance;
	
	GameLogicHandler*			_gameLogicHandler;

public:
	static GameMessageHandler* getInstance();

	GameMessageHandler();
	virtual ~GameMessageHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual void onConnectOk() override;
	virtual void onConnectFail() override;
	virtual void onDisConnect() override;
	void onReceiveMessageJson(std::string msg_json, std::string id_msg);
};

#define GetGameMessageHandler() GameMessageHandler::getInstance()

#endif // __GAME_MESSAGE_HANDLER_H__