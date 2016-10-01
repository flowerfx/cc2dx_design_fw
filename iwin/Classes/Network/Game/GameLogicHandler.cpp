#include "GameLogicHandler.h"

#include "Models/Player.h"
#include "Network/Core/Message.h"
#include "Screens/ScreenManager.h"
#include "Network/Global/GlobalLogicHandler.h"
#include "Network/Global/Globalservice.h"
#include "Common/ChipParticle.h"
#include "Screens/GameScreens/GameScreens.h"
#include "Network/JsonObject/JsonHeader.h"

bool GameLogicHandler::isJoinGame = false;
IMessagehandler* GameLogicHandler::gameHandlers = nullptr;
GameLogicHandler::GameLogicHandler()
{

}

GameLogicHandler::~GameLogicHandler()
{

}


void GameLogicHandler::onShowGameScreen(bool isReconnect, s16 roomId, s16 boardId, int ownerId, int money, std::vector<Player*> players)
{
	if (isReconnect)
	{
		GlobalService::sendSingleCommand(IwinProtocol::RECONNECTED_AND_GOTOGAME);
	}

	//ScrMgr->SendDataToScreen(MENU_LAYER::HUD_SCREEN, IwinProtocol::JOIN_BOARD, cv_void(roomId), cv_void(boardId), cv_void(ownerId), cv_void(money), cv_void(players));
	GameController::showGameScreen(roomId, boardId, ownerId, money, players);

}


void GameLogicHandler::onBonusMoney(ubyte roomID, ubyte boardID, int playerID, int money, std::string des, s64 currMoney)
{
	if (GameController::currentGameScreen != nullptr && 
		GameController::currentGameScreen->IsShow())
	{
		if (playerID == GameController::myInfo->IDDB)
		{
			//change info
			GameController::myInfo->setMoney(currMoney);
			GameController::myInfo->userProfile->money = currMoney;
			//notice change info push to all screen
			ScrMgr->MyInfoChanged();
		}

		GameController::currentGameScreen->onBonusMoney(playerID, money, des, currMoney);
	}
	else 
	{
		//khac ở trong game
		if (money == 0)
			return;

		if (money > 0)//bay particle
		{
			ChipParticle::getInstance()->visiable(true);
			//playsound
			GetSound->PlaySoundEffect("SFX_COIN_DROP");
		}
		else
		{

		}

		if (playerID == GameController::myInfo->IDDB)
		{
			//change info
			GameController::myInfo->setMoney(currMoney);
			GameController::myInfo->userProfile->money = currMoney;
			//notice change info push to all screen
			ScrMgr->MyInfoChanged();
		}
	}
}
	
void GameLogicHandler::onSetBetRange(iwinmesage::GetMinMaxBoard * object)
{
	if (GameController::currentGameScreen != nullptr 
		&& object != nullptr 
		&& GameController::gameId == object->getGameId()
		&& GameController::currentGameScreen->GetBoardID() == object->getBoardID()
		&& GameController::currentGameScreen->GetRoomID()  == object->getRoomID()) 
	{
		s64 max_money = object->getMaxMoney();
		// lay max va min trong object
		GameController::currentGameScreen->onBetRange(
			object->getMinMoney(), 
			max_money,
			object->getMaxMoney());
	}
}

void GameLogicHandler::onChatInBoard(ubyte roomid, ubyte boarid, std::string msg, int fromid)
{
	if (GameController::currentGameScreen != nullptr
		&& GameController::currentGameScreen->GetBoardID() == boarid
		&& GameController::currentGameScreen->GetRoomID() == roomid)
	{
		GameController::currentGameScreen->OnChatInBoard(fromid, msg);
	}
}

void GameLogicHandler::onKicked(ubyte roomID, ubyte boardID, int kicked, std::string reason)
{
	if (GameController::currentGameScreen != nullptr
		&& GameController::currentGameScreen->GetBoardID() == boardID
		&& GameController::currentGameScreen->GetRoomID() == roomID)
	{
		GameController::currentGameScreen->kicked(roomID, boardID, kicked, reason);
	}
}

void GameLogicHandler::onSystemChat(ubyte roomid, ubyte boarid, std::string msg)
{
	if (GameController::currentGameScreen != nullptr
		&& GameController::currentGameScreen->GetBoardID() == boarid
		&& GameController::currentGameScreen->GetRoomID() == roomid)
	{
		GameController::currentGameScreen->OnSystemChat(msg);
	}
}

void GameLogicHandler::onReceivedWaitTimeInfo(int sec)
{
	if (GameController::currentGameScreen != nullptr)
	{
		GameController::currentGameScreen->OnReceivedWaitTimeInfo(sec);
	}
}
