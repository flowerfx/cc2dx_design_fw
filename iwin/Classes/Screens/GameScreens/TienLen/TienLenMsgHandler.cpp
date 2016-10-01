#include "TienLenMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "TienLenScreen.h"
TienLenMsgHandler* TienLenMsgHandler::_instance = nullptr;
TienLenMsgHandler::TienLenMsgHandler()
{
}

TienLenMsgHandler::~TienLenMsgHandler()
{

}

int TienLenMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void TienLenMsgHandler::onMessage(iwincore::Message* msg)
{
	CCLOG("TienLenMsgHandler Main command: %d", msg->getCommandId());
	if (TienLenScreen::getInstance() == nullptr)
	{
		PASSERT2(false, "the game is exit so do-not receive msg anymore ! safe to continue");
		return;
	}

	switch (msg->getCommandId()) 
	{
		case IwinProtocol::RETURN_GAME:
		{
			int maxPlayer = GameController::getMaxPlayerOf(GameController::gameId);
			int limitStepTime = msg->readInt();
			int userIDMove = msg->readInt();
			char card_moved_leng = msg->readByte();
			vector<unsigned char> card_moved(card_moved_leng, 0);
			for (int i = 0; i < card_moved_leng; i++) {
				card_moved[i] = msg->readByte();
			}
			int next_user_id = msg->readInt();

			char card_leng = msg->readByte();
			std::vector<unsigned char> myCard;
			for (int i = 0; i < card_leng; i++)
			{
				myCard.push_back(msg->readByte());
			}
			vector<int> user_rank(maxPlayer, 0);
			vector<unsigned char>  pos_rank(maxPlayer, 0);
			vector<bool> user_skip(maxPlayer, false);
			for (int i = 0; i < maxPlayer; i++) {
				user_rank[i] = -1;
				pos_rank[i] = -1;
				user_skip[i] = false;
			}

			for (int i = 0; i < maxPlayer; i++) {
				int id = msg->readInt();
				if (id != -1) {
					user_rank[i] = id;
					pos_rank[i] = msg->readByte();
					user_skip[i] = msg->readBool();
				}
			}
			int limitTime = msg->readInt();
			//bool clearCurrentCards = true;
			/*if (GameController::getCurrentGameType() != GameType::GAMEID_TIENLEN_SOLO)
			clearCurrentCards = msg->readBool();*/
			TienLenScreen::getInstance()->OnReconnect(
				limitStepTime,
				userIDMove,
				card_moved,
				next_user_id,
				myCard,
				limitTime,
				user_rank,
				pos_rank,
				user_skip);

			//ScrMgr->SwitchToMenu(HUD_SCREEN, ScrMgr->GetCurrentMenuUI());
			break;
		}
		case IwinProtocol::SET_MONEY: {
			char roomID = msg->readByte();
			char boardID = msg->readByte();
			int money = msg->readInt();
			if (TienLenScreen::getInstance()->GetRoomID() == roomID
				&&  TienLenScreen::getInstance()->GetBoardID() == boardID) {
				 TienLenScreen::getInstance()->setBetMoney(money);
			}
		}
		break;
		case IwinProtocol::START: 
		{
			// TienLenScreen::getInstance()->resetGame();
			char roomID6 = msg->readByte();
			char boardID6 = msg->readByte();
			char interval = msg->readByte();
			std::vector<unsigned char> myCards;
			for (int i = 0; i < NUMBER_CARD_TL; i++)
			{
				myCards.push_back(msg->readByte());
			}
			int whoMoveFirst = msg->readInt();

			if (GameController::currentGameScreen != TienLenScreen::getInstance()) 
			{
				PASSERT2(false, "why currnet screen not a tienlenscreen ????");
				// show screen tien len 
			}

			if (TienLenScreen::getInstance()->GetBoardID() == boardID6
				&&  TienLenScreen::getInstance()->GetRoomID() == roomID6) 
			{
				TienLenScreen::getInstance()->start(whoMoveFirst, interval, myCards);
			}
		}
								  break;
		case IwinProtocol::MOVE:
			moveCommand(msg);
			break;
		case IwinProtocol::FINISH:
			finishCommand(msg);
			break;
		case IwinProtocol::STOP_GAME:
		{
			char roomID16 = msg->readByte();
			char boardID16 = msg->readByte();
			std::vector<char> cardsShow;

			if (!msg->isEndRead()) {
				int whoShowCard = msg->readInt();
				char nCards = msg->readByte();
				for (int i = 0; i < nCards; i++) {
					cardsShow.push_back(msg->readByte());
				}
				onStopGame(roomID16, boardID16, whoShowCard, cardsShow);
			}
			else
			{
				onStopGame(roomID16, boardID16, -1, cardsShow);
			}

		}
		break;

		case IwinProtocol::SKIP_TIENLEN:
			skipCommand(msg);
			break;
		case IwinProtocol::FORCE_FINISH: // Tới trắng
			forceFinishCommand(msg);
			break;
		case IwinProtocol::MOVE_ERROR:
			moveErrorCommand(msg);
			break;

		case IwinProtocol::GET_TIME_BAR:
		{
			char timeStart = msg->readByte();
			TienLenScreen::getInstance()->onGetTimeLeftAutoStart(timeStart);
		}
		break;
		case IwinProtocol::TIENLENSOLO_COMMAND:
		{
			char subCommand = msg->readByte();
			CCLOG("TienLenMessageScreen Sub command: %d"
				, subCommand);
			switch ((int)subCommand) {
			case 1:// board_info
			{
				char timeRemain = msg->readByte();
				TienLenScreen::getInstance()->onGetTimeLeftAutoStart(timeRemain);
			}
			break;
			case 2:// move
				moveCommand(msg);
				break;
			case 3:// skip
				skipCommand(msg);
				break;
			case 4:// force finish
				forceFinishCommand(msg);
				break;
			case 5:// finish
				finishCommand(msg);
			case 6:// move error
				moveErrorCommand(msg);
				break;
			}
		}
		break;
	}
}
void TienLenMsgHandler::onSomeOneFinish(ubyte roomID10, ubyte boardID10, int whoFinish,
	ubyte finishPosition, int dMoney, int dExp)
{
}

