#include "BaiCaoMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "BaiCaoScreen.h"
BaiCaoMsgHandler* BaiCaoMsgHandler::_instance = nullptr;
BaiCaoMsgHandler::BaiCaoMsgHandler()
{
}

BaiCaoMsgHandler::~BaiCaoMsgHandler()
{

}

int BaiCaoMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void BaiCaoMsgHandler::onMessage(iwincore::Message* msg)
{
	CCLOG("BaiCaoMsgHandler Main command: %d", msg->getCommandId());
	try
	{
		ubyte roomID, boardID;
		int money, id;
		switch (msg->getCommandId())
		{
		case IwinProtocol::WANT_DRAW:
			// BAI CAO
			// command so 24 dung de set tien dat cua rieng tung nguoi
			// (khong phai cai)
			roomID = msg->readByte();
			boardID = msg->readByte();
			money = msg->readInt();
			id = msg->readInt();

			if (BaiCaoScreen::getInstance()->GetRoomID() == roomID
				&& BaiCaoScreen::getInstance()->GetBoardID() == boardID)
			{
				BaiCaoScreen::getInstance()->OnBetMoney(money, id);
			}
			break;
		case 80:
			// BAI CAO
			// command so 80 dung de dat tien cuoc cho tat ca player
			// cmd nay server tra ve tu dong
			roomID = msg->readByte();
			boardID = msg->readByte();
			id = msg->readInt();
			money = msg->readInt();
			if (BaiCaoScreen::getInstance()->GetRoomID() == roomID
				&& BaiCaoScreen::getInstance()->GetBoardID() == boardID)
			{
				BaiCaoScreen::getInstance()->OnBetMoney(money, id);
			}
			break;
		case IwinProtocol::SET_MONEY:
			// CAO RUA
			// command 19 chu ban dat muc cuoc trong game bai cao rua
			roomID = msg->readByte();
			boardID = msg->readByte();
			money = msg->readInt();
			if (BaiCaoScreen::getInstance()->GetRoomID() == roomID
				&& BaiCaoScreen::getInstance()->GetBoardID() == boardID)
			{
				BaiCaoScreen::getInstance()->ChangeBetMoney(money, roomID, boardID);
			}
			break;

		case IwinProtocol::STOP_GAME: // 50
									 // stop game, server gui ve
									 // Tien của 4 thằng co the = -1 nếu player ko tồn tại
									 // Bài của 4 thằng, có thể = -1 nếu player ko tồn tại
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			ubyte bPlayer = msg->readByte();
			std::vector<std::vector<int>> cardR;

			for (int i = 0; i < bPlayer; i++)
			{
				std::vector<int> pcard;
				pcard.push_back(msg->readInt());
				for (int j = 1; j < 4; j++)
				{
					pcard.push_back(msg->readByte());
				}
				cardR.push_back(pcard);
			}
			if (BaiCaoScreen::getInstance()->GetRoomID() == roomID
				&& BaiCaoScreen::getInstance()->GetBoardID() == boardID)
			{
				//DialogUtil.hideAllDialog();
				BaiCaoScreen::getInstance()->OnStopGame(bPlayer, cardR);
			}
		}
		break;
		case 91:
		{
			int who1 = msg->readInt();
			string des1 = msg->readString();
			BaiCaoScreen::getInstance()->OnScores(who1, des1);
		}
		break;
		case IwinProtocol::PLAYING:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			ubyte interval = 0;
			ubyte numPlayer = 0;
			int cards[6][4];
			if (GameController::gameId == GameType::GAMEID_BAICAO)
			{
				interval = msg->readByte();// doc ra 0, doc cho co
				numPlayer = msg->readByte();
				for (int i = 0; i < numPlayer; i++)
				{
					cards[i][0] = msg->readInt();
					for (int j = 1; j < 4; j++)
					{
						cards[i][j] = (int)msg->readByte();
					}
				}
			}
			else
			{
				//BaiCaoScreen::getInstance()->totalMoney = msg->readInt();
				int i = 0;
				while (!msg->isEndRead())
				{
					cards[i][0] = msg->readInt();
					for (int j = 1; j < 4; j++)
					{
						cards[i][j] = (int)msg->readByte();
					}
					i++;
				}
				numPlayer = i;
			}
			if (BaiCaoScreen::getInstance()->GetBoardID() == boardID
				&& BaiCaoScreen::getInstance()->GetRoomID() == roomID)
			{
				BaiCaoScreen::getInstance()->OnPlaying(interval, numPlayer, cards);
			}
		}
		break;
		case IwinProtocol::START:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			ubyte interval1 = msg->readByte();
			if (GameController::gameId == GameType::GAMEID_BAICAORUA)
			{
				BaiCaoScreen::getInstance()->totalMoney = msg->readInt();
			}
			vector<ubyte> myCards(3, 0);
			for (int i = 0; i < 3; i++)
			{
				myCards[i] = msg->readByte();
			}

			if (BaiCaoScreen::getInstance()->GetBoardID() == boardID
				&& BaiCaoScreen::getInstance()->GetRoomID() == roomID)
			{
				BaiCaoScreen::getInstance()->OnStartGame(myCards, interval1);
			}
		}
		break;
		case IwinProtocol::MOVE:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			id = msg->readInt();// ID cua thang vua moi lat bai
			std::vector<char> cardID;
			for (int i = 0; i < 3; i++)
			{
				cardID.push_back(msg->readByte());
			}
			if (BaiCaoScreen::getInstance()->GetBoardID() == boardID
				&& BaiCaoScreen::getInstance()->GetRoomID() == roomID)
			{
				BaiCaoScreen::getInstance()->OnFlipCard(id, cardID);
			}
		}
		break;
		case IwinProtocol::RETURN_GAME:
		{
			int nPlayer = msg->readInt();
			bool isPlaying = msg->readBool();
			int limitTime = 0;
			int remainTime = 0;
			int totalMoney = 0;
			ubyte myCards[3];
			if (isPlaying)
			{
				limitTime = msg->readInt();
				remainTime = msg->readInt();
				if (GameController::gameId == GameType::GAMEID_BAICAORUA)
				{
					totalMoney = msg->readInt();
				}
				for (int i = 0; i < 3; i++)
				{
					myCards[i] = msg->readByte();
				}
			}
			std::vector<std::vector<int>> playerCards;
			std::vector<int> playerBetMoneys;
			for (int i = 0; i < nPlayer; i++)
			{
				std::vector<int> lsP;
				lsP.push_back(msg->readInt()); // user id
				if (lsP[0] == -1)
					continue;
				if (GameController::gameId == GameType::GAMEID_BAICAO)
				{
					playerBetMoneys.push_back(msg->readInt());
				}
				if (!isPlaying) continue;
				for (int j = 1; j < 4; j++)
				{
					ubyte c = msg->readByte();
					lsP.push_back(c);
				}
				playerCards.push_back(lsP);
			}

			BaiCaoScreen::getInstance()->OnReconnect(
				limitTime,
				remainTime,
				playerCards,
				playerBetMoneys,
				nPlayer,
				myCards,
				totalMoney,
				isPlaying);
			break;
		}
		case IwinProtocol::GET_TIME_BAR:
		{
			byte timeStart = msg->readByte();
			BaiCaoScreen::getInstance()->onGetTimeLeft(timeStart);
			break;
		}
		case IwinProtocol::QUICK_PLAY:
		{
			roomID = msg->readByte();
			boardID = msg->readByte();
			int userId = msg->readInt();
			bool isQuickPlay = msg->readBool();
			if (GameController::currentGameScreen->GetRoomID() == roomID
				&& GameController::currentGameScreen->GetBoardID() == boardID) 
			{
				BaiCaoScreen::getInstance()->onSomeOneQuickPlay(userId, isQuickPlay);
			}
			break;
		}

		}
	}
	catch (exception e)
	{
		//Log.log(this.ToString(), e.ToString());
	}
}


