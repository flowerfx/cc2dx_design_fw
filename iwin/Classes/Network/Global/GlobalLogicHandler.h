#ifndef GLOBALLOGICHANDLER_H
#define GLOBALLOGICHANDLER_H
#include <string>
#include <functional>

#include "Network/Core/Message.h"
#include "Models/ChatMessage.h"

class GameObjectData;

namespace iwinmesage
{
	class GameList;
	class BoardOwner;
	class CaptchaInfo;
	class CaptchaResult;
	class Payment;
	class LobbyList;
	class ChatLobbyList;

	class AvatarCat;
	class AvatarCatList;
	class AvatarList;
	class BuyingAvatar;
	class RubyToVNDInfo;
	class PaymentResult;
	class PaymentHistoryList;
	class CancelResult;
	class RequestFeature;
	class LuckyCircleResultList;
	class LuckyCircleResultListItem;
}

using namespace iwinmesage;
using namespace std;
class User;
class Player;
class Achievement;
class Province;
class Item;
class Category;
class GlobalLogicHandler
{
public:
	static int GAME_TYPE;// = -1
	static bool isJoinGame;

	void autoJoinGameAfterLogin();
	void showDialogDieuHuong(RequestFeature* feature);

	void OnArchivement(char gameId, std::vector<std::string> archivements, int avaXP);
public:

	static std::function<void()>	onSetGameTypeOk;

	
	GlobalLogicHandler();
	~GlobalLogicHandler();
	void onSetClientInfoOK(std::string wellcom);
    
    void onGetInfo(int subId, std::string info);
	
	void onLoginSuccess();
	void onLoginFacebookSuccess();
	void onLoginFail(string reason);
	void onErrorMessage(string reason);

	void onReceivedGameList(GameList* gamelift);
	void onReceiveOwnerBoardList(std::vector<BoardOwner> boardOwners, int win, int lose, int draw, int leave);
	void onSetGameTypeSuccess(int gameID, bool isReconnect);

	void onShowProfile(User * user);
	void onShowProfileOfUser(User * user);

	void onGetSMSSyntax(s16 subId, std::string smsData, std::string address, std::string confirmMessage);

	void OnUserAllAchievement(int userId, std::vector<Achievement*> list);

	void onCallBackNameExist(std::string msg);

	void onDisConnect();

	void onServerMessage(const char * msg);
	void onServerInfo(const char * msg);
	void onReceiveMessageJson(std::string msg_json , std::string id_msg);
	void onReceiveCaptcha(CaptchaInfo* captcha);
	void onReceiveCaptchaResult(CaptchaResult* captcha);
	void onReceivePayment(Payment* payment);
	void onReceiveLobbyList(LobbyList* lobby_list);
	void onReceiveChatLobbyList(ChatLobbyList* chat_lobby_list);

	//void onInviteJoinBoard(int gameId, int roomId, int boardId, std::string password, std::string message, int levelroom, int idInvite, std::string userName, int idAvatar);
	void onInviteJoinBoard(iwincore::Message* msg);

	void onReceivedStrongestList(ubyte page, std::vector<Player*> list);
	void onReceivedRichestWinList(ubyte page, std::vector<Player*> list);
	void onReceivedRichestRubyList(ubyte page, std::vector<Player*> list);

	//change pass result
	void onReceivedChangePass(bool is_ok, std::string dec);

	//show dialog notice succeed or not
	void onShowDialogResult(bool is_ok, std::string dec);

	//register callback
	void onRegisterInfo(
		std::string username, 
		bool available, 
		std::string smsPrefix, 
		std::string smsTo, 
		std::string info);

	/**
	* Nhan ve danh sach avatar category
	*
	* @param avatarCatList
	*/
	void onReceiveAvatarCatList(AvatarCatList * avatarCatList);

	/**
	* Nhan ve danh sach avatar
	*
	* @param avatarList
	*/
	void onReceiveAvatarList(AvatarList* avatarList);

	/**
	* Nhan ve tu server sau khi mua avatar
	*
	* @param buyingAvatar
	*/
	void onBuyingAvatarResult(BuyingAvatar *buyingAvatar);

	void onReceiveListWait(int countRoom, int countUser, std::map<int , GameObjectData*> list);
	void onReceiveEnableRuby(bool enableRuby, bool enableCashout, std::string message);
	void onReceiveListRubyToVND(RubyToVNDInfo* info);
	void onReceivePaymentResult(PaymentResult* result);
	void onReceiveListHistory(PaymentHistoryList* list);
	void onReceiveCancelCashout(CancelResult* cancel);

	void onVerifyUser(int type_verify, bool is_success, std::string descript);
	/**
	* Nhận về thông báo sau khi gửi Confirm code lên cho server
	*
	* @param is_succ
	* @param description
	*/
	void onReceiveConfirmSMSCode(bool is_succ, std::string description);
	/**
	* nhận được số lượng quest
	*
	* @param count
	*/
	void onReceivedCountQuestList(int count);

	void onRequestFeature(RequestFeature* feature);
	/**
	* nhan tien sau khi nap
	*
	* @param newMoney
	* @param info
	*/
	void onTransferMoney(s64 newMoney, std::string info, int moneyType);


	//
	void onSearchResult(std::vector<Player*> searchList);

	void onAddFriendResult(ubyte addResult);

	void onFriendConfirmationResult(std::string msg);

	void onRequestAddFriend(std::vector<IwinMessage*> arrMessage);

	/**
	* danh sach ban be
	*
	* @param friendList
	*/
	void onFriendList(std::vector<Player*> friendList);
	void onListInvite(std::vector<Player*> list);
	void onListWaitingFriend(std::vector<Player*> list);
	void onMessageFrom(IwinMessage * m);

	void onDelFriendResult(ubyte delResult);

	void onReceivedLeaderboardFriendList(int state , std::vector<Player*> list);

	void onProvinceList(std::vector<Province*> provinceList);

	void onWattingList(std::vector<Player*> l);

	/**
	* lay avatar cua toi
	*/
	void onMyAvatars(std::vector<Item*> items);
	/**
	* lay vat pham cua toi
	*/
	void onBoughtItems(std::vector<Item*> listItem, int cateId, int totalPage, ubyte gender);

	/**
	* danh sach vat pham su kien (server cho ,co the mo ra)
	*/
	void onEventItemsList(std::vector<Item*> items);

	/**
	* nhan duoc vat pham sau khi hoan thanh 1 nhiem vu nao do chi la hinh hien
	* len choi thoi
	*
	* @param itemID
	* @param itemName
	*/
	void onEventItems(int itemID, std::string itemName);

	void onChangeMyAvatar(int avatar);

	void onReceivedGiftCategoties(std::vector<Category*> categories);

	void onReceviedGiftItems(int categoryID, int totalPage, std::vector<Item*> items);

	void sendGitfTo(int from, std::vector<int> tos, int itemID);

	void onHeartBeatResponse();
};
#endif