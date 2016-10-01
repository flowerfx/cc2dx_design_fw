#include "CoTuongMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "CoTuongScreen.h"
#include "CoTuongPlayer.h"

CoTuongMsgHandler* CoTuongMsgHandler::_instance = nullptr;
CoTuongMsgHandler::CoTuongMsgHandler()
{
	_instance = this;
}

CoTuongMsgHandler::~CoTuongMsgHandler()
{

}

int CoTuongMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void CoTuongMsgHandler::onMessage(iwincore::Message* msg)
{

	switch (msg->getCommandId())
	{
	case IwinProtocol::CO_TUONG_COMMAND: // command game binh
	case IwinProtocol::CO_UP_COMMAND: // command game binh
	{
		byte subCommand = msg->readByte();
		switch (subCommand) {
		case 15: // START:
		{
			byte roomID6 = msg->readByte();
			byte boardID6 = msg->readByte();
			int interval = msg->readInt();
			int whoMoveFirst = msg->readInt();
			int ownerQquotaTime = msg->readInt();
			int quotaTime = msg->readInt();
			if (getCoTuongScreen()->GetBoardID() == boardID6 && getCoTuongScreen()->GetRoomID() == roomID6) {
				//for (int i = 0; i < getCoTuongScreen().players.size; i++) {
				//	Player player = coTuongScreen().players.get(i);
				//	if (player.IDDB == coTuongScreen().ownerID) {
				//		player.quotaTime = ownerQquotaTime;
				//	}
				//	else {
				//		player.quotaTime = quotaTime;
				//	}
				//	coTuongScreen().setTimeQuota(player.IDDB, player.quotaTime);
				//}
				getCoTuongScreen()->onStartGame(whoMoveFirst, interval);
			}
			break;
		}
		case 12:// het 3 lan di lai
		{
			//coTuongScreen().onRemoveButton(msg.readByte());
		}
		break;
		case 25: // MOVE
		{
			int whoMove = msg->readInt();
			byte xS = msg->readByte();
			byte yS = msg->readByte();
			byte xD = msg->readByte();
			byte yD = msg->readByte();
			int quotaTime = msg->readInt();
			int nextMove = msg->readInt();
			byte newPieceType = msg->readByte();// co tuong thi la -1
			Player* player = getCoTuongScreen()->getPlayer(whoMove);
			if (player != nullptr) {
				((CoTuongPlayer*)(player))->setQuotaTime(quotaTime);
				getCoTuongScreen()->setTimeQuota(player->IDDB, quotaTime);
			}

			CCLOG("%d %d %d %d %d %d %d", whoMove, xS, yS, xD, yD, nextMove, newPieceType);
			getCoTuongScreen()->onMove(whoMove, xS, yS, xD, yD, nextMove, newPieceType);
			break;
		}
		case 26:// UNDO
		{
			getCoTuongScreen()->onOpponentWantUndo();
			break;
		}
		case 30: // WANT_DRAW:
		{
			getCoTuongScreen()->onOpponentWantDraw();
		}
		break;
		case 35:
		{
			getCoTuongScreen()->onOpponentWantLose();
		}
		break;
		}
		}
		break;
		case IwinProtocol::GET_TIME_BAR:
		{
			ubyte timeStart = msg->readByte();
			getCoTuongScreen()->OnShowCountDown(timeStart, .0f);
		}
		break;
		case IwinProtocol::SET_MONEY:
		{
			int roomId = msg->readByte();
			int boardId = msg->readByte();
			int money = msg->readInt();
			if (getCoTuongScreen()->GetRoomID() == roomId 
				&& getCoTuongScreen()->GetBoardID() == boardId)
			{
				getCoTuongScreen()->setBetMoney(money);
			}
		}
		break;
		case IwinProtocol::RETURN_GAME:
		{
			
		}
		break;
	}
}