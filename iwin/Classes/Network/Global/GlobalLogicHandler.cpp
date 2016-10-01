#include "GlobalLogicHandler.h"
#include "Globalservice.h"
#include "Common/GameController.h"

#include "Network/IwinProtocol.h"

#include "Network/JsonObject/JsonHeader.h"
#include "Network/JsonObject/Heartbeat/HeartBeatConfiguration.h"

#include "Models/Achievement.h"
#include "Screens/ScreenManager.h"
#include "Models/Province.h"
#include "Models/Item.h"
#include "Models/Category.h"

#include "Screens/LoginScreen.h"
#include "Screens/GameScreens/GameScreens.h"

#include "../IwinCommand.h"
#include "Network/Game/GameLogicHandler.h"
#include "Network/Game/GameMessageHandler.h"

int GlobalLogicHandler::GAME_TYPE = -1;
std::function<void()> GlobalLogicHandler::onSetGameTypeOk = nullptr;
bool GlobalLogicHandler::isJoinGame = false;
GlobalLogicHandler::GlobalLogicHandler()
{

}

GlobalLogicHandler::~GlobalLogicHandler()
{

}

void GlobalLogicHandler::onSetClientInfoOK(std::string wellcom)
{
	GlobalService::sendSingleCommand(1702);
	if (GlobalService::actionConnectOk)
	{
		GlobalService::actionConnectOk();
		GlobalService::actionConnectOk = nullptr;
	}
}

void GlobalLogicHandler::onLoginSuccess()
{
	CCLOG("----------------------login sucess!");
	//@PDT: TODO
	//TrackingService.trackLoginSuccess(AccountTypes.iwin);
	GlobalService::sendSingleCommand(IwinProtocol::LOGIN_SUCESS);

	GameController::resetToNewAccount();
	//NewFriendScreen.ADDFRIEND_MESSAGE.clear();
	//NewFriendScreen.ADMIN_MESSAGE.clear();
	//DatabaseService.createDBWithName(String
		//.valueOf(GameController.myInfo->IDDB));

	GameController::setLoginType(AccountTypes::iwin);

	/*if (GameController.game.getScreen() == GameController.screenManager.registerScreen
		&& !Utility
		.isNullOrEmpty(GameController.screenManager.registerScreen
			.getUserName())) {
		GameController.myInfo
			.setName(LoginScreen.userName = GameController.screenManager.registerScreen
				.getUserName());
		LoginScreen.pass = GameController.screenManager.registerScreen
			.getPassword();
	}*/

	/*
	* luu lai thong tin de autologin, goi sau cai check o tren de gan
	* username,pass vo LoginAccountScreen username va pass
	*/
	GameController::saveAutoLogin();
	/*
	* khi đăng nhập thành công là quay về bước khởi điểm, xóa lịch sử màn
	* hình.
	*/
	/*GameController.game.clearScreenStack();
	GameController.screenManager.showMainScreen(null);
	GameController.screenManager.mainScreen.onLoginSuccess();*/
	HANDLE_MSG(IwinProtocol::LOGIN_SUCESS, nullptr);


	// Lay danh sach server moi
	GlobalService::getInfo(11);

	/**
	* Lay link more game.
	*/
	GlobalService::getInfo(15);
	// Lay tracker level.
	GlobalService::getInfo(14);

	// //////////////////////////
	GlobalService::getInfo(7); // Co tin gi moi khong
							  // Lay cai link Huong dan.
	GlobalService::getInfo(18);
	/*
	* khong biet no la cai gi
	*/
	GlobalService::sendSingleCommand(1703);

	/*if (Gdx.app.getType() != ApplicationType.WebGL) {
		Platform.instance.getCurrentLocation(new IActionListener(){

			@Override
			public void actionPerformed(Object o) {
			if (o instanceof GPSLocation) {
				GlobalService.sendLocation((GPSLocation)o);
			}
		}
		});
	}*/
	// Danh dau da login thanh cong
	//iWinOnlineGame.preferencesManager.setFirstTimeLoginOK();

	if (GAME_TYPE >= 0) {
		GameController::doPlay(GAME_TYPE);
		GAME_TYPE = -1;
	}
}

void GlobalLogicHandler::onLoginFacebookSuccess()
{
}

void GlobalLogicHandler::onLoginFail(string reason)
{
	CCLOG("----------------------login fail!");
	HANDLE_MSG(IwinProtocol::LOGIN_FAIL, (void*)reason.c_str());

}

void GlobalLogicHandler::onErrorMessage(string reason)
{
	CCLOG("Error message from server: %s", reason.c_str());
	HANDLE_MSG(IwinProtocol::ERROR_MESSAGE, (void*)reason.c_str());
}

