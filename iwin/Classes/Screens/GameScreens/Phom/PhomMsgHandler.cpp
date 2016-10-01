#include "PhomMsgHandler.h"
#include "cocos2d.h"
#include "Network/IwinProtocol.h"
#include "Common/GameController.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/GamePlayMgr.h"
#include "PhomScreen.h"
#include "PhomUtils.h"

PhomMsgHandler* PhomMsgHandler::_instance = nullptr;
PhomMsgHandler::PhomMsgHandler()
{
	_instance = this;
}

PhomMsgHandler::~PhomMsgHandler()
{

}

PhomScreen* PhomMsgHandler::GetPhomScreen()
{
	return static_cast<PhomScreen*>(GamePlay->GetCurrentGameScreen());
}

int PhomMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void PhomMsgHandler::onMessage(iwincore::Message* msg)
{
	switch (msg->getCommandId())
	{
	case IwinProtocol::SET_MONEY:
	{
		int roomId = msg->readByte();
		int boardId = msg->readByte();
		int money = msg->readInt();
		if (GetPhomScreen()->GetRoomID() == roomId
			&& GetPhomScreen()->GetBoardID() == boardId)
		{
			GetPhomScreen()->setBetMoney(money);
		}
	}
	break;
	case IwinProtocol::START:
	{
		int roomID = msg->readByte();
		int boardID = msg->readByte();
		int interval = msg->readByte();
		int currentPlayer = msg->readInt();
		int downFirst = msg->readInt();
		VecByte cards;
		while (true) {
			cards.push_back(msg->readByte());
			if (msg->isEndRead()) break;
		}
		if (GetPhomScreen()->GetBoardID() == boardID
			&& GetPhomScreen()->GetRoomID() == roomID) {
			// code test loi phom khong tao phom
			GetPhomScreen()->onStartGame(interval, cards, currentPlayer,
				downFirst);
		}
	}
	break;
	case IwinProtocol::MOVE:
	{
		int roomID = msg->readByte();
		int boardID = msg->readByte();
		int currentPlayer = msg->readInt();
		int nextPlayer = msg->readInt();
		int cardId = msg->readByte();
		int numCards = msg->readByte();
		if (GetPhomScreen()->GetBoardID() == boardID
			&& GetPhomScreen()->GetRoomID() == roomID) {
			GetPhomScreen()->onFire(currentPlayer, nextPlayer, cardId, numCards);
		}

	}
	break;
	case IwinProtocol::GET_CARD:
	{
		/*
		* rút bài
		*/
		int roomID = msg->readByte();
		int boardID = msg->readByte();
		int currentPlayer = msg->readInt();
		int cardID = msg->readByte();
		if (GetPhomScreen()->GetBoardID() == boardID
			&& GetPhomScreen()->GetRoomID() == roomID) {
			GetPhomScreen()->onGetCard(currentPlayer, cardID);
		}
	}
	break;
	case IwinProtocol::EAT_CARD:
	{
		/*
		* ăn bài
		*/
		int roomID = msg->readByte();
		int boardID = msg->readByte();
		int downFirst = msg->readInt(); // thằng hạ bài đầu tiên mới
		if (GetPhomScreen()->GetBoardID() == boardID
			&& GetPhomScreen()->GetRoomID() == roomID) {
			GetPhomScreen()->onEatCard(downFirst);
		}
	}
	break;
	case IwinProtocol::HA_PHOM:
	{
		/*
		* hạ phỏm
		*/
		int roomID = msg->readByte();
		int boardID = msg->readByte();
		/*
		* các phỏm mà thằng current đang có mỗi phỏm cách nhau -1
		*/
		int cardID;
		std::vector<Phom> phoms;
		Phom phom;
		while (true) {
			if (msg->isEndRead()) break;
			cardID = (char)msg->readByte();
			if (cardID > -1)
				phom._cards.push_back(cardID);
			else {
				CardUtils::sortByValue_CRChB(true, phom._cards);
				phoms.push_back(phom);
				phom._cards.clear();
				phom._type = -1;
			}
		}
		if (GetPhomScreen()->GetBoardID() == boardID
			&& GetPhomScreen()->GetRoomID() == roomID) {
			GetPhomScreen()->onDownPhom(phoms);
		}
	}

		break;
		case IwinProtocol::FINISH:
		{
			int roomID = msg->readByte();
			int boardID = msg->readByte();
			int currentPlayer = msg->readInt();
			int score = msg->readInt();
			int moneyPlayer = msg->readInt();
			byte numRac = msg->readByte();
			VecByte cards;
			if (numRac > 0) {
				while (true) {
					if (msg->isEndRead()) break;
					cards.push_back(msg->readByte());
				}
			}
			if (GetPhomScreen()->GetBoardID() == boardID
				&& GetPhomScreen()->GetRoomID() == roomID) {
				GetPhomScreen()->onFinish(currentPlayer, score, cards, moneyPlayer);
			}
		}
		break;
		case IwinProtocol::ADD_CARD:// gửi bài
								   /*
								   * ý nghĩa của việc gửi bài : để bớt điểm của bài rác.
								   *
								   * trả về các thằng nhận được bài ,cùng với phỏm >=4 lá tạo được
								   * khi nó nhận được lá bài mình gửi lấy về ứng với thằng nào thì
								   * thay cái phỏm 3 lá ban đầu của nó thành cái phỏm >=4 lá nhận
								   * được này
								   */
		{
			int roomID = msg->readByte();
			int boardID = msg->readByte();
			std::vector<int> userIDs;
			VecByte userAddCards;
			int currentPlayer;
			int cardID;
			std::vector<std::vector<unsigned char>> userAddPhomCards;
			while (true) {
				if (msg->isEndRead()) break;
				currentPlayer = msg->readInt(); // thằng nhận
				cardID = msg->readByte(); // lá bài gửi (hợp lệ)
				userIDs.push_back(currentPlayer);
				userAddCards.push_back(cardID);
				VecByte cards;				// bài tạo thành phỏm sau khi
											// gửi (chỉ có cái phỏm tạo
											// dc)
				while (true) {
					if (msg->isEndRead()) break;
					cards.push_back(msg->readByte());

				}
				userAddPhomCards.push_back(cards);

			}
			if (GetPhomScreen()->GetBoardID() == boardID
				&& GetPhomScreen()->GetRoomID() == roomID) {
				GetPhomScreen()->onSendCard(userIDs, userAddCards,
					userAddPhomCards);
			}
		}
			
			break;
		case IwinProtocol::SEND_LIST_ADD_CARD:
		{
			byte sizeCard = msg->readByte();
			VecByte cardsSend;
			for (int i = 0; i < sizeCard; i++) {
				cardsSend.push_back(msg->readByte());
			}
			GetPhomScreen()->OnCardCanSend(cardsSend);
		}
		break;
		case IwinProtocol::MOVE_ERROR:
		{
			/*
			* đi lỗi
			*/
			int boardID = msg->readByte();
			int roomID = msg->readByte();
			std::string info = msg->readString();
			if (GetPhomScreen()->GetBoardID() == boardID
				&& GetPhomScreen()->GetRoomID() == roomID) {
				GetPhomScreen()->onMoveError(info);
			}
		}
			break;
		case IwinProtocol::RETURN_GAME:
		{

			int interval = msg->readByte();
			int currentPlayer = msg->readInt();// thằng đang đánh
			int lastMovePlayer = msg->readInt(); // thằng đánh trước đó
			int downFirst = msg->readInt(); // thằng hạ bài đầu tiên
			int cardID = (byte)msg->readInt(); // lá bài vừa mới đánh

			byte numPlayers = msg->readByte();
			std::vector<int>userIDs;
			std::vector<std::vector<unsigned char>> userFiredCards;
			std::vector<std::vector<unsigned char>> userEatCards;
			std::vector<std::vector<Phom>> userPhomCards;
			byte len;
			VecByte myCards;

			for (int i = 0; i < numPlayers; i++) {
				userIDs.push_back(msg->readInt()); // userID

											// đọc bài đánh
				len = msg->readByte();
				VecByte cards;
				for (int j = 0; j < len; j++) {
					cards.push_back(msg->readByte());
				}
				userFiredCards.push_back(cards);

				// đọc bài ăn
				len = msg->readByte();
				cards.clear();
				for (int j = 0; j < len; j++) {
					cards.push_back(msg->readByte());
				}
				userEatCards.push_back(cards);

				// đọc bài phỏm
				len = msg->readByte();// có bao nhiêu phỏm
				byte phomLen;
				std::vector<Phom> phoms;
				Phom phom;
				for (int j = 0; j < len; j++) {
					phomLen = msg->readByte();// số lá bài trong phỏm này
					for (int k = 0; k < phomLen; k++) {
						phom._cards.push_back(msg->readByte());
					}
					VecByte byte_arr;
					phom._type = PhomUtils::getPhomTypeAlone(phom._cards, byte_arr);
					msg->readByte();// -1 đánh dấu kết thúc 1 phỏm (xài cho
								   // J2ME)
					phoms.push_back(phom);
				}
				userPhomCards.push_back(phoms);
			}

			// đọc bài của mình
			len = msg->readByte();
			for (int i = 0; i < len; i++) {
				myCards.push_back(msg->readByte());
			}
			int currentMoveTimeRemain = msg->readInt();// thời gian của lại
													  // của lượt đánh
													  // hiện tại

			GetPhomScreen()->OnReconnect(interval, currentMoveTimeRemain,
				currentPlayer, lastMovePlayer, downFirst, cardID,
				userIDs, userFiredCards, userEatCards, userPhomCards,
				myCards);
		}
			break;
		case 66:// ù ,ù khan
		{
			int roomID = msg->readByte();
			int boardID = msg->readByte();
			int currentPlayer = msg->readInt();
			// 0 la binh thuong, 1 la u khan, 2 la u den
			byte u = msg->readByte();
			/*
			* bài của nó
			*/
			VecByte cards;
			while (true) {
				if (msg->isEndRead()) break;

				cards.push_back(msg->readByte());

			}
			if (GetPhomScreen()->GetBoardID() == boardID
				&& GetPhomScreen()->GetRoomID() == roomID) {
				GetPhomScreen()->onU(currentPlayer, u, cards);
			}
		}
			break;
		case IwinProtocol::GET_TIME_BAR:
		{
			int timeStart = msg->readByte();
			GetPhomScreen()->OnShowCountDown(timeStart, .0f);
		}
		break;
		default:
			CCLOG("Phom Unhandle command: %d", msg->getCommandId());
	}
}