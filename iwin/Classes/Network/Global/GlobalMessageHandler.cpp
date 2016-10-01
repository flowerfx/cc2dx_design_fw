#include "LanguageMgr.h"
#include "Common/Common.h"
#include "GlobalMessageHandler.h"
#include "Network/Game/GameMessageHandler.h"
#include "Common/GameController.h"
#include "Services/GuideService.h"
#include "Globalservice.h""
#include "Network/IwinProtocol.h"
#include "Models/Province.h"
#include "Models/Achievement.h"
#include "Models/ChatMessage.h"
#include "Models/Item.h"
#include "Models/Category.h"

using namespace std;
using namespace iwincore;
using namespace Utility;
GlobalMessageHandler::GlobalMessageHandler()
{
	globalLogicHandler = new GlobalLogicHandler();
	jsonMessageHandler = new JSONMessageHandler(globalLogicHandler);
}
GlobalMessageHandler::~GlobalMessageHandler()
{
	if (jsonMessageHandler)
		delete jsonMessageHandler;
}

std::string GlobalMessageHandler::createOnlineInfo(ubyte room, ubyte board, ubyte gameId)
{
	std::string a = "";
	if (gameId != -1)
	{
		std::string sub = "";

			a = sub +
				(
				(gameId == GameType::GAMEID_TIENLEN || gameId == GameType::GAMEID_TIENLEN_SOLO) ? LANG_STR("killer_13s")
					: (gameId == GameType::GAMEID_TIENLEN_MB ? LANG_STR("killer_13n")
						: "GAME_ID"/*GameController::getGameName(gameId)*/)
					).GetString();
	}
	return a;
}