void GlobalLogicHandler::onGetInfo(int subId, string info)
{
	CCLOG("GlobalLogicHandler::onGetInfo subId: %d info: %s", subId, info.c_str());
	//ScrMgr->OnHideDialog();
	switch (subId) 
	{
	case 1:
		//            DialogUtil.showTellFriendDialog(info);
		break;
	case 2:// Call hot line
//            iWinOnlineGame.preferencesManager.setHotLine(info);
		break;
	case 12:// Thong tin muc su kien.
//            if (GameController.game.getPreferencesManager().getLanguage() == LanguageType.en) {
//                info += "?lang=en";
//            }
	case 3: // Thong tin more games/la mot link trang web
	case 10://
//            DialogUtil.hideWaitDialog();
//            Gdx.app.log(TAG, "Show webview: " + info);
//            GameController.showWebViewer(info);
		break;
	case 11:
		// bỏ không cần dùng nữa
		break;
	case 6: // thông tin, tin tức mới nhất từ server. Bỏ
		break;
	case 7: // Có tin tức mới nào không
//            Log.log(TAG, "nhận về số tin tức mới : " + info);
//            int numNew = 0;
//            try {
//                numNew = Integer.parseInt(!Utility.isNullOrEmpty(info) ? info : "0");
//            } catch (Exception e) {
//            }
//            GameController.isHaveNewEvent = numNew > 0;
//            if (GameController.screenManager.mainScreen != null) {
//                GameController.screenManager.mainScreen.onReceivedNumNewEvent(numNew);
//            }
		break;
	case 13:// co gio vang ko
//            GameController.runBanner(info);
		break;
	case 14:// Set tracker.
//            Gdx.app.log(TAG, "set tracking level: " + info);
//            try {
//                TrackingService.setTrackerLevel(Integer.parseInt(info));
//            } catch (Exception e) {
//                TrackingService.setTrackerLevel(1);
//            }
		break;
	case 15:// Link lay more game.
//            if (!Utility.isNullOrEmpty(info) && !info.equals("null")) {
//                if (!info.endsWith("/"))
//                    info += "/";
//                if (Gdx.app.getType() == ApplicationType.iOS) {
//                    info += "ios";
//                } else if (Gdx.app.getType() == ApplicationType.Android) {
//                    info += "android";
//                } else if (Gdx.app.getType() == ApplicationType.Desktop) {
//                    String os = DeviceService.getManager().getOs().toLowerCase();
//                    if (os.contains("mac")) {
//                        info += "mac";
//                    } else {
//                        info += "win";
//                    }
//                } else if (Gdx.app.getType() == ApplicationType.WebGL) {
//                    info += "web";
//                }
//                iWinOnlineGame.preferencesManager.setMoreGameURL(info);
//            }
		break;
	case 16:
	{
		// Lay ve canh bao cua server khi vao man hinh chat.
		auto current_menu_ui = ScrMgr->GetCurrentMenuUI();
		if (current_menu_ui == FRIEND_SCREEN)
		{
			ScrMgr->SendDataToScreen(FRIEND_SCREEN, 16, (void*)info.c_str());
		}
		break;
	}
	case 17:// Lay email gop y.
//            DialogUtil.hideWaitDialog();
//            GameController.showEmailComposer(info);
		break;
	case 18:// Lay huong dan.
		// if (info != null && info.length() > 0) {
		// Gdx.app.log(TAG, "Get guild links from server: " + info);
		// iWinOnlineGame.preferencesManager.setGuide(info);
		// GuideService.addGuide(info);
		// }
		break;
	case 19:
	{
		// Lay link de get avatar tu version 4.3.3 tro ve truoc.
		//            if (info != null && info.length() > 0) {
		//                Gdx.app.log(TAG, "onGetInfo 19: " + info);
		//                iWinOnlineGame.preferencesManager.setAvatarLink(info);
		//                ImageService.extractLinkFromJSON(info);
		//            }

		HANDLE_MSG(subId, (void*)info.c_str());
		break;
	}
	case 20:// Get json de gioi thieu iwin cho ban be tren facebook.
//            DialogUtil.hideWaitDialog();
//            if (info != null && info.length() > 0) {
//                FBUtils.postWall(info, new IRequestActionCallBack() {
//                    
//                    @Override
//                    public void onSucceed(Object o) {
//                        DialogUtil.startOKDlg(L.gL(898));
//                    }
//                    
//                    @Override
//                    public void onFail(Object o) {
//                    }
//                });
//            }
		break;
	case 21:// Lay link de get avatar moi (tu version 4.3.4 tro ve sau)
//            if (info != null && info.length() > 0) {
//                // Luu vao pref
//                iWinOnlineGame.preferencesManager.setAvatarLink(info);
//                ImageService.extractLinkFromJSON(info);
//            }
		break;
	case 22:
		//            Log.log(TAG, "CountlyTracker: " + info);
		//            PreferenceService.getInstance().setCountlyTracker(info);
		break;
	case 23:
		if (!info.empty()) {
			HeartBeatConfiguration* config = new HeartBeatConfiguration();
			config->toData(info);
			GameMgr->startHeartBeat(config);
		}
		//            Gdx.app.log(TAG, "on received HeartBeatConfiguration: " + info);
		//            if (!Utility.isNullOrEmpty(info)) {
		//                HeartBeatConfiguration heartBeatConfiguration = GameController.game.json
		//                .fromJson(HeartBeatConfiguration.class, info);
		//                GameController.game.startHeartBeat(heartBeatConfiguration);
		//            }
		break;
	}
}

void GlobalLogicHandler::onReceivedGameList(GameList* gamelist)
{
	GameController::setGameList(gamelist);

	HANDLE_MSG(-1, (void*)"GET_GAME_LIST", (void*)gamelist, nullptr);

	//if(GameController::getMainScreen()) GameController::getMainScreen()->onReceivedGameList(gamelist);
	autoJoinGameAfterLogin();

}

void GlobalLogicHandler::autoJoinGameAfterLogin()
{
	/*PreferenceService pref = GameController.game.getPreferencesManager();
	if (pref.isAutoLogin() && GlobalService.isReconnect == false && isFirstLoginSuccessfully) {
		isFirstLoginSuccessfully = false;
		byte gameID = pref.getAutoJoinGameId();
		int money = pref.getAutoJoinGameBetMoney();
		if (GameController.isSupported(gameID) && gameID > -1 && money > -1) {
			Log.log(TAG, "autoJoinGameAfterLogin {game=" + GameController.getGameName(gameID) + " ,money=" + money
				+ "}");
			PickRoomScreen.autoJoinGameFindBoardBetMoney = money;
			GameController.doPlay(gameID);
		}
	}*/
}

void GlobalLogicHandler::OnArchivement(char gameId, std::vector<string> archivements, int avaXP)
{
	PASSERT2(false , "The method or operation is not implemented.");
}