void TienLenMsgHandler::onStopGame(ubyte roomID11, ubyte boardID11, int whoShowCards,
	std::vector<char> cardsShow)
{
	if (TienLenScreen::getInstance() && TienLenScreen::getInstance()->GetBoardID() == boardID11
		&& TienLenScreen::getInstance()->GetRoomID() == roomID11)
	{
		TienLenScreen::getInstance()->StopGame(whoShowCards, cardsShow);

	}
}

void TienLenMsgHandler::moveCommand(iwincore::Message* msg)
{
	char roomID = msg->readByte();
	char boardID = msg->readByte();
	int whoMove = msg->readInt();
	char nCard = msg->readByte();
	std::vector<unsigned char> cards;
	for (int i = 0; i < nCard; i++)
	{
		cards.push_back(msg->readByte());
	}
	int nextMove = msg->readInt();
	if (TienLenScreen::getInstance()->GetBoardID() == boardID
		&& TienLenScreen::getInstance()->GetRoomID() == roomID)
	{
		TienLenScreen::getInstance()->Move(whoMove, cards, nextMove);
	}
}

void TienLenMsgHandler::skipCommand(iwincore::Message* msg)
{
	int roomID = msg->readByte();
	int boardID = msg->readByte();
	int whoSkip = msg->readInt();
	int nextMove = msg->readInt();
	bool clearCurrentCards = true;
	//if (GameController::gameId != GameController::getCurrentGameType())
		clearCurrentCards = msg->readBool();
	if (!TienLenScreen::getInstance())
	{
		PASSERT2(false, "the screen not init or be destroyed!");
		return;
	}
	if (TienLenScreen::getInstance()->GetBoardID() == boardID
		&&TienLenScreen::getInstance()->GetRoomID() == roomID)
	{
		TienLenScreen::getInstance()->skip(whoSkip, nextMove,
			clearCurrentCards);
	}
}

void TienLenMsgHandler::finishCommand(iwincore::Message* msg)
{
	char roomID15 = msg->readByte();
	char boardID15 = msg->readByte();
	int whoFinish = msg->readInt();
	char finishPosition = msg->readByte();
	int d_money = 0;
	if (GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
		d_money = msg->readInt();
	int d_exp = msg->readInt();
	TienLenScreen::getInstance()->Finish(whoFinish, finishPosition, d_money, d_exp);
}

void TienLenMsgHandler::forceFinishCommand(iwincore::Message* msg)
{
	char roomID18 = msg->readByte();
	char boardID18 = msg->readByte();

	if (TienLenScreen::getInstance()
		&& TienLenScreen::getInstance()->GetBoardID() == boardID18
		&& TienLenScreen::getInstance()->GetRoomID() == roomID18)
	{
		if (TienLenScreen::getInstance())
		{
			TienLenScreen::getInstance()->ForceFinish();
		}
	}
}
void TienLenMsgHandler::moveErrorCommand(iwincore::Message* msg)
{
}


