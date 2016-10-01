#include "GameMessageHandler.h"
#include "Network/IwinProtocol.h"
#include "Models/Player.h"
#include "Common/GameController.h"
#include "Network/Game/GameLogicHandler.h"
#include "Screens/GameScreens/GameScreens.h"
#include "Screens/ScreenManager.h"
#include "Network/IwinCommand.h"

#include "Network/JsonObject/JsonHeader.h"
GameMessageHandler* GameMessageHandler::s_instance = nullptr;

GameMessageHandler* GameMessageHandler::getInstance()
{
	if (!s_instance)
	{
		s_instance = new GameMessageHandler();
	}
	return s_instance;
}


GameMessageHandler::GameMessageHandler()
{
	_gameLogicHandler = new GameLogicHandler();
}


GameMessageHandler::~GameMessageHandler()
{
	CC_SAFE_DELETE(_gameLogicHandler);
}


void GameMessageHandler::onMessage(iwincore::Message* msg)
{
	if (GameLogicHandler::gameHandlers != nullptr)
	{
		auto msg_save_res = GameLogicHandler::gameHandlers->onSaveMessage(msg);
		if (msg_save_res != 0)
		{
			CCLOG("the msg cannot excute, push to list and wait");
			return;
		}
	}

	switch (msg->getCommandId())
	{
	case IwinProtocol::BONUS_MONEY:
	{
		ubyte roomID = msg->readByte();
		ubyte boardID = msg->readByte();
		int playerID = msg->readInt();
		int money = msg->readInt();
		std::string des = msg->readString();
		s64 currMoney = -1;
		if (!msg->isEndRead()) {
			currMoney = msg->readLong(); // số win hiện tại của user
		}
		_gameLogicHandler->onBonusMoney(roomID, boardID, playerID, money, des, currMoney);
	}
	break;
	case IwinProtocol::RECONNECTED_AND_GOTOGAME:
	case IwinProtocol::JOIN_VIEWER:
	case IwinProtocol::JOIN_BOARD:
	{
		//chien handle here
		/*if (ScrMgr->GetCurrentMenuUI() == LOGIN_SCREEN)
		{
			ScrMgr->OnShowDialog(L"Reconnect", L"Đang kết nối lại !",DIALOG_ZERO_BUTTON_WAIT,"reconnect",
			[msg](const char * str)
			{
				GetGameMessageHandler()->onMessage(msg);
			});
			return;
		}*/

		ubyte roomID = msg->readByte();
		ubyte boardID = msg->readByte();
		int ownerID = msg->readInt();
		int money = msg->readInt();
		// Read player infos here.
		std::vector<Player*> players;
		bool available = true;
		while (available) {
			Player* playerInfo = GameController::createPlayerByGameID();
			if (msg->isEndRead()) break;
			playerInfo->IDDB = msg->readInt();
			if (playerInfo->IDDB == -1) {
				playerInfo->setName("");

			}
			else {
				playerInfo->setName(msg->readString());
				playerInfo->setMoney(msg->readLong());
				playerInfo->avatar = msg->readShort();
				playerInfo->setExp(msg->readInt());
				playerInfo->isReady = msg->readBool();
				if (playerInfo->IDDB == GameController::myInfo->IDDB) {
					GameController::myInfo->setExp(playerInfo->getExp());
					if (GameController::currentMoneyType == RUBY_MONEY)
					{
						GameController::myInfo->setRuby(playerInfo->getMoney());
					}
					else
					{
						GameController::myInfo->setMoney(playerInfo->getMoney());
					}
					

				}

			}
			players.push_back(playerInfo);

		}

		// handle reconnect
		_gameLogicHandler->onShowGameScreen(msg->getCommandId() == IwinProtocol::RECONNECTED_AND_GOTOGAME, roomID, boardID,
			ownerID, money, players);

	}
	break;
	case IwinProtocol::SOMEONE_JOINBOARD: // On someone join the
										 // board.
	{
		Player* playerInfo = GameController::createPlayerByGameID();
		int seat = msg->readByte();
		playerInfo->IDDB = msg->readInt();
		playerInfo->setName(msg->readString());
		playerInfo->setMoney(msg->readLong());
		playerInfo->avatar = msg->readShort();
		playerInfo->setExp(msg->readInt());
		playerInfo->isReady = false;
		playerInfo->finishPosition = -1;
		if (GameController::currentGameScreen != nullptr)
			(GameController::currentGameScreen)->onSomeOneJoinBoard(seat, playerInfo);
	}
	break;
	case IwinProtocol::SOMEONE_LEAVEBOARD: {
		int leaveID = msg->readInt();
		int newOwnerID = msg->readInt();
		if (GameController::currentGameScreen != nullptr)
			GameController::currentGameScreen->onSomeOneLeaveBoard(leaveID, newOwnerID);
	}
										   break;
	case IwinProtocol::CONFIRM_LEAVE_GAME: // confirm xác nh?n r?i bàn
	{
		string msgConfirm = msg->readString();
		if (GameController::currentGameScreen) {
			GameController::currentGameScreen->doConfirmQuit(msgConfirm);

		}
	}
	break;
	case IwinProtocol::LEAVE_BOARD: // confirm xác nh?n r?i bàn
	{
		if (GameController::currentGameScreen) {
			GameController::currentGameScreen->doQuit();
			ScrMgr->OnHideDialog();
            GameController::currentGameScreen = nullptr;
		}
	}
	break;
	case IwinProtocol::READY: // On someone ready.
	{
		int ID = msg->readInt();
		bool isReady = msg->readBool();
		if (GameController::currentGameScreen != nullptr
			&& GameController::getCurrentGameType() != GameType::GAMEID_GIAI_DAU_CO_UP)
		{
			GameController::currentGameScreen->onSomeOneReady(ID, isReady);
		}
	}
	break;
	case IwinProtocol::KICK: {
		ubyte roomID = msg->readByte();
		ubyte boardID = msg->readByte();
		int kicked = msg->readInt();
		std::string reason = "";
		if (!msg->isEndRead()) reason = msg->readString();

		_gameLogicHandler->onKicked(roomID, boardID, kicked, reason);
		break;
	}
	case IwinProtocol::ON_CHAT_BOARD:// Chat trong ban choi.
	{
		ubyte roomID = msg->readByte();
		ubyte boardID = msg->readByte();
		int from = msg->readInt();
		std::string text = msg->readString();
		_gameLogicHandler->onChatInBoard(roomID, boardID, text, from);
	}
	break;
	case IwinProtocol::SYSTEM_INFO: 
	{
		ubyte roomID = msg->readByte();
		ubyte boardID = msg->readByte();
		std::string text = msg->readString();
		_gameLogicHandler->onSystemChat(roomID, boardID, text);
	}
	break;
	case IwinProtocol::GAME_RESULT:
	{
		// Nhan ket qua sau cuoc choi.
		std::vector<GameResult*> l;
		while (!msg->isEndRead()) {
			GameResult* info12 = new GameResult();
			info12->result = msg->readByte();
			info12->ID = msg->readInt();
			info12->d_exp = msg->readInt();
			info12->d_money = msg->readInt();
			l.push_back(info12);	
		}
		if (GameController::currentGameScreen != nullptr) 
		{
			GameController::currentGameScreen->OnMatchResult(l);
		}
	}
	break;
	case IwinProtocol::ERROR_MESSAGE:
	{
		std::string error = msg->readString();
		ScrMgr->OnShowDialog("error", msg->readString(), DIALOG_ONE_BUTTON);
	}
	break;
	case IwinProtocol::INVITE_PLAYER: 
	{
		ubyte gameId = msg->readByte();
		ubyte roomId = msg->readByte();
		ubyte boardID = msg->readByte();
		std::string password = msg->readString();
		std::string message = msg->readString();
		ubyte levelRoom = msg->readByte();// them byte level tu
										// 247
		int idInvite = msg->readInt();// them byte id nguoi moi
									 // tu 248
		//gameLogicHandler.onInvitedJoinBoard(gameId, roomId, boardID, password, message, levelRoom,
		//	idInvite);
	}
	break;
	case IwinProtocol::BUZZ: 
	{
		int userid = msg->readInt();
		std::string content = msg->readString();
		if (GameController::currentGameScreen) {
			GameController::currentGameScreen->onBuzz(userid, content);
		}
	}
	break;
	case 1302:// thong bao tu choi
	{
		ScrMgr->OnShowDialog("notice", msg->readString(), DIALOG_ONE_BUTTON);
	}
	break;
	case IwinProtocol::WAIT_TIME_INFO:
	{
		int second = msg->readInt();
		if (GameController::getCurrentGameType() != GameType::GAMEID_GIAI_DAU_CO_UP) 
		{
			_gameLogicHandler->onReceivedWaitTimeInfo(second);
		}	
	}
	break;
	case IwinProtocol::MONEY_TYPE_INFO:
	{
		int moneyType = msg->readInt();
		int countUser = msg->readInt();
		int iddb;
		s64 money;
		GameController::currentMoneyType = moneyType;

		if (GameController::currentGameScreen)
			GameController::currentGameScreen->updateTextWithMoneyType();

		if (GameController::currentMoneyType == RUBY_MONEY && GameController::currentGameScreen)
		{
			for (int i = 0; i < countUser; i++)
			{
				iddb = msg->readInt();
				money = msg->readLong();
				GameController::currentGameScreen->setRubyMoney(iddb, money);
			}
		}

	}
	break;
	case IwinProtocol::GET_WAITING_LIST:
	{
		// danh sach cho
		std::vector<Player*> l;

		bool available = true;
		while (!msg->isEndRead())
		{
			Player * p = new Player();
			p->IDDB = msg->readInt();
			p->setName(msg->readString());
			p->setMoney(msg->readLong());
			p->avatar = msg->readShort();
			p->setExp(msg->readInt());
			p->isReady = true; // Is online
			p->userProfile->status = msg->readString();
			l.push_back(p);

			
		}

		if (GameController::currentGameScreen) 
		{
			GameController::currentGameScreen->OnReceiveWaitingList(l);
		}
		else
		{
			PASSERT2(false, "msg waiting list receive here");
			for (auto p : l)
			{
				delete p;
			}
			l.clear();
		}
		break;
	}
	default:
		if (GameLogicHandler::gameHandlers != nullptr) 
		{
			GameLogicHandler::gameHandlers->onMessage(msg);
		}
		else
		{
			CCLOG("No handler for message: %d", msg->getCommandId());
		}

	}
}

void GameMessageHandler::onReceiveMessageJson(std::string msg_json, std::string id_msg)
{
	if (id_msg == IwinCommand::GET_MIN_MAX_BOARD)
	{
		GetMinMaxBoard * obj = new GetMinMaxBoard();
		obj->toData(msg_json);
		_gameLogicHandler->onSetBetRange(obj);
		delete obj;
	}
}

void GameMessageHandler::onConnectOk()
{

}

void GameMessageHandler::onConnectFail()
{

}

void GameMessageHandler::onDisConnect()
{

}