void GlobalLogicHandler::onReceiveOwnerBoardList(std::vector<BoardOwner> boardOwners, int win, int lose, int draw, int leave)
{
	/*if (GameController::getLobbyScreen())
	{
		GameController::getLobbyScreen()->onReceiveOwnerBoardList(boardOwners, win, lose, draw, leave);
	}*/

	if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::LOBBY_SCREEN)
	{
		ScrMgr->SendDataToScreen(MENU_LAYER::LOBBY_SCREEN, -1, (void*)"GET_OWNER_BOARD_LIST", (void*)&boardOwners, (void*)&win, (void*)&lose, (void*)&draw, (void*)&leave);
	}
	else if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::NEW_LOBBY_SCREEN)
	{
		ScrMgr->SendDataToScreen(MENU_LAYER::NEW_LOBBY_SCREEN, 2, nullptr, (void*)&boardOwners, (void*)&win, (void*)&lose, (void*)&draw, (void*)&leave);
	}
}

void GlobalLogicHandler::onSetGameTypeSuccess(int gameID, bool isReconnect)
{
	//DialogUtil.hideWaitDialog();
	GameController::setGameType(gameID);
	GlobalService::isReconnect = isReconnect;
	/*if (isReconnect) {
		AbstractScreen baseScreen = GameController.game.getScreen();
		if (baseScreen instanceof BaseScreen) {
			((BaseScreen)baseScreen).loadResourceGame();
		}
		return;
	}*/
	if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::LOBBY_SCREEN)
	{
		ScrMgr->SendDataToScreen(MENU_LAYER::LOBBY_SCREEN, 0, nullptr);
	}
	else if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::NEW_LOBBY_SCREEN)
	{
		ScrMgr->SendDataToScreen(MENU_LAYER::NEW_LOBBY_SCREEN, 0, nullptr);
	}

}

void GlobalLogicHandler::onShowProfile(User * user)
{
	HANDLE_MSG(IwinProtocol::GET_PROFILE, (void*)user);
	delete user;
}

void GlobalLogicHandler::onShowProfileOfUser(User * user)
{
	//HANDLE_MSG(IwinProtocol::GET_PROFILE, (void*)user);
	GetCommonScr->OnSetUserPlayer(user);
	delete user;
}


void GlobalLogicHandler::onGetSMSSyntax( s16 subId, std::string smsData, std::string address, std::string confirmMessage)
{
	ScrMgr->SendDataToScreen(MENU_LAYER::LOGIN_SCREEN, IwinProtocol::GET_SMS_SYNTAX, nullptr, cv_void(subId), (void*)(smsData.c_str()), (void*)(address.c_str()), (void*)(confirmMessage.c_str()));
}

void GlobalLogicHandler::OnUserAllAchievement(int userId, std::vector<Achievement*> list)
{
	HANDLE_MSG(IwinProtocol::USER_DATA, cv_void(userId), cv_void(list));
}

void GlobalLogicHandler::onCallBackNameExist(std::string msg)
{
	CCLOG("name exist from server: %s", msg.c_str());
	ScrMgr->SendDataToScreen(MENU_LAYER::LOGIN_SCREEN, IwinProtocol::CHECK_USERNAME, (void*)msg.c_str());
}

void GlobalLogicHandler::onDisConnect()
{
	ScrMgr->ForceLogOutToLoginScreen();
}

void GlobalLogicHandler::onServerInfo(const char * msg)
{
	HANDLE_MSG(IwinProtocol::SERVER_INFO, (void*)(msg));
}

void GlobalLogicHandler::onServerMessage(const char * msg)
{
	//
	ScrMgr->SendDataToScreen(MAIN_SCREEN, -1, (void*)"SERVER_MSG", (void*)msg);
}

