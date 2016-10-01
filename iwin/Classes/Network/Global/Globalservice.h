#include "../Core/Message.h"
#include "../Core/MobileClient.h"
#include "../Core/IMessagehandler.h"
#include "../Core/JsonMessage.h"
#include "Network/JsonObject/cashoutruby/PaymentInfo.h"

#pragma once
class GlobalService
{
private:
	static MobileClient* _coreservice;
public:

	static bool isReconnect;


	static std::string TAG ;
	static std::function<void(void)> actionConnectOk ;

	static void connect(const char* ip, const char* port, const std::function<void()> & result_callback);
	static void onSendClientInfo();
	static void sendString(std::string text);
	static void sendString(s16 comand,std::string data);
	static void sendMessage(iwincore::Message* msg);
	static void sendSingleCommand(s16 idCommand);
	static void login(std::string username, std::string pass);
    
    static void getInfo(int subId);

	static void sendJSONString(JsonMessage* jsonMsg);
	/**
	* Lay pham vi tien cuoc ma user co the dat.
	*/
	static void getBetRange();

	/**
	* This function send only command ID.
	*
	* @param commandId
	*/
	static void sendSingleCommand(int commandId);

	static void requestRoomList();
	static void requestProfileOf(int id);
	static void UpdateProfile(User * user);
	static void UpdateStatus(std::string status);
	static void joinAnyBoard();

	static void getAllUserAchievement(int id_user);
	static void getUserAchievementOfGameID(int id_user, int game_id);

	static void chatToBoard(ubyte roomID, ubyte boardID, std::string text);

	static void joinBoard(ubyte roomID, ubyte boardID, std::string pass);

	/**
	* dung khi dang ngoi xem chuyen sang ngoi choi (cho cac game co cho ngoi
	* xem) co the xai nhu binh thuong co cac game khong co ngoi xem
	*
	* @param roomID
	* @param boardID
	* @param pass
	*/
	static void joinBoard_Normal(ubyte roomID, ubyte boardID, std::string pass);
	static void leaveBoard(ubyte roomID, ubyte boardID);
	static void ready(ubyte roomID, ubyte boardID, bool isReady);
	static void quickPlay(ubyte roomID, ubyte boardID);
	static void confirmLeaveGame(ubyte roomID, ubyte boardID, ubyte isConfirm);
	/**
	* Set money trong cac game ma tien cuoc chung.
	*
	* @param roomID
	* @param boardID
	* @param money
	*/
	static void setMoney(ubyte roomID, ubyte boardID, int money);
	/**
	* Cai dat password cho ban choi.
	*
	* @param roomID
	* @param boardID
	* @param pass
	*/
	static void setPassword(ubyte roomID, ubyte boardID, std::string pass);

	/**
	* Kick player trong ban choi.
	*
	* @param roomID
	* @param boardID
	* @param kickID
	*/
	static void kick(ubyte roomID, ubyte boardID, int kickID);
	static void startGame(ubyte roomID, ubyte boardID);
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
	static void startGame(ubyte roomID, ubyte boardID, ubyte timeLimit);

	static void requestBoardList(ubyte id);
	/**
	* Thuc hien mo vat pham su kien.
	*
	* @param itemID
	*            :ID cua item muon open.
	*/
	static void openEventItem(int itemID);

	/**
	* send command request a player join board.
	*/
	static void requestInvite(int userID);

	/**
	* Yeu cau server tra ve mot danh sach loai type.
	*
	* @param type
	*            : Loai muon tra 1 : Danh sach TOP/RANK. 2 : Danh sach phong
	*            cho 3 : Danh sách tỉnh thành.
	*/
	static void getList(int type);

	static void requestQuestList();

	static void acceptCancelQuest(int questId, bool isCancel);

	static void giveItem(ubyte roomid, ubyte controllerid, int itemID,
		std::vector<int> toUserIDs);
	static void requestListGift(int categoryId, ubyte page);
	static void buzz();
	static void requestBgImage(int id);

	/**
	* * Yêu cầu server xem thông tin của một bàn chơi.
	*
	* @param boardID
	*            - ID của bàn chơi muốn xem.
	*/
	static void viewBoardInfoOf(ubyte roomID, ubyte boardID);

	/**
	* Game lotto
	*/
	static void sendBuyLotto(int number, int money);
	static void getGift(int categoryId, ubyte gender, ubyte type);

	/**
	* tu choi loi moi khi vao game
	*/
	static void sendDeny_Invited(int userID);

