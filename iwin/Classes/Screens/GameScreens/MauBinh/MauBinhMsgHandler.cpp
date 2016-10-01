#include "MauBinhMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "MauBinhScreen.h"
#include "Result.h"
MauBinhMsgHandler* MauBinhMsgHandler::_instance = nullptr;
MauBinhMsgHandler::MauBinhMsgHandler()
{
	_instance = this;
}

MauBinhMsgHandler::~MauBinhMsgHandler()
{

}

int MauBinhMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void MauBinhMsgHandler::onMessage(iwincore::Message* msg)
{

	switch (msg->getCommandId())
	{
		case 47: // command game binh
		{
			ubyte subCommand = msg->readByte();
			switch (subCommand)
			{
				case 20:  // Start game
				{
					ubyte roomID = msg->readByte();
					ubyte boardID = msg->readByte();
					int interval = msg->readByte();
					ubyte myCards[13];
					for (int i = 0; i < 13; i++) {
						myCards[i] = msg->readByte();
					}
					char mauBinhType = (char)msg->readByte();
					GetMauBinhScreen()->onStartGame(roomID, boardID, myCards,
						interval, mauBinhType);
				}
				break;
				case 10:
				{
					int time = msg->readByte();
					int type = msg->readByte();
					GetMauBinhScreen()->onPlayingTime(time, type);
				}
				break;
				case 15: {
					int limitTime = msg->readByte();
					int restTime = msg->readByte();
				}
				break;
				case 25:// Auto binh.
				{
					ubyte cardIDs[13];
					for (int i = 0; i < 13; i++) {
						cardIDs[i] = msg->readByte();
					}
					GetMauBinhScreen()->onAutoBinh(cardIDs);
				}
				break;
				case 30:
				{
					GetMauBinhScreen()->onFinishBinh(msg->readInt());
				}
				break;
				case 40:
				{
					std::vector < Result*> results;
					Result* mR = new Result();
					mR->userID = msg->readInt();
					mR->winMoney = msg->readInt();
					mR->winChi = msg->readInt();
					mR->mauBinhType = (char)msg->readByte();
					results.push_back(mR);
					bool available = true;
					while (available) {
					
						Result* r = new Result();
						r->userID = msg->readInt();
						r->winMoney = msg->readInt();
						r->winChi = msg->readInt();
						for (int i = 0; i < 13; i++) {
							r->cardIds[i] = msg->readByte();
						}
						r->mauBinhType = (char)msg->readByte();
						r->winChiMauBinh = (char)msg->readByte();
						r->winChi01 = (char)msg->readByte() * -1;
						r->winChi02 = (char)msg->readByte() * -1;
						r->winChi03 = (char)msg->readByte() * -1;
						r->multiK = (char)msg->readByte();
						r->winChiAce = (char)msg->readByte();
						mR->winChiAce += r->winChiAce;
						results.push_back(r);
						if (msg->isEndRead()) available = false;
						
					}
					GetMauBinhScreen()->onResult(results);
				}
				break;
				case 51:
				{
					int userId = msg->readInt();
					ubyte cardIDs[13];
					int autoBinhBinhType = msg->readInt();
					//BinhScreen.setAutoBinhType(autoBinhBinhType);
					for (int i = 0; i < 13; i++)
					{
						cardIDs[i] = msg->readByte();
					}
					GetMauBinhScreen()->onAutoBinh(cardIDs);
					GetMauBinhScreen()->onFinishBinh(userId);
				}
				break;
				case 52:
				{
					std::string textSapHame = msg->readString();
					GetMauBinhScreen()->onTextSapHam(textSapHame);
				}
				break;
				default:
					CCLOG("No handler for subcommand game Mau Binh %d", subCommand);

			}
		}
		break;
		case IwinProtocol::GET_TIME_BAR:
		{
			ubyte timeStart = msg->readByte();
			GetMauBinhScreen()->OnShowCountDown(timeStart, .0f);
		}
		break;
		case IwinProtocol::SET_MONEY:
		{
			int roomId = msg->readByte();
			int boardId = msg->readByte();
			int money = msg->readInt();
			if (GetMauBinhScreen()->GetRoomID() == roomId 
				&& GetMauBinhScreen()->GetBoardID() == boardId)
			{
				GetMauBinhScreen()->setBetMoney(money);
			}
		}
		break;
		case IwinProtocol::RETURN_GAME:
		{
			// -1:Dang xem, 0 dang choi game.
			byte gameStatus = msg->readByte();
			if (gameStatus == -1) {
				GetMauBinhScreen()
					->setBoardInfo(msg->readByte(), msg->readByte());
			}
			else {
				byte limitTime = 0;
				byte restTime = 0;
				char maubinhType = -1;
				VecByte cardIDs;
				for (int i = 0; i < 13; i++) {
					cardIDs.push_back(msg->readByte());
				}
				limitTime = msg->readByte();
				restTime = msg->readByte();
				maubinhType = msg->readByte();
				GetMauBinhScreen()->OnReconnect(gameStatus, cardIDs, limitTime,
					restTime, maubinhType);
			}
		}
		break;
	}
}