void GlobalLogicHandler::onReceiveMessageJson(std::string msg_json, std::string id_msg)
{
	if (msg_json == "") 
	{
		CCLOG("command %s is empty", id_msg.c_str());
		return;
	}

	if (id_msg == IwinCommand::GET_QUEST_CATEGORY_LIST || 
		id_msg == IwinCommand::GET_QUESTS_BY_CATEGORY_ID)
	{
		ScrMgr->SendDataToScreen(MENU_LAYER::EVENT_SCREEN, -1, (void*)id_msg.c_str(),(void*)msg_json.c_str());
	}
	else if (id_msg == IwinCommand::GET_AVA_CAT)
	{
		AvatarCatList * avatarCatList = new AvatarCatList();
		avatarCatList->toData(msg_json);
		onReceiveAvatarCatList(avatarCatList);
	}
	else if (id_msg == IwinCommand::GET_AVA)
	{
		AvatarList * avatarList = new AvatarList();
		avatarList->toData(msg_json);
		onReceiveAvatarList(avatarList);
	}
	else if (id_msg == IwinCommand::BUY_AVA)
	{
		BuyingAvatar * buyingAvatar = new BuyingAvatar();
		buyingAvatar->toData(msg_json);
		onBuyingAvatarResult(buyingAvatar);
	}
	else if (id_msg == IwinCommand::GET_OWNER_BOARD_LIST ||
		id_msg == IwinCommand::GET_BOARD_FROM_LOBBYID)
	{
		GetOnwerBoardListResponse* boardListResponse = new GetOnwerBoardListResponse();
		boardListResponse->toData(msg_json);
		onReceiveOwnerBoardList(boardListResponse->getBoardOwners(), boardListResponse->getWin(), boardListResponse->getLose(), boardListResponse->getDraw(), boardListResponse->getLeave());
	}
	else if (id_msg == IwinCommand::GET_CAPTCHA)
	{
		CaptchaInfo *info = new CaptchaInfo();
		info->toData(msg_json);
		onReceiveCaptcha(info);
		delete info;
	}
	else if (id_msg == IwinCommand::CHECK_CAPTCHA)
	{
		CaptchaResult *result = new CaptchaResult();
		result->toData(msg_json);
		onReceiveCaptchaResult(result);
		delete result;
	}
	else if (id_msg == IwinCommand::GET_LOBBY_LIST)
	{
		LobbyList* l = new LobbyList();
		l->toData(msg_json);
		onReceiveLobbyList(l);
	}
	else if (id_msg == IwinCommand::GET_GLOBAL_CHAT_LIST)
	{
		ChatLobbyList* l = new ChatLobbyList();
		l->toData(msg_json);
		onReceiveChatLobbyList(l);
	}
	else if (id_msg == IwinCommand::REQUEST_PAYMENT_INFOR)
	{
		Payment* payment = new Payment();
		payment->toData(msg_json);
		onReceivePayment(payment);
	}
	else if (id_msg == IwinCommand::GET_GAME_LIST)
	{
		GameList *gamelist = new GameList();
		gamelist->toData(msg_json);
		onReceivedGameList(gamelist);
	}
	else if (id_msg == IwinCommand::RUBYDT_GET_INFO)
	{
		RubyToVNDInfo* info = new RubyToVNDInfo();
		info->toData(msg_json);
		onReceiveListRubyToVND(info);
	}
	else if(id_msg == IwinCommand::RUBYDT_GET_CARD)
	{
		PaymentResult* giftResult = new PaymentResult();
		giftResult->toData(msg_json);
		onReceivePaymentResult(giftResult);
	}
	else if (id_msg == IwinCommand::RUBYDT_GET_HISTORY)
	{
		PaymentHistoryList* listHistory = new PaymentHistoryList();
		listHistory->toData(msg_json);
		onReceiveListHistory(listHistory);
	}
	else if (id_msg == IwinCommand::RUBYDT_CANCEL_GET_CARD)
	{
		CancelResult* cancel = new CancelResult();
		cancel->toData(msg_json);
		onReceiveCancelCashout(cancel);
	}
	else if (id_msg == IwinCommand::COUNT_QUEST_LIST)
	{ 
		int count = atoi(msg_json.c_str()); //  jm.getDataObject(Integer.class);
		onReceivedCountQuestList(count);
	}
	else if (id_msg == IwinCommand::REQUEST_FEATURE)
	{
		RequestFeature* feature = new RequestFeature();
		feature->toData(msg_json);
		onRequestFeature(feature);
		delete feature;
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_USER_INFO)
	{
		TaiXiuGameInfo * taiXiuInfo = new TaiXiuGameInfo();
		taiXiuInfo->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuInfo(taiXiuInfo,(byte)0);/*TaiXiuLayer.getInstance().GET_TAIXIU*/
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_USER_INFO_RUBY)
	{
		TaiXiuGameInfo *  taiXiuInfo_ruby = new TaiXiuGameInfo();
		taiXiuInfo_ruby->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuInfo(taiXiuInfo_ruby,(byte)0);/*TaiXiuLayer.getInstance().GET_TAIXIU*/
	}
	else if (id_msg == IwinCommand::TAIXIU_BET)
	{
		TaiXiuBetResult *  taiXiuBetResult = new TaiXiuBetResult();
		taiXiuBetResult->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuBetResult(taiXiuBetResult);
	}
	else if (id_msg == IwinCommand::TAIXIU_BET_RUBY)
	{
		TaiXiuBetResult * taiXiuBetResult_ruby = new TaiXiuBetResult();
		taiXiuBetResult_ruby->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuBetResult(taiXiuBetResult_ruby);

	}
	else if (id_msg == IwinCommand::TAIXIU_GET_TOP_USERS)
	{
		TaiXiuUserOrderList * taixiuOderList = new TaiXiuUserOrderList();
		taixiuOderList->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuUserOrderList(taixiuOderList);
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_TOP_USERS_RUBY)
	{
		TaiXiuUserOrderList * taixiuOderList_ruby = new TaiXiuUserOrderList();
		taixiuOderList_ruby->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuUserOrderList(taixiuOderList_ruby);
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_USER_HISTORY)
	{
		TaiXiuUserHistoryList * taixiuHistoryList = new TaiXiuUserHistoryList();
		taixiuHistoryList->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuUserHistoryList(taixiuHistoryList);
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_USER_HISTORY_RUBY)
	{
		TaiXiuUserHistoryList* taixiuHistoryList_ruby = new TaiXiuUserHistoryList();
		taixiuHistoryList_ruby->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuUserHistoryList(taixiuHistoryList_ruby);
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_INFO)
	{
		TaiXiuGameInfo* txInfo = new TaiXiuGameInfo();
		txInfo->toData(msg_json);

		GetCommonScr->OnReceivedTaiXiuInfo(txInfo, (byte)1);/*TaiXiuLayer.getInstance().UPDATE_TAIXIU*/
	}
	else if (id_msg == IwinCommand::TAIXIU_GET_UNSUPPORTED_LIST)
	{
		TaiXiuUnsupportedList* unsupport = new TaiXiuUnsupportedList();
		unsupport->toData(msg_json);

		ScrMgr->setTaiXiuUnsupportedList(unsupport);
	}
	else if (id_msg == IwinCommand::GET_LUCKY_CIRCLE_INFO)
	{
		LuckyCircleQuestList* lucky_result_list = new LuckyCircleQuestList();
		lucky_result_list->toData(msg_json);
		GetCommonScr->OnRecieveLuckyCircleResultList(lucky_result_list);

	}
	else if (id_msg == IwinCommand::TURNING_LUCKY_CIRCLE)
	{
		LuckyCircleResultList* lucky_result_item = new  LuckyCircleResultList();
		lucky_result_item->toData(msg_json);
		GetCommonScr->OnRecieveLuckyCircleResultItem(lucky_result_item);
		
	}
	else
	{
		GetGameMessageHandler()->onReceiveMessageJson(msg_json, id_msg);
	}
  

}
void GlobalLogicHandler::onReceiveCaptcha(CaptchaInfo* captcha)
{
	auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
	if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
	{
		current_top_ui = ScrMgr->GetCurrentMenuUI(1);
	}

	switch (current_top_ui)
	{
	case IAP_SCREEN:
	case LOGIN_SCREEN:
	case CHANGE_AWARD_SCREEN:
	{
		ScrMgr->SendDataToScreen(current_top_ui, -1, (void*)("GET_CAPTCHA"), (void*)(captcha));
		break;
	}
	default:
		break;
	}
}