	/**
	* Lấy 1 danh sách các user đang nằm trong phòng ch�? có tên đúng và gần
	* đúng với tên truy�?n vào.
	*
	* @param username
	*/
	static void searchInWaittingList(std::string username);

	/**
	* Set max player in any game with roomID and boardID
	*
	* @param roomID
	* @param boardID
	* @param max
	*/
	static void setMaxPlayer(ubyte roomID, ubyte boardID, int max);

	/**
	* commnad cho game bau cua
	*
	* @param command
	*            WANT_DRAW số 24 dùng cho ti�?n cược : byte sub : + 0 : đặt
	*            cược [ int item, int money ] (money cuối cùng sau khi nhân lần
	*            cược) + 1: hủy cược [ int item]
	* @param roomID
	* @param boardID
	* @param subCommand
	*            0 la bet 1 la delbet 2 setbet
	* @param itemID
	*            = -1 la ko gui byte nay len
	* @param money
	*            = -1 la ko gui byte nay len
	*/
	static void betBauCua(s16 command, ubyte roomID, ubyte boardID,
		ubyte subCommand, int itemID, int money);

	/**
	* phuong thuc gui msg don gian cho cac game dung chung
	*
	* @param command
	* @param roomID
	* @param boardID
	* @param mo
	*/
	static void setCommandinGametoServer(s16 command, ubyte roomID,
		ubyte boardID, int mo);

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
	static void latBaiBaiCao(ubyte roomID, ubyte boardID, std::vector<char> cardsID);
	static void requestWaitingListInGame();
	static void playerToViewer(ubyte roomID, ubyte boardID);
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
	static void requestAndAnswerLoseAndDraw(s16 command, ubyte roomID,
		ubyte boardID, bool accept);

	/**
	* dung cho caro va co vua
	*
	* @param roomID
	* @param boardID
	* @param x
	* @param y
	*/
	static void move(ubyte roomID, ubyte boardID, ubyte x, ubyte y);
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
	static void requestSimpleCommand(s16 command, ubyte roomID,
		ubyte boardID);

	static void sendHeartBeat();
	static void getHeartBeatConfiguration();
	static void getNewGameList();
	static void getCountQuestList();


	/**
	* lấy danh sách bàn chơi (chủ bàn)
	*
	* @param betMoney
	*/
	static void requestOwnerBoardList(int betMoney);

	/**
	* Set game type.
	*
	* @param gameType
	* @param onSetGameTypeOK
	*            Call khi set game type ok.
	*/
	static void setGameType(int gameType, std::function<void()> _onSetGameTypeOK);


	static void requestRegister(std::string username, std::string password);

	static void requestCreateBoard(int subCommand, int money, ubyte numPlayer, std::string password);

	static void getSMSInfoForgotPassword(std::string username);
	static void loginWithFacebookToken(std::string token, std::string version);

	static void CheckUserName(std::string user_name);
	static void getCaptchaFirstTime();
	static void getCaptchaRefresh(std::string id);
	static void sendCheckCaptcha(std::string id, std::string answer);
	static void sendChargeMoney(ubyte charge_by_id, ubyte charge_by_subid, std::string serial, std::string pin, std::string moneyType);

	static void requestPaymentInfo(const char* unit);
	static void GetQuestCategoryList();
	static void GetQuestByCategoryID(int id);
	//static void SendJSONString(std::string json_str);
	static void getLobbyList(ubyte gameID, ubyte lobbyType);
	static void doPlayNowLobby(int moneyType, int money);
	static void getBoardFromLobby(int moneyType, int money);
	static void sendLobbyChat(ubyte gameId, int userId, std::string message);
	static void getGlobalChatList();
	static void sendStaticInviviteToPlayGame(bool canInvite);
	//close socket
	static void CloseSocket();


	/**
	* Lay danh sach cao thu
	*
	* @param page
	* @param gameId
	*/
	static void requestStrongest(int page, ubyte gameId);

	/**
	* lay danh sach dai gia
	*
	* @param page
	*/
	static void requestRichestWin(int page);

	static void requestRichestRuby(int page);

	//change password
	static void changePassword(std::string old_pass, std::string new_pass);

	static void getForgotPassword(ubyte type, std::string userName);

	//avatar service
	static void getItemImg(int itemId);
	//get avatar shop from server
	static void requestAvatarShop();
	static void requestAvatar(int avatar);
	/**
	* request mua avatar
	*
	* @param id
	*/
	static void requestBuyAvatar(int id);

	static void getAvatarUsing(int userId, ubyte gender);

