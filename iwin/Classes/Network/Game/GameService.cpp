#include "GameService.h"
#include "Network/Core/JsonMessage.h"
#include "Network/IwinCommand.h"
#include "Network/IwinProtocol.h"
#include "Network/Global/Globalservice.h"
#include "Common/GameController.h"

/**
* Lay pham vi tien cuoc ma user co the dat.
*/
void GameService::getBetRange() {
	JsonMessage* msg = new JsonMessage(IwinCommand::GET_MIN_MAX_BOARD);
	GlobalService::sendString(2800, msg->toJson());
}

/**
* This function send only command ID.
*
* @param commandId
*/
void GameService::sendSingleCommand(int commandId) {
	iwincore::Message* m = new iwincore::Message(commandId);
	GlobalService::sendMessage(m);
}

void GameService::requestRoomList() {
	sendSingleCommand(IwinProtocol::ROOM_LIST);
}

void GameService::joinAnyBoard() {
	sendSingleCommand((GameController::getCurrentGameType() != GameType::GAMEID_POKER
		&& GameController::getCurrentGameType() != GameType::GAMEID_POKER_HK
		&& GameController::getCurrentGameType() != GameType::GAMEID_CO_UP
		&& GameController::getCurrentGameType() != GameType::GAMEID_GIAI_DAU_CO_UP
		&& GameController::getCurrentGameType() != GameType::GAMEID_CHESS && GameController
		::getCurrentGameType() != GameType::GAMEID_LIENG) ? IwinProtocol::JOIN_ANY_BOARD
		: IwinProtocol::AUTO_JOIN_VIEWER);
}

void GameService::chatToBoard(ubyte roomID, ubyte boardID, std::string text) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ON_CHAT_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(text);
	GlobalService::sendMessage(m);
}

void GameService::joinBoard(ubyte roomID, ubyte boardID, std::string pass) {
	iwincore::Message* m = new iwincore::Message(
		(GameController::getCurrentGameType() != GameType::GAMEID_CHESS
			&& GameController::getCurrentGameType() != GameType::GAMEID_CO_UP
			&& GameController::getCurrentGameType() != GameType::GAMEID_GIAI_DAU_CO_UP
			&& GameController::getCurrentGameType() != GameType::GAMEID_POKER
			&& GameController::getCurrentGameType() != GameType::GAMEID_POKER_HK && GameController
			::getCurrentGameType() != GameType::GAMEID_LIENG) ? IwinProtocol::JOIN_BOARD
		: IwinProtocol::JOIN_VIEWER);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	GlobalService::sendMessage(m);
	//Log.log(TAG, Util.getLogString("joinBoard", "roomID", "boardID",
		//"pass", roomID, boardID, pass));
}

/**
* dung khi dang ngoi xem chuyen sang ngoi choi (cho cac game co cho ngoi
* xem) co the xai nhu binh thuong co cac game khong co ngoi xem
*
* @param roomID
* @param boardID
* @param pass
*/
void GameService::joinBoard_Normal(ubyte roomID, ubyte boardID, std::string pass) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::JOIN_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	GlobalService::sendMessage(m);
}

void GameService::leaveBoard(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::LEAVE_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}

void GameService::ready(ubyte roomID, ubyte boardID, bool isReady) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::READY);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putBool(isReady);
	GlobalService::sendMessage(m);
}
void GameService::quickPlay(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::QUICK_PLAY);
	m->putByte(roomID);
	m->putByte(boardID);
	//m.putInt(userId);
	GlobalService::sendMessage(m);
}
/**
* Set money trong cac game ma tien cuoc chung.
*
* @param roomID
* @param boardID
* @param money
*/
void GameService::setMoney(ubyte roomID, ubyte boardID, int money) {
	// System.out.println("Set money");
	//Gdx.app.log("Game Screen", "Set money");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_MONEY);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(money);
	GlobalService::sendMessage(m);
}

/**
* Cai dat password cho ban choi.
*
* @param roomID
* @param boardID
* @param pass
*/
void GameService::setPassword(ubyte roomID, ubyte boardID, std::string pass) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_PASS);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	GlobalService::sendMessage(m);
}

/**
* Kick player trong ban choi.
*
* @param roomID
* @param boardID
* @param kickID
*/
void GameService::kick(ubyte roomID, ubyte boardID, int kickID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::KICK);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(kickID);
	GlobalService::sendMessage(m);
}

void GameService::startGame(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::START);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}

/**
* Dung cho chuc nang moi. khi start game them config time limit
*
* @param roomID
*            current roomID
* @param boardID
*            current boardID
* @param timeLimit
*            timeSetup from board owner
*/
void GameService::startGame(ubyte roomID, ubyte boardID, ubyte timeLimit) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::START);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(timeLimit);
	GlobalService::sendMessage(m);
}

void GameService::requestBoardList(ubyte id) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::BOARD_LIST);
	m->putByte(id);
	GlobalService::sendMessage(m);
}

/**
* Thuc hien mo vat pham su kien.
*
* @param itemID
*            :ID cua item muon open.
*/
void GameService::openEventItem(int itemID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(17);
	m->putInt(itemID);
	GlobalService::sendMessage(m);
}

/**
* send command request a player join board.
*/
void GameService::requestInvite(int userID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::INVITE_PLAYER);
	m->putInt(userID);
	GlobalService::sendMessage(m);
	// m.cleanup();
}

/**
* Yeu cau server tra ve mot danh sach loai type.
*
* @param type
*            : Loai muon tra 1 : Danh sach TOP/RANK. 2 : Danh sach phong
*            cho 3 : Danh sách tỉnh thành.
*/
void GameService::getList(int type) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_LIST);
	m->putByte(type);
	GlobalService::sendMessage(m);
	// m.cleanup();
}