void GlobalLogicHandler::onReceiveCaptchaResult(CaptchaResult* result)
{
	auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
	if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
	{
		current_top_ui = ScrMgr->GetCurrentMenuUI(1);
	}


	switch (current_top_ui)
	{
	case IAP_SCREEN:
	case LOGIN_SCREEN:
	case CHANGE_AWARD_SCREEN:
	{
		ScrMgr->SendDataToScreen(current_top_ui, -1, (void*)("CHECK_CAPTCHA"), (void*)(result));
		break;
	}
	default:
		break;
	}
}

void GlobalLogicHandler::onReceivePayment(Payment* payment)
{
	ScrMgr->SendDataToScreen(MENU_LAYER::IAP_SCREEN, 2, (void*)(payment));
}

void GlobalLogicHandler::onReceiveLobbyList(LobbyList* lobby_list)
{
	ScrMgr->SendDataToScreen(MENU_LAYER::NEW_LOBBY_SCREEN, 1, (void*)(lobby_list));
}

void GlobalLogicHandler::onReceiveChatLobbyList(ChatLobbyList* chat_lobby_list)
{
	ScrMgr->SendDataToScreen(MENU_LAYER::NEW_LOBBY_SCREEN, 3, (void*)(chat_lobby_list));
}

//void GlobalLogicHandler::onInviteJoinBoard(int gameId, int roomId, int boardId, std::string password, std::string message, int levelroom, int idInvite, std::string userName, int idAvatar)
void GlobalLogicHandler::onInviteJoinBoard(iwincore::Message* msg )
{
	char gameId = msg->readByte();
	char roomId = msg->readByte();
	char boardID = msg->readByte();
	std::string password = msg->readString();
	std::string message = msg->readString();
	char levelRoom = msg->readByte();// them byte level tu
									 // 247
	int idInvite = msg->readInt();// them byte id nguoi moi
								  // tu 248
	std::string userName = "";
	int idAvatar = -2;
	if (!msg->isEndRead())
	{
		userName = msg->readString();
		idAvatar = msg->readInt();
	}

	GetCommonScr->SetScreenComeFrom(ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI()));
	GetCommonScr->OnSetDataInvite(gameId, roomId, boardID, password, message, idAvatar);
}

void GlobalLogicHandler::onReceivedStrongestList(ubyte page, std::vector<Player *> list)
{
	HANDLE_MSG(IwinProtocol::STRONGEST_LIST, cv_void(page), cv_void(list));
}

void GlobalLogicHandler::onReceivedRichestWinList(ubyte page, std::vector<Player *> list)
{
	HANDLE_MSG(IwinProtocol::RICHEST_LIST, cv_void(page), cv_void(list));
}

void GlobalLogicHandler::onReceivedRichestRubyList(ubyte page, std::vector<Player * > list)
{
	HANDLE_MSG(IwinProtocol::RICHEST_RUBY_LIST, cv_void(page), cv_void(list));
}

void GlobalLogicHandler::onReceivedChangePass(bool is_ok, std::string dec)
{
	if (is_ok)
	{
		ScrMgr->OnShowDialog(
			"update_successful", dec, TYPE_DIALOG::DIALOG_ONE_BUTTON
		);
	}
	else
	{
		ScrMgr->OnShowDialog(
			"error", dec, TYPE_DIALOG::DIALOG_ONE_BUTTON
		);
	}
}

void GlobalLogicHandler::onShowDialogResult(bool is_ok, std::string dec)
{
	if (is_ok)
	{
		ScrMgr->OnShowDialog(
			"notice", dec, TYPE_DIALOG::DIALOG_ONE_BUTTON
		);
	}
	else
	{
		ScrMgr->OnShowDialog(
			"error", dec, TYPE_DIALOG::DIALOG_ONE_BUTTON
		);
	}
}

void GlobalLogicHandler::onRegisterInfo(
	std::string username,
	bool available,
	std::string smsPrefix,
	std::string smsTo,
	std::string info)
{
	if (!available) 
	{
		RKString text = LangMgr->GetString("have_account");
		ScrMgr->OnShowDialog("notice", text + " " + username, DIALOG_ONE_BUTTON);
	}
	else 
	{
		// Da dang ky thanh cong , luc nay luu thong tin lai de cho no dang
		// nhap luon
		auto login_ui = static_cast<LoginScreen*>(ScrMgr->GetMenuUI(LOGIN_SCREEN));
		//login_ui->startWaitingDialog();
		RKString current_pass = login_ui->GetPassword();
		GameMgr->Network_LoginToGame(username, current_pass);

		
	}
}