	/**
	* gui du lieu tam hinh chup len server lam avatar.
	*
	* @param imgData
	*/
	static void sendCaptureAvatar(char* imgData, size_t leng);

	/**
	* Lay hinh avatar voi user id.
	*/
	static void getAvatarByUserID(int userid);
	/**
	* request lay ds avatar de phan trang
	*/
	static void requestAvatarPage(int page);
	/**
	* request lay ds avatar de phan trang theo gioi tinh @type 1 la nam, 2 nu,
	* 3 doc
	*
	* @param page
	*/
	static void requestAvatarPage(int page, int type);

	/**
	* Láº¥y táº¥t cáº£ cÃ¡c avatar mÃ  user Ä‘ang sá»Ÿ há»¯u.
	*/
	static void getMyAvatars();

	/**
	* Thuc hien doi avatar cho user.
	*
	* @param avatarID
	*/
	static void changeMyAvatar(int avatarID);

	/**
	* Xoa 1 avatar cua user dang co.
	*
	* @param avatarID
	*/
	static void deleteMyAvatar(int avatarID);

	/**
	* Mua vật phẩm hoặc avatar cho bạn
	*/
	static void buyForFriend(ubyte type, int itemID, int friendID, std::string friendName);

	/**
	* gui request lay avatar nhan ve o Global message handler
	*/
	static void requestShopAvatar(int page);

	/**
	* Lay danh sach category
	*/
	static void getAvatarCategory();

	/**
	* Lay danh sach avatar theo id
	*
	* @param catId
	*            id cua category can lay avatar
	*/
	static void getAvatarOfCategory(int catId);

	/**
	* Thuc hien mua 1 avatar
	*
	* @param avatar
	*/
	static void buyAvatar(int id, int64_t money);

	static void getIsEnableRuby();

	static void requestListWait();
	static void requestListRubyToVND();

	static void requestCharge(PaymentInfo* payment);
	static void requestListHistoryCashOut(int page);
	static void requestCancleCashout(int id);

	/**
	* Gửi mã xác nhận lên cho server
	*/
	static void sendConfirmSMSCode(std::string confirmation_code);
	/**
	* Chứng thực số điện thoại
	* type = 1 : Verify phone
	* @param value : Phone to verify
	*/
	static void requestVerifyPhone(std::string value);

	/**
	* Chứng thực Email
	* type = 0 : Verify email
	* @param value : email to verify
	*/
	static void requestVerifyEmail(std::string value);


	/**
	* nạp để mua item trong shop item
	*
	* @param shopId
	* @param smsContent_send_to_server
	* @param smsTo
	*/
	static void chargeSMSWithShop(int shopId, std::string smsContent_send_to_server, std::string smsTo);

	static void transferMoney(std::string transferNick, int transferQuantity);

	static void transferMoneyWithType(std::string transferNick, int transferQuantity, int type);
	/*
	* friend param 
	
	*/

	/**
	* Lay danh sach ban be cua chinh minh. Lay danh sach ban be tu 2.4.3
	*/
	static void requestFriendList();

	/**
	* tìm nick
	*
	* @param text
	*/
	static void searchNick(std::string nick);

	static void addFriend(int id, std::string name);
	static void requestRichFriendList();
	static void requestStrongFriendList(int gameId);
	static void friendAcceptRequest(int id);
	static void friendDenyRequest(int id);

	/**
	* lay danh sach ma minh da gui loi moi ket ban
	*/
	static void requestListInviteFriend();
	/**
	* lay danh sach moi minh lam ban
	*/
	static void requestListWaitingFriend();
	static void deleteHistory(int historyId);
	/**
	* rut bai (dung cho xi zach)
	*
	* @param command
	* @param roomId
	* @param boardId
	* @param type
	*/
	static void Fire(s16 command, ubyte roomId, ubyte boardId, ubyte type);
	static void sendCommandinGametoServer(const s16 ADD_CARD, int _roomID, int _boardID, int idUser);

	/*
	*
	* tai xiu global service
	*/

	static void getTaiXiuInfo(std::string name);
	static void taiXiuBet(std::string name, s64 value, ubyte type);
	static void getTaiXiuLeaderBoard();
	static void getTaiXiuMyHistoryList();
	
	//
	static void sendChargeMoneyForInApp(ubyte charge_by_id, std::string token, std::string moneyType);

	static void sendScreenIdToServer(int idScreen);
	static void GetLuckyCircleInfo(int bonusEventId, int screenType);
	static void RequestTurningLuckyCircle(int circleType);
};