void GlobalMessageHandler::onMessage(iwincore::Message* msg)
{
	CCLOG("->>message command: %d", msg->getCommandId());
	//try
	{
		switch (msg->getCommandId())
		{
		case IwinProtocol::RESPOND_QUESTION_FROM_SERVER:
		{
			CCLOG("----------------------------------------------- RESPOND_QUESTION_FROM_SERVER");
			// int commandId3001 = msg->readInt();
			int questionSize3001 = msg->readInt();
			int* questionId = new int[questionSize3001];
			string* questionstring = new string[questionSize3001];
			string* info_getWin = new string[questionSize3001];
			int* litmitTime = new int[questionSize3001];
			for (int i = 0; i < questionSize3001; i++) {
				questionId[i] = msg->readInt();
				info_getWin[i] = msg->readString();
				questionstring[i] = msg->readString();
				litmitTime[i] = msg->readInt();
			}
			if (questionSize3001 == 1) { // aMinh nhắn nhủ khi size == 1 thì
										 // show luôn dialog, còn > 1 thì
										 // hiển thị list câu hỏi ở tab
										 // khác
										 /*	globalLogicHandler.onReceivedQuestion(IwinProtocol::RESPOND_QUESTION_FROM_SERVER, questionId[0],
										 questionstring[0], litmitTime[0], info_getWin[0]);*/
			}
		}
		break;
		case IwinProtocol::HEART_BEAT:
			//GameController.game.onHeartBeatResponse();
			break;
		case 2704:
		{
			int sub = msg->readInt();
			switch (sub) {
			case 0:
				/*DialogUtil.hideWaitDialog();
				GameController.showWebViewer(msg->readString());*/
				break;
			}
		}
		break;
		case 2: {
			// lay thong tin tren web
			string question = msg->readString();
			string link = msg->readString();
			//globalLogicHandler.saveExitQuestionLink(question, link);

		}
				break;
		case IwinProtocol::REGISTER_INFO: {
            std::string username = msg->readString();
			bool available = msg->readBool();
			std::string smsPrefix = msg->readString();
			std::string sendTo = msg->readString();
			std::string info1 = msg->readString();
			/*	if (GameController.screenManager.registerScreen != null)
			GameController.screenManager.registerScreen.warningButton[0].setVisible(false);
			*/
			CCLOG("IwinProtocol::REGISTER_INFO username: %s ; smsPrefix: %s ; sendTo: %s ; info: %s",
                  username.c_str(),
                  smsPrefix.c_str(),
                  sendTo.c_str(),
                  info1.c_str());

			globalLogicHandler->onRegisterInfo(username, available, smsPrefix, sendTo, info1);
			break;
		}
		case IwinProtocol::CHECK_USERNAME: {
			std::string ms = msg->readString();
			std::string ms1 = "";
			if (ms == "")
			{
				ms1 = msg->readString();
			}
			//std::string ms2 = msg->readString();
			globalLogicHandler->onCallBackNameExist(ms1);
			break;
		}
		case 2800:
			jsonMessageHandler->onMessage(msg->readString());
			break;
		case 220:// Re
		{
			/*DialogUtil.hideWaitDialog();
			byte isOK = msg->readByte();
			if (isOK == 1) {
			globalLogicHandler.onSetClientInfoOK(null);
			}
			else {
			onConnectionFail();
			}*/
		}
		break;
		case IwinProtocol::LOGIN_FAIL:
			globalLogicHandler->onLoginFail(msg->readString());
			break;
		case IwinProtocol::REQUIRE_UPDATE_PASS:// buoc doi mat khau
											   //globalLogicHandler.onReceivedChangePass();
			break;
		case IwinProtocol::LOGIN_SUCESS:
			// Update lai thong tin cho doi tuong myInfo->
			if (!GameController::myInfo)
			{
				GameController::myInfo = new Player();
			}
            else
            {
                delete GameController::myInfo;
                GameController::myInfo = new Player();
            }
			GameController::myInfo->IDDB = msg->readInt();
			GameController::myInfo->money = msg->readLong();
			GameController::myInfo->avatar = msg->readShort();
			GameController::myInfo->exp = msg->readInt();
			//CCLOG(TAG, "Login user id: %d" + GameController.myInfo->IDDB);
			if (!msg->isEndRead())
				GameController::myInfo->isActived = msg->readBool();
			if (!msg->isEndRead())
				GameController::myInfo->ruby = msg->readLong();
			if (!msg->isEndRead())
				GameController::myInfo->name = msg->readString();
			globalLogicHandler->onLoginSuccess();
			break;
		case IwinProtocol::GET_INFO:
		{
			int subId = (char)msg->readByte();
			string info = "";
			if (subId == -1) {
				subId = msg->readByte();
			}
			else {
				info = msg->readString();
			}
			globalLogicHandler->onGetInfo(subId, info);
		}
		break;
		case IwinProtocol::SET_GAME_TYPE:
		{
			globalLogicHandler->onSetGameTypeSuccess(msg->readByte(), false);
		}
		break;
		case IwinProtocol::RECONNECT:
		{
			globalLogicHandler->onSetGameTypeSuccess(msg->readByte(), true);
		}
		break;
		case IwinProtocol::ERROR_MESSAGE:
		{
			globalLogicHandler->onErrorMessage(msg->readString());
		}
		break;

		case IwinProtocol::GET_PROFILE:
		{
			User * userData = new User();

			userData->id = msg->readInt();
			userData->nickName = msg->readString();
			userData->money = msg->readLong();
			userData->fullname = msg->readString();
			userData->gender = msg->readByte();
			userData->genderStatus = msg->readBool();
			userData->yearOfBirth = msg->readShort();
			userData->address = msg->readString();
			userData->idnumber = msg->readString();
			userData->idnumberStatus = msg->readBool();
			userData->status = msg->readString();
			userData->phoneNo = msg->readString();
			userData->province = msg->readByte();

			Province * a = new Province();
			a->id = userData->province;
			a->name = msg->readString();
			if (userData->province != -1
				&& Province::getProviceName(a->id) == std::to_string(a->id))
			{
				Province::list.push_back(a);
			}
			userData->monthOfBirth = msg->readByte();
			userData->dayOfBirth = msg->readByte();
			userData->birthdayStatus = msg->readBool();
			userData->percent = msg->readInt();
			userData->isSentGift = msg->readBool();
			userData->playingTime = msg->readString();
			userData->avatarID = msg->readInt();
			userData->benefitElo = msg->readString();
			userData->bestElo = msg->readString();
			userData->avaElo = msg->readInt();
			userData->eloT = msg->readString();

			userData->email = msg->readString();
			userData->isEmailValidate = msg->readBool();
			userData->isPhoneValidate = msg->readBool();
			userData->ruby = msg->readLong();

			if (userData->id == GameController::myInfo->IDDB)
			{
				if (userData->money != GameController::myInfo->money)
				{
					userData->money = MAX(userData->money, GameController::myInfo->money);
				}
				if (userData->ruby != GameController::myInfo->ruby)
				{
					PASSERT2(false, "userData ruby is differrent!");
					userData->ruby = MAX(userData->ruby, GameController::myInfo->ruby);
				}

				GameController::myInfo->setUserProfile(userData);
			}
			globalLogicHandler->onShowProfile(userData);
		}
		break;
		case IwinProtocol::USER_DATA:
		{
			//DialogUtil.hideWaitDialog();
			char type = msg->readByte();
			switch (type)
			{
			case 5:
			{
				// archivements, thong tin cap do game.
				msg->readInt();
				char gameId = msg->readByte();
				int xp = msg->readInt();
				int win = msg->readInt();
				int lose = msg->readInt();
				int draw = msg->readInt();
				int giveup = msg->readInt();
				int avaXP = 0;

				int elo = msg->readInt();
				string descXP = msg->readString();
				avaXP = msg->readInt();

				
				std::vector<string> archivements = {
					std::to_string(xp),
					// "Elo: " + elo,
					// L.Get(196) + descXP,
					std::to_string(win), std::to_string(lose),
					std::to_string(draw), std::to_string(giveup) };


				globalLogicHandler->OnArchivement(gameId, archivements, avaXP);
			}
			break;
			case 6:
				// lay thanh tich tat ca game
				int userId = msg->readInt();
				std::vector<Achievement*> list;
				while (!msg->isEndRead())
				{
					s16 game_id = msg->readByte();
					int exp = msg->readInt();
					int win = msg->readInt();
					int lose = msg->readInt();
					int draw = msg->readInt();
					int give_up = msg->readInt();
					int elo_number = msg->readInt();
					std::string elo = msg->readString();
					int elo_id = msg->readInt();
					Achievement* achieve = new Achievement(
						game_id,
						exp,
						win,
						lose,
						draw,
						give_up,
						elo_number,
						elo,
						elo_id);
					if (achieve->gameId != GameController::gameId)
					{
						list.push_back(achieve);

					}
					else
					{
						list.insert(list.begin() + 0, achieve);
					}
				}
				globalLogicHandler->OnUserAllAchievement(userId, list);
				break;
			}
		}
		break;
		case IwinProtocol::SERVER_MESSAGE:
		{
			int subId = msg->readByte();

			switch (subId)
			{
			case 0:
			{
				std::string server_message = msg->readString();
				CCLOG("SERVER MESSAGE: SUBid: %d Message: %s", subId, server_message.c_str());

				globalLogicHandler->onServerMessage(server_message.c_str());
				break;
			}
			case 1:
			{
				std::string server_message = msg->readString();
				CCLOG("SERVER MESSAGE: SUBid: %d Message: %s", subId, server_message.c_str());
				globalLogicHandler->onServerInfo(server_message.c_str());
				break;
			}
			case 2:
			{
				std::string server_message = msg->readString();
				std::string link = msg->readString();
				//globalLogicHandler.onOpenWap(info, link);

				CCLOG("SERVER MESSAGE: SUBid: %d link : %s Message: %s", subId, link.c_str(), server_message.c_str());

				break;
			}
			case 3:
			{
				// nhan duoc vat pham su kien
				// Trung thuong
				int itemId;
				std::string itemName;
				while (!msg->isEndRead())
				{
					itemId = msg->readInt();
					itemName = msg->readString();
					globalLogicHandler->onEventItems(itemId, itemName);

				}
				break;
			}
			case 4:
			{
				// Cần phải login để dùng chức năng này
				// globalLogicHandler.onNeedLogin(msg.readString());
				break;
			}
			}

			break;
		}
		case IwinProtocol::GET_SMS_SYNTAX:
		{
			ubyte subId = msg->readByte();
			std::string smsData = msg->readString();
			std::string address = msg->readString();
			std::string confirmMessage = msg->readString();
			//Log.log(TAG, Util.getLogString("GET_SMS_SYNTAX", "subId", "smsData", "address", "confirmMessage", subId,
				//smsData, address, confirmMessage));
			globalLogicHandler->onGetSMSSyntax(subId, smsData, address, confirmMessage);
		}
		break;
		case IwinProtocol::LOGIN_WITH_ACCESS_TOKEN:
		{
			break;
		}
		case IwinProtocol::SERVER_INFO:
		{
			std::string broast_cast_message = msg->readString(); 
			CCLOG("Server Info: %s", broast_cast_message.c_str());
			globalLogicHandler->onServerInfo(broast_cast_message.c_str());
			break;
		}
		case IwinProtocol::INVITE_PLAYER:
		{
			globalLogicHandler->onInviteJoinBoard(msg);
			break;
		}
		case IwinProtocol::GET_IMAGE: 
		{
			int type = (int)msg->readByte();
			switch (type) 
			{
			case 3:
				// BG cu
				//int itemId = msg->readInt();
				break;
			case 4:
				// item
				// int itemId = msg.readInt();
				// int size = msg.readShort();
				// byte[] data = new byte[size];
				// if (size > 0) {
				// msg.read(data);
				// }
				break;
			case 5:
				// // Bg
				// int idi = msg.readInt();
				// int local = msg.readInt();
				// int size = msg.readShort();
				// byte[] data = new byte[size];
				// if (size > 0) {
				// msg.read(data);
				// globalLogicHandler.onBackgroundImage(idi, local, data);
				// }
				break;
			case 6:
				// // Logo
				// int size = msg.readShort();
				// byte[] data = new byte[size];
				// if (size > 0) {
				// msg.read(data);
				// globalLogicHandler.onLogoImage(data);
				// }
				break;
			}
			break;
		}
		case IwinProtocol::GET_LIST: 
		{
			ubyte type = msg->readByte();
			if (type == 1) 
			{
				// Danh sach cac loai top dai gia/cao thu
				// Array<Object[]> list = new Array<Object[]>();
				// available = true;
				// while (available) {
				// try {
				// Object[] listItem = new Object[2];
				// listItem[0] = msg.readByte();
				// listItem[1] = msg.readString();
				// list.add(listItem);
				// } catch (Exception e) {
				// available = false;
				// }
				// }
				// globalLogicHandler.onTopList(list);
			}
			else if (type == 3) 
			{
				std::vector<Province*> provinceList;

				while (!msg->isEndRead()) 
				{
					Province* p = new Province();
					p->id = msg->readByte();
					p->name = msg->readString();
					provinceList.push_back(p);				
				}

				globalLogicHandler->onProvinceList(provinceList);
			}
			else if (type == 2) 
			{
				std::vector<Player*> l;
				while (!msg->isEndRead())
				{
					Player *p = new Player();
					p->IDDB = msg->readInt();
					p->setName(msg->readString());
					p->setMoney(msg->readLong());
					p->avatar = msg->readShort();
					p->setExp(msg->readInt());
					p->isReady = true; // Is online
					p->userProfile->status = msg->readString();
					l.push_back(p);
				}
				globalLogicHandler->onWattingList(l);
			}
			break;
		}
		case IwinProtocol::GET_BOUGHT_AVATARS_USE_TEXT_TIME: {
			// lay avatar cua toi
			int n = msg->readInt();
			vector<Item*> items ;
			if (n > 0) 
			{
				for (int i = 0; i < n; i++) 
				{
					auto it = new Item(Item::TYPE_ITEM::AVATAR);
					it->userAvatarID = msg->readInt();
					it->id = msg->readInt();
					it->name = msg->readString();
					it->description = msg->readString();
					items.push_back(it);
				}
			}
			globalLogicHandler->onMyAvatars(items);
			break;
		}														
		case IwinProtocol::ITEMS:
		{
			// if (gameMessageListener != null
			// && !ProfileLayer.class.getName().equals(
			// GameController.game.getScreen().TAG)) {
			// gameMessageListener.onMessage(msg);
			// break;
			// }
			ubyte sId = msg->readByte();
			switch (sId) 
			{
			case 14: // send gift in game. Gift catelory.
			{
				std::vector<Category*> l ;
				while (!msg->isEndRead()) {
					Category * i = new Category();
					i->categoryId = msg->readInt();
					i->name = msg->readString();
					i->parentId = msg->readInt();
					l.push_back(i);
				}
				globalLogicHandler->onReceivedGiftCategoties(l);
			}
			break;
			case 15: // Gift in the catelory.
			{
				std::vector<Item *> l;
				int type = msg->readInt();
				int totalPage = msg->readInt();
				while (!msg->isEndRead()) {
					Item * i = new Item(Item::TYPE_ITEM::ITEM);
					i->id = msg->readInt();
					i->name = msg->readString();
					i->price = msg->readInt();
					l.push_back(i);
				}
				globalLogicHandler->onReceviedGiftItems(type, totalPage, l);
			}
			break;
			case IwinProtocol::ITEMS_GIVE_ITEM: // Do send gift.
			{
				ubyte ok = msg->readByte();
				if (ok == 0) 
				{
					// Read from userID.
					int fromUserID = msg->readInt();
					// Read itemID.
					int itemID = msg->readInt();
					// Read to userIDs.
					std::vector<int> toUsers;
					while (!msg->isEndRead()) 
					{
						toUsers.push_back(msg->readInt());
					}
					globalLogicHandler->sendGitfTo(fromUserID, toUsers, itemID);
				}
				else if (ok == 1) 
				{
					globalLogicHandler->onShowDialogResult(true, msg->readString());
				}
			}
			break;
			// case 21:// request shop vat pham
			// {
			// Array<Item> items = new Array<Item>();
			// int totalShopItemPage = msg.readInt();
			// available = true;
			// while (available) {
			// try {
			// Item i = new Item(Item.AVATAR);
			// i.id = msg.readInt();
			// i.name = msg.readString();
			// i.price = msg.readInt();
			// i.description = msg.readString();
			// i.isBuyUse = msg.readBoolean();
			// items.add(i);
			// } catch (Exception e) {
			// available = false;
			// }
			// }
			// /*
			// * totalPage: int { itemid: int itemname: String price: int
			// * desc: String isBought: boolean, isBought: true lÃ  mua
			// * rá»“i, false lÃ  chÆ°a mua }
			// */
			// break;
			// }
			// case IWINProtocol.ITEMS_GET_ITEMS_BY_CATEGORY: {
			// Array<Item> items = new Array<Item>();
			// msg.readInt();// Doc catalo type, cai moi hien k
			// // dung, doc bo.
			// msg.readByte();// Doc byte gioi tinh, hien khong
			// // dung, doc bo.
			// int totalPage = msg.readInt();
			// available = true;
			// while (available) {
			// try {
			// Item i = new Item(Item.ITEM);
			// i.id = msg.readInt();
			// i.name = msg.readString();
			// i.setPrice(msg.readInt());
			// i.description = msg.readString();
			// i.isBuyUse = msg.readBoolean();
			// items.add(i);
			// } catch (Exception e) {
			// available = false;
			// }
			// }
			// break;
			// }
			/*
			* case IWINProtocol.ITEMS_GET_DETAILS_USER_ITEM: //#if
			* DefaultConfiguration System.out.println("SERVER:
			* -------------ITEMS_GET_DETAILS_USER_ITEM 01"); //#endif //
			* Vector itemAsetList = new Vector(); // while (msg.available()
			* > 0) { ItemDetail itemAset = new ItemDetail();
			* itemAset.userItemId = msg.readInt(); itemAset.ID =
			* msg.readInt(); itemAset.name = msg.readString();
			* itemAset.efftectedGame = msg.readString();
			* itemAset.effectedHours = msg.readShort(); itemAset.itemGender
			* = msg.readByte(); itemAset.desc = msg.readString();
			* itemAset.expiredDate = msg.readString(); itemAset.isBuyOrGive
			* = msg.readByte(); itemAset.isUsing = msg.readBoolean();
			* itemAset.timeUsed = msg.readShort(); //itemAset. //#if
			* DefaultConfiguration System.out.println("<SERVER
			* <<<<<<<<<<<<<<<<<<<ITEMS_GET_DETAILS_USER_ITEM: " +
			* itemAset.ID); System.out.println("<SERVER
			* <<<<<<<<<<<<<<<<<<<ITEMS_GET_DETAILS_USER_ITEM: " +
			* itemAset.isUsing); //#endif
			* globalLogicHandler.onSetItemDetails(itemAset, (byte) 1);
			* break;
			*/
			case IwinProtocol::ITEMS_GET_ITEMS_BY_USER:
			{
				// lay cac vat pham ma user co
				std::vector<Item*> listItem;
				int cateId = msg->readInt();
				ubyte gender = msg->readByte();
				int totalPage = msg->readInt();
				while (!msg->isEndRead()) 
				{
					auto item = new Item(Item::TYPE_ITEM::ITEM);
					item->userItemId = msg->readInt();
					item->id = msg->readInt();
					item->name = msg->readString();
					item->price = msg->readInt();
					item->description = msg->readString();
					item->isBuyUse = msg->readBool();
					listItem.push_back(item);
				}
				globalLogicHandler->onBoughtItems(listItem, cateId, totalPage, gender);
				break;
			}
			case 16: 
			{
				std::vector<Item*> assets ;

				while (!msg->isEndRead()) {
						Item * item = new Item(Item::TYPE_ITEM::EVENT_ITEM);
						item->userItemId	=	msg->readInt();
						item->id			=	msg->readInt();
						item->name			=	msg->readString();
						item->description	=	msg->readString();
						assets.push_back(item);					
				}
				globalLogicHandler->onEventItemsList(assets);
				break;
			}
			case 17: 
			{
				PASSERT2(false, "not handle this case");
				ubyte resul = msg->readByte();
				globalLogicHandler->onShowDialogResult(true, msg->readString());
				if (resul == 0) 
				{
					int money = msg->readInt();
					//DialogUtil.startOKDlg(L.gL(14) + money);
					//if (GameController.game.getScreen() instanceof BaseScreen)
					//	((BaseScreen)GameController.game.getScreen()).flyMoney(money);
				}
				break;
			}
			case 18: {
				// Vector lis = new Vector();
				// while (msg.available() > 0) {
				// Integer a = new Integer(msg.readInt());
				// lis.addElement(a);
				// }
				// globalLogicHandler.onListBg(lis);
				break;
			}
			case 19: {
				// //Láº¥y cac item cua mot nhan vat dang so huu.
				// //Tong so trang vat pham ma 1 user co, phan theo so luong
				// item toi da ma client co the hien thi.
				// //Vui long xem qua ham getUserItems().
				// int totalPage = msg.readInt();
				// //Tong so item duoc tra ve tu server.
				// int totalItem = msg.readInt();
				// Vector itemList = new Vector();
				// for (int i = 0; i < totalItem; i++) {
				// Item item = new Item(Item.ITEM);
				// item.id = msg.readInt();
				// item.name = msg.readString();
				// itemList.addElement(item);
				// }
				// globalLogicHandler.onShowItemsOnProfile(totalPage,
				// itemList);
				break;
			}
			}
			break;
		}
		case IwinProtocol::CHANGE_AVATAR:
		{
			globalLogicHandler->onChangeMyAvatar(msg->readInt());
			break;
		}
		case IwinProtocol::STRONGEST_LIST: {
			ubyte gameId = msg->readByte();
			ubyte page = msg->readByte();

			vector<Player * > list;
			static int numStrongest = 0;
			if (page < 1)
			{
				numStrongest = 0;
			}

			while (!msg->isEndRead())
			{
				// Player info = new TopPlayer();
				Player * info = new Player();
				info->IDDB = msg->readInt();
				info->setName(msg->readString());
				info->setMoney(msg->readLong());
				info->avatar = msg->readShort();
				info->setExp(msg->readInt());
				info->isReady = true; // Is online
				info->userProfile->status = msg->readString();
				info->giftID = numStrongest++;
				info->setRuby(msg->readLong());
				list.push_back(info);
			}
			globalLogicHandler->onReceivedStrongestList(page, list);
			break;
		}
		case IwinProtocol::RICHEST_LIST:
		{
			ubyte page = msg->readByte();
			vector<Player * > list;
			static int numRichest = 0;
			if (page < 1)
			{
				numRichest = 0;
			}
			while (!msg->isEndRead())
			{
				// Gdx.app.log("RICHEST_LIST", "Player " + numRichest);
				Player * p = new Player();
				p->IDDB = msg->readInt();
				p->setName(msg->readString());
				p->setMoney(msg->readLong());
				p->avatar = msg->readShort();
				p->setExp(msg->readInt());
				p->isReady = true; // Is online
				p->userProfile->status = msg->readString();
				ubyte romId = msg->readByte();
				ubyte boardId = msg->readByte();
				ubyte gameID = msg->readByte();
				p->onlineInfo = createOnlineInfo(romId, boardId, gameID);
				p->giftID = numRichest++;
				list.push_back(p);
			}
			globalLogicHandler->onReceivedRichestWinList(page, list);
			break;
		}
		case IwinProtocol::RICHEST_RUBY_LIST: {
			int page = msg->readInt();
			vector<Player *> list;
			static int numRichest = 0;
			if (page < 1)
			{
				numRichest = 0;
			}
			while (!msg->isEndRead())
			{
				Player * p = new Player();
				p->IDDB = msg->readInt();
				p->setName(msg->readString());
				p->setRuby(msg->readLong());
				p->avatar = msg->readShort();
				p->setExp(msg->readInt());
				p->userProfile->status = msg->readString();
				p->giftID = numRichest++;
				list.push_back(p);
			}
			globalLogicHandler->onReceivedRichestRubyList(page, list);
			break;
		}
		case IwinProtocol::CHANGE_PASSWORD:
		{
			bool isOK = msg->readBool();
			std::string desc = msg->readString();

			globalLogicHandler->onReceivedChangePass(isOK, desc);
			
			break;
		}
		case IwinProtocol::GET_PASSWORD:
		{
			bool isSuccess = msg->readBool();
			std::string mess = msg->readString();
			globalLogicHandler->onShowDialogResult(isSuccess, mess);

			break;
		}
		case IwinProtocol::REQUEST_AVATARLIST:
		{

		}
		break;
		case IwinProtocol::GET_STATISTIC_WAITING_BOARD_USER:
		{
			int countUserWait = msg->readInt();
			int countRoomWait = msg->readInt();
			ubyte countGame = msg->readByte();
			std::map<int , GameObjectData*> listGameWait;
			for (int i = 0; i < countGame; i++) {
				GameObjectData* data = new GameObjectData();
				data->gameId = msg->readByte();
				data->user = msg->readInt();
				data->room = msg->readInt();

				listGameWait.insert(std::pair<int, GameObjectData*>(data->gameId,data));
			}

			globalLogicHandler->onReceiveListWait(countRoomWait, countUserWait, listGameWait);
		}
		break;
		case IwinProtocol::ENABLE_RUBY:
		{
			bool enableRuby = msg->readBool();
			bool enableCashOut = msg->readBool();
			std::string message = "";
			if (msg->isEndRead())
			{
				message = msg->readString();
			}

			GameController::enableRuby = enableRuby;

			GameController::enableCashOut = enableCashOut;

			globalLogicHandler->onReceiveEnableRuby(enableRuby, enableCashOut, message);
		}
		break;
		case IwinProtocol::VERIFY_USER:
		{
			int type_verify = msg->readInt();
			bool is_success = msg->readBool();
			std::string descript = msg->readString();
			globalLogicHandler->onVerifyUser(type_verify, is_success, descript);
			break;
		}
		case IwinProtocol::CONFIRM_SMS_CODE:
		{
			bool is_succ = msg->readBool();
			std::string description = msg->readString();
			globalLogicHandler->onReceiveConfirmSMSCode(is_succ, description);
			break;
		}
		case IwinProtocol::TRANSFER_MONEY:
		{
			s64 newMoney = msg->readLong();
			std::string info = msg->readString();
			int moneyType = BASE_MONEY;//mac dinh la win 
			moneyType = msg->readInt();
			globalLogicHandler->onTransferMoney(newMoney, info, moneyType);
			break;
		}
		case IwinProtocol::SEARCH:
		{
			std::vector<Player*> searchList ;			
			while (!msg->isEndRead())
			{
				Player *p = new Player();
				p->IDDB = msg->readInt();
				p->setName(msg->readString());
				searchList.push_back(p);
			}
			globalLogicHandler->onSearchResult(searchList);
			break;
		}
		case IwinProtocol::FRIEND:
		{
			//DialogUtil.hideWaitDialog();
			//ScrMgr
			int id = msg->readByte();
			switch (id) 
			{
				case 1: // yeu cau ket ban
				{
					globalLogicHandler->onAddFriendResult(msg->readByte());
					break;
				}
				case 2: // xac nhan yeu cau ket ban
				{
					globalLogicHandler->onFriendConfirmationResult(msg->readString());
					break;
				}
				case 3: // tu choi ket ban
				{
					globalLogicHandler->onShowDialogResult(true, msg->readString());
					break;
				}
				case 4: // Danh sach cho ket ban
				{
					std::vector<IwinMessage*> arrMessage ;
					while (!msg->isEndRead()) {
						IwinMessage * m = new IwinMessage();
						m->fromID = msg->readInt();
						m->fromName = msg->readString();
						std::string str = LangMgr->GetString("_want_to_make_friends").GetString();
						m->message = m->fromName + str;
						m->typeN = 10;// truoc 249 la 1 sau nay doi thanh 10
						arrMessage.push_back(m);					
					}
					globalLogicHandler->onRequestAddFriend(arrMessage);
					break;
				}
				case 5:
				{
					std::vector<Player*> friendList ;
					while (!msg->isEndRead()) 
					{
						
						Player * info = new Player();
						info->IDDB = msg->readInt();
						info->setName(msg->readString());
						info->setMoney(msg->readLong());
						info->avatar = msg->readShort();
						ubyte onli = msg->readByte();
						info->isReady = (onli != 0);
						if (onli == 1) 
						{
								// doc thong tin, chi cÃ³ tá»« version 2.4.3
								// [roomId (byte), boardId(byte), gameId (byte)
								// ]
								info->roomID = msg->readByte();
								info->boardID = msg->readByte();
								info->gameID = msg->readByte();
								// từ 4.4.6 có
								info->hasBoardPassword = msg->readBool();
								// ///////////////////////////////////////////////
								info->onlineInfo = createOnlineInfo(info->roomID, info->boardID, info->gameID);
								info->userProfile->status = msg->readString();
						}
						else 
						{
								info->userProfile->status = msg->readString();
						}
						friendList.push_back(info);						
					}
					globalLogicHandler->onFriendList(friendList);
					break;
				}
				case 8: 
				{
					// Ket qua mua qua cho ban
					int res = msg->readInt();
					globalLogicHandler->onShowDialogResult(true, LANG_STR("buy_success").GetString());
					break;
				}
				case 9: 
				{
					auto res = msg->readShort();
					globalLogicHandler->onShowDialogResult(true, LANG_STR("buy_success").GetString());			
					break;
				}
				case 13:
				{
					// Ket qua gui tin nhan
					auto res = msg->readByte();
					if (res == 1)
					{
						globalLogicHandler->onShowDialogResult(true, LANG_STR("tin_nhắn_đã_được_gửi_đi").GetString());
					}
					else 
					{
						std::string str = msg->readString();
						globalLogicHandler->onShowDialogResult(true, str);
					}
					break;
				}
				case 6:
				{
					// minh moi nguoi ta lam ban
					std::vector<Player*> list ;

					while (!msg->isEndRead()) 
					{
						Player * p = new Player();
						p->IDDB = msg->readInt();
						p->setName(msg->readString());
						list.push_back(p);
					}
					globalLogicHandler->onListInvite(list);
					break;
				}
				case 14:
				{
					// ng ta moi minh lam ban
					std::vector<Player*> list;

					while (!msg->isEndRead()) 
					{
						Player * p = new Player();
						p->IDDB = msg->readInt();
						p->setName(msg->readString());
						p->avatar = msg->readInt();
						list.push_back(p);
					}
					globalLogicHandler->onListWaitingFriend(list);
					break;
				}
				case 15: 
				{		// Ket qua yeu cau ket ban
						  // 0 gửi yêu cầu kết bạn thành công
						  // 1 đã có trong danh sách bạn
						  // 2 đã full bạn
					auto res = msg->readByte();
					std::string str = msg->readString();
					globalLogicHandler->onShowDialogResult(true, str);
					break;
				}
				case 16:
				{
					std::vector<Player*> richFriendList;
	
					int count = 0;
					while (!msg->isEndRead())
					{
							Player * info = new Player();
							info->IDDB = msg->readInt();
							info->setName(msg->readString());
							// info.group = msg.readInt();
							info->setMoney(msg->readLong());
							info->avatar = msg->readShort();
							ubyte onli = msg->readByte();
							info->isReady = (onli != 0);
							info->giftID = count++;
							if (onli == 1) 
							{
								// doc thong tin, chi cÃ³ tá»« version 2.4.3
								// [roomId (byte), boardId(byte), gameId (byte)
								// ]
								info->roomID = msg->readByte();
								info->boardID = msg->readByte();
								info->gameID = msg->readByte();
								info->onlineInfo = createOnlineInfo(info->roomID, info->boardID, info->gameID);
								info->userProfile->status = msg->readString();
							}
							else 
							{
								info->userProfile->status = msg->readString();
							}
							info->setRuby(msg->readLong());
							richFriendList.push_back(info);
					}
					globalLogicHandler->onReceivedLeaderboardFriendList(0 ,richFriendList);
					break;
				}
				case 20:
				{
					std::vector<Player*> strongFriendList;

					int count = 0;
					while (!msg->isEndRead()) 
					{
						Player * info = new Player();
						info->IDDB = msg->readInt();
						info->setName(msg->readString());
							// info.group = msg.readInt();
						info->setMoney(msg->readLong());
						info->avatar = msg->readShort();
						ubyte onli = msg->readByte();
						info->isReady = (onli != 0);
						info->giftID = count++;
						if (onli == 1) 
						{
							info->setExp(msg->readInt());
								// doc thong tin, chi cÃ³ tá»« version 2.4.3
								// [roomId (byte), boardId(byte), gameId (byte)
								// ]
							info->roomID = msg->readByte();
							info->boardID = msg->readByte();
							info->gameID = msg->readByte();
							info->onlineInfo = createOnlineInfo(info->roomID, info->boardID, info->gameID);
							info->userProfile->status = msg->readString();
						}
						else 
						{
							info->userProfile->status = msg->readString();
						}
						info->setRuby(msg->readLong());
						strongFriendList.push_back(info);
					}
					globalLogicHandler->onReceivedLeaderboardFriendList(2 ,strongFriendList);
					break;
				}
				case 18:
				{
					break;
				}
			}
			break;
		}
		default:
			GetGameMessageHandler()->onMessage(msg);

		}

	}
	/*catch (const std::exception&)
	{

	}*/


}

void GlobalMessageHandler::onConnectOk()
{
	GlobalService::onSendClientInfo();
	//chien todo: after login and change to mainscreen will get info
	// Lay link de lay avatar (version 4.3.3 tro ve truoc).
	GlobalService::getInfo(19);
	GlobalService::getHeartBeatConfiguration();
	if (!GetGuideService()->getIsLoadDataFromServerOK()) {
		GetGuideService()->requestAllHelp();
	}
	//    if (!ReviewService.getInstance().isLoadDataFromServerOK) {
	//        ReviewService.getInstance().requestDataFromServer();
	//    }
	//    if (GameController.screenManager.mainScreen != null) {
	//        GameController.screenManager.mainScreen.requestListWait();
	//    }
	//    CoreServices.getInstance().setBackgroundMessageHandler(BackgroundMessageHandler.getInstance());
}

void GlobalMessageHandler::onConnectFail()
{

}

void GlobalMessageHandler::onDisConnect()
{
	globalLogicHandler->onDisConnect();
}