/**
* Nhan ve danh sach avatar category
*
* @param avatarCatList
*/
void GlobalLogicHandler::onReceiveAvatarCatList(AvatarCatList* avatarCatList) {

	if (avatarCatList != nullptr && avatarCatList->getList().size() > 0) 
	{
		//auto avatarCats = avatarCatList->getList();

		auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
		if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
		{
			current_top_ui = ScrMgr->GetCurrentMenuUI(1);
		}


		switch (current_top_ui)
		{
			case MAIN_SCREEN:
			case PROFILE_SCREEN:
			case HUD_SCREEN:
			{
				ScrMgr->SendDataToScreen(current_top_ui, -1, (void*)("GET_AVA_CAT"), (void*)(avatarCatList));
				break;
			}
			default:
			{
				PASSERT2(false, "have some thing wrong !");
				break;
			}
		}
	}
	else
	{
		PASSERT2(false, "have some thing wrong !");
	}

}

/**
* Nhan ve danh sach avatar
*
* @param avatarList
*/
void GlobalLogicHandler::onReceiveAvatarList(AvatarList* avatarList) {

	if (avatarList != nullptr && avatarList->getList().size() >0)
	{
		//auto avatarCats = avatarCatList->getList();

		auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
		if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
		{
			current_top_ui = ScrMgr->GetCurrentMenuUI(1);
		}


		switch (current_top_ui)
		{
		case MAIN_SCREEN:
		case PROFILE_SCREEN:
		case HUD_SCREEN:
		{
			ScrMgr->SendDataToScreen(current_top_ui, -1, (void*)("GET_AVA"), (void*)(avatarList));
			break;
		}
		default:
		{
			PASSERT2(false, "have some thing wrong !");
			break;
		}
		}
	}
	else
	{
		PASSERT2(false, "have some thing wrong !");
	}
}

/**
* Nhan ve tu server sau khi mua avatar
*
* @param buyingAvatar
*/
void GlobalLogicHandler::onBuyingAvatarResult(BuyingAvatar * buyingAvatar) 
{
	s64 curent_money = GameController::myInfo->money ;// //
	PASSERT2(GameController::myInfo->money == GameController::myInfo->userProfile->money, "problem here!");

	bool isValid = false;
	if (buyingAvatar->getIsSucceed() && curent_money > buyingAvatar->getMoney())
	{
		isValid = true;
	}
	else if (!buyingAvatar->getIsSucceed())
	{
		isValid = true;
	}

	if (buyingAvatar != nullptr && isValid)
	{
		if (buyingAvatar->getIsSucceed() && curent_money > buyingAvatar->getMoney())
		{
			GameController::myInfo->money = GameController::myInfo->userProfile->money = curent_money - buyingAvatar->getMoney();
			GameController::myInfo->avatar = buyingAvatar->getId();
			GameController::myInfo->userProfile->avatarID = buyingAvatar->getId();
			ScrMgr->MyInfoChanged();
			ScrMgr->OnShowDialog("notice", buyingAvatar->getMessage(), DIALOG_ONE_BUTTON);
		}

		if (!buyingAvatar->getIsSucceed())
		{
			ScrMgr->OnShowDialog("notice", 
				buyingAvatar->getMessage(), 
				DIALOG_TWO_BUTTON, "", 
				[](const char * str, const char * name_btn) 
			{
				auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
				if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
				{
					current_top_ui = ScrMgr->GetCurrentMenuUI(1);
				}

				ScrMgr->SwitchToMenu(MENU_LAYER::IAP_SCREEN, current_top_ui);

			},nullptr,nullptr,"top-up","");
		}

		delete buyingAvatar;
		return;

		auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
		if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
		{
			current_top_ui = ScrMgr->GetCurrentMenuUI(1);
		}

		switch (current_top_ui)
		{
		case MAIN_SCREEN:
		case PROFILE_SCREEN:
		case HUD_SCREEN:
		{
			ScrMgr->SendDataToScreen(current_top_ui, -1, (void*)("BUY_AVA"), (void*)(buyingAvatar));
			break;
		}
		default:
		{
			PASSERT2(false, "have some thing wrong !");
			break;
		}
		}
	}
}

void GlobalLogicHandler::onReceiveListWait(int countRoom, int countUser, std::map<int, GameObjectData*> list)
{
	ScrMgr->SendDataToScreen(MAIN_SCREEN, -1, (void*)("GET_STATISTIC_WAITING_BOARD_USER"), cv_void(countRoom), cv_void(countUser), cv_void(list));
}

void GlobalLogicHandler::onReceiveEnableRuby(bool enableRuby, bool enableCashout, std::string message)
{
	if(message != "")
		ScrMgr->OnShowDialog(LangMgr->GetString("notice"), message, TYPE_DIALOG::DIALOG_ONE_BUTTON);
}

void GlobalLogicHandler::onReceiveListRubyToVND(RubyToVNDInfo* info)
{
	ScrMgr->SendDataToScreen(CHANGE_AWARD_SCREEN, 1, (void*)info);
}

void GlobalLogicHandler::onReceivePaymentResult(PaymentResult* result)
{
	ScrMgr->SendDataToScreen(CHANGE_AWARD_SCREEN, 2, (void*)result);
}

void GlobalLogicHandler::onReceiveListHistory(PaymentHistoryList* list)
{
	ScrMgr->SendDataToScreen(CHANGE_AWARD_SCREEN, 3, (void*)list);
}

void GlobalLogicHandler::onReceiveCancelCashout(CancelResult* cancel)
{
	ScrMgr->SendDataToScreen(CHANGE_AWARD_SCREEN, 4, (void*)cancel);
}