void GameService::requestQuestList() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::QUEST);
	m->putByte(0);
	GlobalService::sendMessage(m);
}

void GameService::acceptCancelQuest(int questId, bool isCancel) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::QUEST);
	m->putByte(isCancel ? 2 : 1);
	m->putInt(questId);
	GlobalService::sendMessage(m);
}

void GameService::giveItem(ubyte roomid, ubyte controllerid, int itemID, std::vector<int> toUserIDs) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(IwinProtocol::ITEMS_GIVE_ITEM);
	m->putByte(roomid);
	m->putByte(controllerid);
	m->putInt(itemID);
	for (size_t i = 0; i < toUserIDs.size(); i++) {
		if (toUserIDs.at(i) >= 0) {
			m->putInt(toUserIDs.at(i));
		}
	}
	GlobalService::sendMessage(m);
}

void GameService::requestListGift(int categoryId, ubyte page) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(15);
	m->putInt(categoryId);
	m->putByte(page);
	GlobalService::sendMessage(m);
}

void GameService::buzz() {
	sendSingleCommand(IwinProtocol::BUZZ);
}

void GameService::requestBgImage(int id) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_IMAGE);
	m->putByte(5); // Item Image
	m->putInt(id);
	GlobalService::sendMessage(m);
}

/**
* * Yêu cầu server xem thông tin của một bàn chơi.
*
* @param boardID
*            - ID của bàn chơi muốn xem.
*/
void GameService::viewBoardInfoOf(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::BOARD_INFO);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}

/**
* Game lotto
*/
void GameService::sendBuyLotto(int number, int money) {
	iwincore::Message* m = new iwincore::Message(1403);
	m->putInt(number);
	m->putInt(money);
	GlobalService::sendMessage(m);
}

void GameService::getGift(int categoryId, ubyte gender, ubyte type) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(IwinProtocol::ITEMS_LIST_CATEGORIES);
	m->putInt(categoryId);
	m->putByte(gender);
	m->putByte(type);
	GlobalService::sendMessage(m);
}

/**
* tu choi loi moi khi vao game
*/
void GameService::sendDeny_Invited(int userID) {
	iwincore::Message* m = new iwincore::Message(1302);
	m->putInt(userID);
	GlobalService::sendMessage(m);
}

/**
* Lấy 1 danh sách các user đang nằm trong phòng ch�? có tên đúng và gần
* đúng với tên truy�?n vào.
*
* @param username
*/
void GameService::searchInWaittingList(std::string username) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SEARCH_WAITING_LIST);
	m->putString(username);
	GlobalService::sendMessage(m);
}

/**
* Set max player in any game with roomID and boardID
*
* @param roomID
* @param boardID
* @param max
*/
void GameService::setMaxPlayer(ubyte roomID, ubyte boardID, int max) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_MAX_PLAYER);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(max);
	GlobalService::sendMessage(m);
}

/**
* commnad cho game bau cua
*
* @param command
*            WANT_DRAW số 24 dùng cho ti�?n cược : ubyte sub : + 0 : đặt
*            cược [ int item, int money ] (money cuối cùng sau khi nhân lần
*            cược) + 1: hủy cược [ int item]
* @param roomID
* @param boardID
* @param subCommand
*            0 la bet 1 la delbet 2 setbet
* @param itemID
*            = -1 la ko gui ubyte nay len
* @param money
*            = -1 la ko gui ubyte nay len
*/
void GameService::betBauCua(s16 command, ubyte roomID, ubyte boardID,
	ubyte subCommand, int itemID, int money) {
	iwincore::Message* m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	if (itemID != -1) {
		m->putInt(itemID);
	}
	if (money != -1) {
		m->putInt(money);
	}
	GlobalService::sendMessage(m);

}

/**
* phuong thuc gui msg don gian cho cac game dung chung
*
* @param command
* @param roomID
* @param boardID
* @param mo
*/
void GameService::setCommandinGametoServer(s16 command, ubyte roomID, ubyte boardID, int mo) {
	iwincore::Message* m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(mo);
	GlobalService::sendMessage(m);
}

/**
* command lat bai cho game Bai Cao
*
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
* @param cards
*            mang card
*/
void GameService::latBaiBaiCao(ubyte roomID, ubyte boardID, std::vector<char> cardsID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	for (int i = 0; i < 3; i++) {
		m->putByte(cardsID[i]);
	}
	GlobalService::sendMessage(m);
}

void GameService::requestWaitingListInGame() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_WAITING_LIST);
	GlobalService::sendMessage(m);
}

void GameService::playerToViewer(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::PLAYER_TO_VIEWER);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}

/**
* xin thua hoac xin hoa trong game co
*
* @param command
*            gui len server
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
* @param accept
*            yeu cau gui len true la chap nhan nguoc lai la false
*/
void GameService::requestAndAnswerLoseAndDraw(s16 command, ubyte roomID, ubyte boardID, bool accept) {
	iwincore::Message* m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putBool(accept);
	GlobalService::sendMessage(m);
}

/**
* dung cho caro va co vua
*
* @param roomID
* @param boardID
* @param x
* @param y
*/
void GameService::move(ubyte roomID, ubyte boardID, ubyte x, ubyte y) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(x);
	m->putByte(y);
	GlobalService::sendMessage(m);
}
/**
* dung cho gameTuxi
*
* @param roomID
* @param boardID
* @param actionTuxi
*/
void GameService::move(ubyte roomID, ubyte boardID, ubyte actionTuxi) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(actionTuxi);
	GlobalService::sendMessage(m);
}

/**
* chi gui command va room board
*
* @param command
*            gui command single
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
*/
void GameService::requestSimpleCommand(s16 command, ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}
