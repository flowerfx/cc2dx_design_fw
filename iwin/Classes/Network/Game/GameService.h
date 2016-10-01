#ifndef __GAME_SERVICE_H__
#define __GAME_SERVICE_H__

#include "Common/Common.h"
class GameService
{
public:
	static void getBetRange();
	static void sendSingleCommand(int commandId);
	static void requestRoomList();
	static void joinAnyBoard();
	static void chatToBoard(ubyte roomID, ubyte boardID, std::string text);
	static void joinBoard(ubyte roomID, ubyte boardID, std::string pass);
	static void joinBoard_Normal(ubyte roomID, ubyte boardID, std::string pass);
	static void leaveBoard(ubyte roomID, ubyte boardID);
	static void ready(ubyte roomID, ubyte boardID, bool isReady);
	static void quickPlay(ubyte roomID, ubyte boardID);
	static void setMoney(ubyte roomID, ubyte boardID, int money);
	static void setPassword(ubyte roomID, ubyte boardID, std::string pass);
	static void kick(ubyte roomID, ubyte boardID, int kickID);
	static void startGame(ubyte roomID, ubyte boardID);
	static void startGame(ubyte roomID, ubyte boardID, ubyte timeLimit);
	static void	requestBoardList(ubyte id);
	static void openEventItem(int itemID);
	static void requestInvite(int userID);
	static void getList(int type);
	static void requestQuestList();
	static void acceptCancelQuest(int questId, bool isCancel);
	static void giveItem(ubyte roomid, ubyte controllerId, int itemID, std::vector<int> toUserIDs);
	static void requestListGift(int categoryId, ubyte page);
	static void buzz();
	static void requestBgImage(int id);
	static void viewBoardInfoOf(ubyte roomID, ubyte boardID);
	static void sendBuyLotto(int number, int money);
	static void getGift(int categoryId, ubyte gender, ubyte type);
	static void sendDeny_Invited(int userID);
	static void searchInWaittingList(std::string username);
	static void setMaxPlayer(ubyte roomID, ubyte boardID, int max);
	static void betBauCua(s16 command, ubyte roomID, ubyte boardID, ubyte subCommand, int itemID, int money);
	static void setCommandinGametoServer(s16 command, ubyte roomID, ubyte boardID, int mo);
	static void latBaiBaiCao(ubyte roomID, ubyte boardID, std::vector<char> cardsID);
	static void requestWaitingListInGame();
	static void playerToViewer(ubyte roomID, ubyte boardID);
	static void requestAndAnswerLoseAndDraw(s16 command, ubyte roomID, ubyte boardID, bool accept);
	static void move(ubyte roomID, ubyte boardID, ubyte x, ubyte y);
	static void move(ubyte roomID, ubyte boardID, ubyte actionTuxi);
	static void requestSimpleCommand(s16 command, ubyte roomID, ubyte boardID);
};

#endif //__GAME_SERVICE_H__