void GlobalLogicHandler::onVerifyUser(int type_verify, bool is_success, std::string descript)
{
	if (type_verify != 1) 
	{ // email // or otherwise
		ScrMgr->OnShowDialog("notice",descript,DIALOG_ONE_BUTTON);
	}
	else if (type_verify == 1) 
	{ // Phone number
		if (is_success) 
		{ 		// Success verify phone numberr

			ScrMgr->OnShowDialog("notice", descript, DIALOG_ONE_BUTTON ,"", 
				[](const char * str, const char * btn_name)
			{
				MENU_LAYER current_screen = ScrMgr->GetCurrentMenuUI(0);
				if (current_screen == DIALOG_SCREEN)
				{
					current_screen = ScrMgr->GetCurrentMenuUI(1);
				}
				GetCommonScr->SetScreenComeFrom(ScrMgr->GetMenuUI(current_screen));
				GetCommonScr->OnShowConfirmCode();
			});
		}
		else 
		{
			ScrMgr->OnShowDialog("notice", descript, DIALOG_ONE_BUTTON);
		}
	}
	
}

void GlobalLogicHandler::onReceiveConfirmSMSCode(bool is_success, std::string description)
{
	if (is_success) 
	{
		ScrMgr->OnShowDialog("notice", description, DIALOG_ONE_BUTTON, "", 
			[](const char * str, const char * btn_name ) 
		{
			GlobalService::requestProfileOf(GameController::myInfo->IDDB);
		});	
	}
	else 
	{
		ScrMgr->OnShowDialog("notice", description, DIALOG_ONE_BUTTON);
	}
}

/**
* nhận được số lượng quest
*
* @param count
*/
void GlobalLogicHandler::onReceivedCountQuestList(int count) 
{
	HANDLE_MSG(-1, cv_void("COUNT_QUEST_LIST"), cv_void(count));
}
void GlobalLogicHandler::onRequestFeature(RequestFeature* feature)
{
	if (feature->getType() == 1)
	{

	}
	else
	{
		showDialogDieuHuong(feature);
	}
}

void GlobalLogicHandler::showDialogDieuHuong(RequestFeature* feature)
{
	int num_btn = feature->getListButton().size();
	if (num_btn == 0)
	{
		int featureId = feature->getFeatureId();
		int gameId = feature->getGameId();
		ScrMgr->OnShowDialog(LangMgr->GetString("notice"), feature->getMessage(), TYPE_DIALOG::DIALOG_ONE_BUTTON, "", [featureId, gameId](const char* call_back, const char* btn_name) {
			ScrMgr->showScreen(featureId, gameId);
		});
	}
}
/**
* nhan tien sau khi nap
*
* @param newMoney
* @param info
*/
void GlobalLogicHandler::onTransferMoney(s64 newMoney, std::string info, int moneyType)
{
	s64 delta_change_money = 0;
	if (moneyType == BASE_MONEY)
	{
		delta_change_money = newMoney - GameController::myInfo->money;
		GameController::myInfo->money = newMoney;// //
		GameController::myInfo->userProfile->money = newMoney;
	}
	else if (moneyType == RUBY_MONEY)
	{
		delta_change_money = newMoney - GameController::myInfo->ruby;
		GameController::myInfo->ruby = newMoney;
		GameController::myInfo->userProfile->ruby = newMoney;
	}

	ScrMgr->OnReceiveTransferMoney(delta_change_money, info, moneyType);
}

void GlobalLogicHandler::onSearchResult(std::vector<Player*> searchList)
{
	if (searchList.size() > 0)
	{
		if (GameController::searchNick == "")
		{
			PASSERT2(false, "nickname need to search must be set !");
			return;
		}
		for (Player * p : searchList)
		{
			if (p->name == GameController::searchNick)
			{
				if (!GameController::isFriend(p->IDDB))
				{
					ScrMgr->OnShowDialog("notice", (LANG_STR("friend_adding")).GetString() + p->name, DIALOG_ZERO_BUTTON_WAIT);
					GlobalService::addFriend(p->IDDB, p->name);
				}
				else
				{
					onShowDialogResult(true , (p->name + " " +LANG_STR("is_already_in_friend")).GetString());
				}			
				return;
			}
		}
	}
	else
	{
		ScrMgr->OnShowDialog("notice", "cannot_find_this_username", DIALOG_ONE_BUTTON);
	}
}

void GlobalLogicHandler::onAddFriendResult(ubyte addResult)
{
	if (addResult == 0) 
	{
		ScrMgr->OnShowDialog("notice", "already_sent_reques", DIALOG_ONE_BUTTON);
		// thanh cong
	}
	else if (addResult == 2) 
	{
		ScrMgr->OnShowDialog("notice", "unable_to_add_you_have", DIALOG_ONE_BUTTON);
		// qua nhieu ban
	}
	else 
	{
		ScrMgr->OnShowDialog("notice", "is_already_in_friend", DIALOG_ONE_BUTTON);
		// da co trong danh sach
	}

	/*if (GameController.game.profileDialog != null && GameController.game.profileDialog.hasParent()) 
	{
		GameController.game.profileDialog.onAddFriendResult(addResult);
	}*/
}

void GlobalLogicHandler::onFriendConfirmationResult(std::string msg)
{
	ScrMgr->OnShowDialog("notice", msg, DIALOG_ONE_BUTTON);

	auto current_top_ui = ScrMgr->GetCurrentMenuUI(0);
	if (current_top_ui == MENU_LAYER::DIALOG_SCREEN)
	{
		current_top_ui = ScrMgr->GetCurrentMenuUI(1);
	}

	switch (current_top_ui)
	{
		case FRIEND_SCREEN:
		{
			GlobalService::requestFriendList();
			break;
		}
		default:
			break;
	}
}

void GlobalLogicHandler::onRequestAddFriend(std::vector<IwinMessage*> arrMessage)
{
    //have delete when push arrMessage
	PUSH_ADDFRIEND_MESSAGE(arrMessage);
	//PASSERT2(false, "arrMsg need to handle here")
	GameMgr->updateUnreadMsg();
}

