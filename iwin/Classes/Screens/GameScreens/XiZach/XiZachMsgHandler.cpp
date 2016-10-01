#include "XiZachMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "XiZachScreen.h"
XiZachMsgHandler* XiZachMsgHandler::_instance = nullptr;
XiZachMsgHandler::XiZachMsgHandler()
{
}

XiZachMsgHandler::~XiZachMsgHandler()
{

}

int XiZachMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void XiZachMsgHandler::onMessage(iwincore::Message* msg)
{

	CCLOG("XiZachMsgHandler Main command: %d", msg->getCommandId());
	if (XiZachMsgHandler::getInstance() == nullptr)
	{
		PASSERT2(false, "the game is exit so do-not receive msg anymore ! safe to continue");
		return;
	}
	try
	{
		byte roomID, boardID;
		int user, money;
		switch (msg->getCommandId())
		{
		case 261:
		{
			byte timeStart = msg->readByte();
			XiZachScreen::getInstance()->OnShowCountDown(timeStart, 0);
		}
		break;
		case IwinProtocol::QUICK_PLAY:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			user = msg->readInt();
			bool isQuickPlay = msg->readBool();
			XiZachScreen::getInstance()->ShowQuickPlay(user, isQuickPlay);
		}
		break;
		case IwinProtocol::WANT_DRAW:
			// command so 24 dung de set tien dat
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			money = msg->readInt();
			user = msg->readInt();
			if (GameController::currentGameScreen->GetRoomID() == roomID &&GameController::currentGameScreen->GetBoardID() == boardID)
			{
				XiZachScreen::getInstance()->OnBetMoney(money, user);
			}
		}
		break;
		case IwinProtocol::TETRIS_COMMAND:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			user = msg->readInt();
			money = msg->readInt();
			if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
			{
				XiZachScreen::getInstance()->OnBetMoney(money, user);
			}
		}
		break;
		case IwinProtocol::STOP_GAME:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			byte numPlayers = msg->readByte();
			vector<vector<int>> cardR(numPlayers);
			/*	int[, ] cardR = new int[numPlayers, 7];*/
			for (int i = 0; i < numPlayers; i++)
			{
				vector<int> ls(7, -1);
				cardR[i] = ls;
			}
			for (int i = 0; i < numPlayers; i++)
			{
				cardR[i][0] = msg->readInt();
				cardR[i][1] = msg->readByte();
				for (int j = 0; j < cardR[i][1]; j++)
				{
					cardR[i][(j + 2)] = msg->readByte();
				}
			}
			if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
			{
				XiZachScreen::getInstance()->OnStopGame(numPlayers, cardR);
			}
		}
		break;
		case IwinProtocol::PLAYING:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			byte interval = msg->readByte();
			byte numPlayers = msg->readByte();
			vector<vector<int>> cards(numPlayers);
			//int[, ] cards = new int[numPlayers, 7];
			for (int i = 0; i < numPlayers; i++)
			{
				vector<int> ls(7, -1);
				cards[i] = ls;
			}
			for (int i = 0; i < numPlayers; i++)
			{
				cards[i][0] = msg->readInt();
				cards[i][1] = msg->readByte();
				for (int j = 0; j < cards[i][1]; j++)
				{
					cards[i][(j + 2)] = msg->readByte();
				}
			}
			if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
			{
				XiZachScreen::getInstance()->OnPlaying(interval, numPlayers, cards);
			}
		}
		break;
		case IwinProtocol::START:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			user = msg->readInt();
			byte interval = msg->readByte();
			vector<char> myCards(2);
			for (size_t i = 0; i < myCards.size(); i++)
			{
				myCards[i] = msg->readByte();
			}
			if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
			{
				XiZachScreen::getInstance()->OnStartGame(myCards, interval, user);
			}
		}
		break;
		case 91:
		{
			user = msg->readInt();
			string des1 = msg->readString();
			XiZachScreen::getInstance()->SetScores(user, des1);
		}
		break;
		case IwinProtocol::MOVE:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			byte type = msg->readByte();
			user = msg->readInt();
			switch (type)
			{
			case 0:
			{
				byte idCard = msg->readByte();
				if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
				{
					XiZachScreen::getInstance()->OnRetrieveCard(user, idCard);
				}
			}
			break;
			case 1:
			{
				byte time = msg->readByte();
				if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
				{
					XiZachScreen::getInstance()->OnSetMove(user, time);
				}
			}
			break;
			case 2:
			{
				// danh cho thang cai, den luot cai di,cai xet bai thang nao
				byte numCards = msg->readByte();
				vector<char> cardsID(numCards);
				for (int i = 0; i < numCards; i++)
				{
					cardsID[i] = msg->readByte();
				}
				if (GameController::currentGameScreen->GetRoomID() == roomID && GameController::currentGameScreen->GetBoardID() == boardID)
				{
					XiZachScreen::getInstance()->OnSetMoveOwner(user, cardsID);
				}
			}
			break;
			}
		}
		break;
		case IwinProtocol::RETURN_GAME:
		{
			int nPlayer = msg->readInt();
			bool isPlaying = msg->readBool();
			bool isOwnerTurn = false;
			int limit_time = 0;
			int remain_time = 0;
			int crMoveID = -1;
			vector<char> myCards(5, -1);
			if (isPlaying)
			{
				isOwnerTurn = msg->readBool();
				limit_time = msg->readInt();
				remain_time = msg->readInt();
				crMoveID = msg->readInt();
				int card_leng = msg->readInt();
				for (int i = 0; i < card_leng; i++)
				{
					myCards[i] = msg->readByte();
				}
			}
			vector<vector<int>> playerCards(nPlayer);
			//int[, ] playerCards = new int[nPlayer, 8];
			for (int i = 0; i < nPlayer; i++)
			{
				vector<int> ls(8, -1);
				playerCards[i] = ls;
			}
			vector<int> playerBetMoneys(nPlayer);
			int num = 0;
			for (int i = 0; i < nPlayer; i++)
			{
				playerCards[i][0] = msg->readInt();// user id
				if (playerCards[i][0] != -1)
				{
					playerCards[i][1] = msg->readByte();//status da bi thang cai lat chua
					playerBetMoneys[i] = msg->readInt();
					if (isPlaying)
					{
						num = msg->readInt();// num card
						playerCards[i][2] = num;
						for (int j = 0; j < num; j++)
						{
							playerCards[i][(3 + j)] = msg->readByte();
						}
					}
				}
			}
			XiZachScreen::getInstance()->OnReconnect(
				limit_time,
				remain_time,
				myCards,
				crMoveID,
				playerCards,
				playerBetMoneys,
				nPlayer,
				isPlaying,
				isOwnerTurn);
			break;
		}
		}
	}
	catch (exception e)
	{
	}

}