/**
* danh sach ban be
*
* @param friendList
*/
void GlobalLogicHandler::onFriendList(std::vector<Player*> friendList)
{
	CCLOG("Danh sách bạn iWin : %d", friendList.size() );
	
	GameController::SetFriendList(friendList);

	/*if (GameController.currentGameScreen != null) 
	{
		if (GameScreen.invitationLayer != null) {
			GameScreen.invitationLayer.setInvitationList(friendList);
		}
	}*/
	//PASSERT2(false, "invitation layer handle here");
	//ScrMgr->OnHideDialog();
	//notified receive friend list
	ScrMgr->onReceiveFriendList();
}

void GlobalLogicHandler::onListInvite(std::vector<Player*> list)
{
	PASSERT2(false, "func empty!");
	// if (GameController.game.getScreen() ==
	// GameController.game.friendScreen) {
	// GameController.game.friendScreen.onListInvite(list);
	// }
}

void GlobalLogicHandler::onListWaitingFriend(std::vector<Player*> list)
{
	PASSERT2(false, "func empty!");
}
void GlobalLogicHandler::onMessageFrom(IwinMessage * m)
{
	PASSERT2(false, "not impelement this!");
}

void GlobalLogicHandler::onDelFriendResult(ubyte delResult)
{
	PASSERT2(false, "not impelement this!");
}

void GlobalLogicHandler::onReceivedLeaderboardFriendList(int state, std::vector<Player*> list)
{
	ScrMgr->OnHideDialog();

	int page = 0;
	HANDLE_MSG(-1, cv_void("LEADERBOARD_FRIEND"), cv_void(state), cv_void(page), cv_void(list));
	
}


void GlobalLogicHandler::onProvinceList(std::vector<Province*> provinceList)
{
	CCLOG("GlobalLogicHandler : Danh sach tinh thanh : so luong = %d",provinceList.size());
	Province::list = provinceList;
	PASSERT2(false, "list province not show on profile screen");
	/*if (GameController.screenManager.profileScreen != null) 
	{
		GameController.screenManager.profileScreen.onProvinceList();
	}*/
}

void GlobalLogicHandler::onWattingList(std::vector<Player*> l)
{
	if (GameController::currentGameScreen)
	{
		GameController::currentGameScreen->OnReceiveWaitingList(l);
	}
	else
	{
		PASSERT2(false, "msg waiting list receive here");
		for (auto p : l)
		{
			delete p;
		}
		l.clear();
	}
}

/**
* lay avatar cua toi
*/
void GlobalLogicHandler::onMyAvatars(std::vector<Item*> items) {
	ScrMgr->OnHideDialog();
	PASSERT2(false, "not handle here");
	ScrMgr->SendDataToScreen(PROFILE_SCREEN, -1, (void*)"ON_MY_AVATAR", (void*)&items);
}

/**
* lay vat pham cua toi
*/
void GlobalLogicHandler::onBoughtItems(std::vector<Item*> listItem, int cateId, int totalPage, ubyte gender) {
	ScrMgr->OnHideDialog();
	ScrMgr->SendDataToScreen(PROFILE_SCREEN, -1, (void*)"BOUGHT_ITEMS", (void*)&listItem, (void*)& cateId, (void*)&totalPage, (void*)&gender);
}
/**
* danh sach vat pham su kien (server cho ,co the mo ra)
*/
void GlobalLogicHandler::onEventItemsList(std::vector<Item*> items)
{
	ScrMgr->OnHideDialog();
	ScrMgr->SendDataToScreen(PROFILE_SCREEN, -1, (void*)"EVENT_ITEMS_LIST", (void*)&items);
}

/**
* nhan duoc vat pham sau khi hoan thanh 1 nhiem vu nao do chi la hinh hien
* len choi thoi
*
* @param itemID
* @param itemName
*/
void GlobalLogicHandler::onEventItems(int itemID, std::string itemName)
{
	ScrMgr->OnShowDialog("notice" , "Xin chúc mừng !Bạn nhận được " + itemName , TYPE_DIALOG::DIALOG_ONE_BUTTON);
	//GameScreen.flyItem(itemID, itemName);
}

void GlobalLogicHandler::onChangeMyAvatar(int avatar)
{
	ScrMgr->OnHideDialog();
	// ItemManager.deleteCacheAvatar(GameController.myInfo.IDDB,
	// GameController.myInfo.avatar);
	// ItemManager.deleteCacheAvatarFromWeb(GameController.myInfo.name,
	// GameController.myInfo.avatar);
	GameController::myInfo->avatar = avatar;
	// if (GameController.loginType == AccountTypes.facebook) {
	// DialogUtil.startOKDlg(L.gL(520), true);
	
	ScrMgr->SendDataToScreen(PROFILE_SCREEN, -1, (void*)"CHANGE_AVATAR_RESULT", (void*)&avatar);
	ScrMgr->MyInfoChanged();
}

void GlobalLogicHandler::onReceivedGiftCategoties(std::vector<Category*> categories) {
	if (GameController::currentGameScreen != nullptr) 
	{
		GameController::currentGameScreen->setGiftCategories(categories);
	}
}

void GlobalLogicHandler::onReceviedGiftItems(int categoryID, int totalPage, std::vector<Item*> items) {
	if (GameController::currentGameScreen != nullptr) 
	{
		GameController::currentGameScreen->setGiftItems(categoryID, totalPage, items);
	}
}

void GlobalLogicHandler::sendGitfTo(int from, std::vector<int> tos, int itemID) {
	if (GameController::currentGameScreen != nullptr) 
	{
		GameController::currentGameScreen->sendGiftTo(from, tos, itemID);
	}
}


void GlobalLogicHandler::onHeartBeatResponse()
{
	GameMgr->onHeartBeatResponse();
}
