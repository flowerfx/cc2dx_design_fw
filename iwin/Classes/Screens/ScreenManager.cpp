#include "ScreenManager.h"
#include <iostream>
#include "../MainGame.h"
#include "Common/Common.h"

#include "LoginScreen.h"
#include "MainScreen.h"
#include "ProfileScreen.h"
#include "LobbyScreen.h"
#include "NewLobbyScreen.h"
#include "DialogScreen.h"
#include "LoadingScreen.h"
#include "HUDScreen.h"
#include "LanguageMgr.h"
#include "LeaderboardScreen.h"
#include "IAPScreen.h"
#include "EventScreen.h"
#include "ChangeAwardScreen.h"
#include "CommonScreen.h"
#include "FriendScreen.h"

#include "GamePlayMgr.h"
#include "GameScreens/GameScreens.h"

#include "Network/Global/Globalservice.h"
#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"
#include "Network/JsonObject/cashoutruby/PaymentResult.h"
#include "Network/JsonObject/Quest/ScreenID.h"

#include "FileManager.h"
#include "Network/IwinCommand.h"
#include "Models/ChatMessage.h"
#include "Models/Item.h"
#include "Network/JsonObject/taixiu/TaiXiuUnsupportedList.h"
#include "Network/JsonObject/Quest/ScreenID.h"
using namespace iwinmesage;
ScreenManager * ScreenManager::m_Instance = NULL;

ScreenManager::ScreenManager()
{
	RegisterMenuUI();

	p_IsPopUpAppear = false;
	p_gameplay_mgr = nullptr;
	p_avatar_default = nullptr;

	p_mark_deinit_gamescreen = false;
	p_common_screen = nullptr;
	p_shop_screen = nullptr;
	p_change_award_screen = nullptr;

	p_gamelist_detail.clear();
	p_user = nullptr;

	p_table_screen_load.clear();
	p_texture_have_load.clear();

	p_taixiu_unsupportlist = nullptr;
	btn_taixiu_dirty = false;
	p_have_screen_id_dirty = false;
    p_have_connect_server= false;
}

ScreenManager::~ScreenManager()
{
	pListLayer.clear();
	if (p_gameplay_mgr)
	{
		delete p_gameplay_mgr;
		p_gameplay_mgr = nullptr;
		p_avatar_default = nullptr;
	}

	p_mark_deinit_gamescreen = false;
	p_common_screen = nullptr;
	p_gamelist_detail.clear();

	//weak pointer do not delete
	p_user = nullptr;

	p_table_screen_load.clear();

	if (p_taixiu_unsupportlist)
	{
		delete p_taixiu_unsupportlist;
	}
	p_taixiu_unsupportlist = nullptr;
}

int  ScreenManager::Init()
{
	p_texture_have_load.push_back("frame_sheet");
	return 1;
}

int	ScreenManager::InitScreen(int state)
{
	if (state == 0)
	{
		//loop throught all the menu and init its
		for (int i = DIALOG_SCREEN; i < MAIN_SCREEN; i++)
		{
			auto val = s_factory_menu_entity.CreateNewProduct((MENU_LAYER)i);
			if (val)
			{
				val->Init();
				pListLayer.insert(std::pair<MENU_LAYER, BaseScreen*>((MENU_LAYER)i, val));
			}
		}

		//SwitchToMenu(LOGIN_SCREEN);
	}
	else if (state == 1)
	{
		//loop throught all the menu and init its
		for (int i = MAIN_SCREEN; i < MENU_COUNT; i++)
		{
			auto val = s_factory_menu_entity.CreateNewProduct((MENU_LAYER)i);
			if (val)
			{
				val->Init();
				pListLayer.insert(std::pair<MENU_LAYER, BaseScreen*>((MENU_LAYER)i, val));
			}
		}

		p_common_screen = GetMenuUI(MENU_LAYER::COMMON_SCREEN);
		//push to the top screen logic cocos2dx
		p_common_screen->push_layer_to_main_scene(100);

		p_shop_screen = GetMenuUI(MENU_LAYER::IAP_SCREEN);
		p_shop_screen->push_layer_to_main_scene(98);
		p_shop_screen->SetIdxMenuData(0);

		p_change_award_screen = GetMenuUI(MENU_LAYER::CHANGE_AWARD_SCREEN);
		p_change_award_screen->push_layer_to_main_scene(99);
		p_change_award_screen->SetIdxMenuData(0);
		ShowShop(false);
		ShowChangeAward(false);
		//SwitchToMenu(MAIN_SCREEN);
	}
	return 1;
}

int	ScreenManager::InitDetailScreen(int id_screen)
{
	if (id_screen < (int)LOADING_SCREEN || id_screen >(int)COMMON_SCREEN)
	{
		PASSERT2(false, "id out of stack");
		return -1;
	}

	auto val = s_factory_menu_entity.CreateNewProduct((MENU_LAYER)id_screen);
	if (val)
	{
		val->Init();
		pListLayer.insert(std::pair<MENU_LAYER, BaseScreen*>((MENU_LAYER)id_screen, val));
		if (id_screen == MENU_LAYER::COMMON_SCREEN)
		{
			p_common_screen = GetMenuUI(MENU_LAYER::COMMON_SCREEN);
			//push to the top screen logic cocos2dx
			p_common_screen->push_layer_to_main_scene(100);
		}
		else if (id_screen == MENU_LAYER::IAP_SCREEN)
		{
			p_shop_screen = GetMenuUI(MENU_LAYER::IAP_SCREEN);
			p_shop_screen->push_layer_to_main_scene(98);
			p_shop_screen->SetIdxMenuData(0);
			ShowShop(false);
		}
		else if (id_screen == MENU_LAYER::CHANGE_AWARD_SCREEN)
		{
			p_change_award_screen = GetMenuUI(MENU_LAYER::CHANGE_AWARD_SCREEN);
			p_change_award_screen->push_layer_to_main_scene(99);
			p_change_award_screen->SetIdxMenuData(0);
			ShowChangeAward(false);
		}

		p_table_screen_load.push_back(id_screen);

		return id_screen;
	}
	return -1;
}

BaseScreen * ScreenManager::GetGameScreen()
{
	if (!p_gameplay_mgr)
	{
		//PASSERT2(p_gameplay_mgr != nullptr, "game screen not init");
		//return nullptr;
		PINFO("the game screen not init , so init its!")
		GameType current_gameplay = GameController::getCurrentGameType();
		InitGamePlay(current_gameplay);
	}
	return p_gameplay_mgr->GetCurrentGameScreen();
}

void ScreenManager::Update(float dt)
{
	for (int i = p_CurretUIActive.size() - 1; i >= 0; i--)
	{
		if (p_CurretUIActive.at(i) == HUD_SCREEN)
		{
			if (p_gameplay_mgr)
			{
				p_gameplay_mgr->GetCurrentGameScreen()->SetIdxMenuData(0);
				p_gameplay_mgr->GetCurrentGameScreen()->Update(dt);
			}
			continue;
		}
		auto ui = pListLayer.at(p_CurretUIActive.at(i));
		ui->SetIdxMenuData(i);
		ui->Update(dt);
	}

	if (p_common_screen)
	{
		p_common_screen->Update(dt);
	}

	if (p_shop_screen && p_shop_screen->isVisible())
	{
		p_shop_screen->Update(dt);
	}

	if (p_mark_deinit_gamescreen)
	{
		DeInitGamePlay();
		p_mark_deinit_gamescreen = false;
	}

	if (btn_taixiu_dirty)
	{
		updateButtonTaiXiu();
		btn_taixiu_dirty = false;
	}

	if (p_have_screen_id_dirty)
	{
		int current_id = (int)ConvertCurrentMenuToScreenID();
		if (current_id > 0)
		{
			GlobalService::sendScreenIdToServer(current_id);
			this->setBtnTaixiuDirty();
		}
		p_have_screen_id_dirty = false;
	}
    
    if(p_have_connect_server)
    {
        auto p_username = static_cast<LoginScreen*>(GetMenuUI(LOGIN_SCREEN))->GetUsername();
        auto p_password = static_cast<LoginScreen*>(GetMenuUI(LOGIN_SCREEN))->GetPassword();
        
        GlobalService::login(p_username.GetString(), p_password.GetString());
        PWARN1("Conect OK and login with user and password");
        p_have_connect_server = false;
        
    }
}

void ScreenManager::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	//dont need this
	/*for (int i = p_CurretUIActive.size() - 1; i >= 0; i--)
	{
		pListLayer.at(p_CurretUIActive.at(i))->VisitMenu(renderer, transform, flags);
	}*/
}

void ScreenManager::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	//dont need this
	/*for (int i = p_CurretUIActive.size() - 1; i >= 0; i--)
	{
		pListLayer.at(p_CurretUIActive.at(i))->DrawMenu(renderer, transform, flags);
	}*/
}

void ScreenManager::SwitchToMenu(MENU_LAYER m, MENU_LAYER from_menu)
{
	if (m != MENU_LAYER::MENU_NONE)
	{
        if(from_menu == MENU_LAYER::MENU_NONE)
        {
			ShowCurrentMenu(m, (int)from_menu);
        }

		if (from_menu!= MENU_LAYER::MENU_NONE && GetMenuUI(from_menu)->GetMenuComeFrom() != m)
		{
			GetMenuUI(m)->SetMenuComFrom(from_menu);
		}
		else if (from_menu == MENU_LAYER::MENU_NONE && GetMenuUI(m)->GetMenuComeFrom() != MENU_LAYER::MENU_NONE)
		{
			GetMenuUI(m)->SetMenuComFrom(from_menu);
		}
        GetMenuUI(m)->SetLayerInteractive(true);
	}
	//
	if (from_menu != MENU_LAYER::MENU_NONE)
	{
		GetMenuUI(from_menu)->OnHide();
        GetMenuUI(from_menu)->SetMenuShowNext(m);
	}
}

void ScreenManager::CloseCurrentMenu(MENU_LAYER m)
{
    auto next_menu = GetMenuUI(m)->GetMenuShowNext();
    ShowCurrentMenu(next_menu, 0);
	DeActiveUI(m);
}

void ScreenManager::ShowCurrentMenu(MENU_LAYER m, int idx)
{
    if(m == MENU_LAYER::MENU_NONE)
    {
        return;
    }
	SetActiveUI(m, idx);
    GetMenuUI(m)->OnShow();
	btn_taixiu_dirty = true;
}

void ScreenManager::SetActiveUI(MENU_LAYER layer, int idx)
{
	if (idx == -1)
		idx = p_CurretUIActive.size();
	else
		idx = 0;
    if(p_CurretUIActive.size() > 0 && p_CurretUIActive.at(idx) == layer)
    {
        return;
    }
    
	if (p_CurretUIActive.size() > 1 && p_CurretUIActive.at(0) == DIALOG_SCREEN)
	{
		PASSERT2(false, "have bug here!");
		p_CurretUIActive.erase(p_CurretUIActive.begin() + 0);
	}

    p_CurretUIActive.insert(p_CurretUIActive.begin() + idx , layer);
}

void ScreenManager::DeActiveUI(MENU_LAYER layer)
{
	int idx = -1;
	for (size_t i = 0; i < p_CurretUIActive.size(); i++)
	{
		if (p_CurretUIActive.at(i) == layer)
		{
			idx = (int)i;
			break;
		}
	}
	if (idx > -1)
	{
		//GetMenuUI(layer)->SetMenuComFrom(MENU_LAYER::MENU_NONE);
		p_CurretUIActive.erase(p_CurretUIActive.begin() + idx);
		if (p_CurretUIActive.size() > 0 && !static_cast<CommonScreen*>(p_common_screen)->HaveLayoutInter())
		{
			GetMenuUI(p_CurretUIActive.at(0))->SetLayerInteractive(true);
		}
	}
}

void ScreenManager::PushMenuToTop(MENU_LAYER layer)
{
	int idx_contain = -1;
	for (size_t i = 0; i < p_CurretUIActive.size(); i++)
	{
		if (p_CurretUIActive.at(i) == layer)
		{
			idx_contain = i;
			break;
		}
	}

	if (idx_contain == -1)
		return;

	//remove in stack
	p_CurretUIActive.erase(p_CurretUIActive.begin() + idx_contain);

	//add to top of stack
	p_CurretUIActive.insert(p_CurretUIActive.begin(), layer);
}

MENU_LAYER ScreenManager::GetCurrentMenuUI(int idx)
{
	if (p_CurretUIActive.size() > 0)
	{
		if ((size_t)idx < p_CurretUIActive.size())
		{
			return p_CurretUIActive.at(idx);
		}
		else
		{
			PASSERT2((size_t)idx < p_CurretUIActive.size(), "idx out of layer stack");
			return p_CurretUIActive.at(0);
		}

	}
	else
	{
		return MENU_LAYER::MENU_NONE;
	}
}

void ScreenManager::RegisterMenuUI()
{
	s_factory_menu_entity.RegisterProduct<LoginScreen>(MENU_LAYER::LOGIN_SCREEN);
	s_factory_menu_entity.RegisterProduct<MainScreen>(MENU_LAYER::MAIN_SCREEN);
	s_factory_menu_entity.RegisterProduct<ProfileScreen>(MENU_LAYER::PROFILE_SCREEN);
	s_factory_menu_entity.RegisterProduct<LobbyScreen>(MENU_LAYER::LOBBY_SCREEN);
	//s_factory_menu_entity.RegisterProduct<DialogScreen>(MENU_LAYER::DIALOG_SCREEN);
	s_factory_menu_entity.RegisterProduct<LoadingScreen>(MENU_LAYER::LOADING_SCREEN);
	s_factory_menu_entity.RegisterProduct<LeaderboardScreen>(MENU_LAYER::LEADERBOARD_SCREEN);
	s_factory_menu_entity.RegisterProduct<IAPScreen>(MENU_LAYER::IAP_SCREEN);
	s_factory_menu_entity.RegisterProduct<EventScreen>(MENU_LAYER::EVENT_SCREEN);
	s_factory_menu_entity.RegisterProduct<NewLobbyScreen>(MENU_LAYER::NEW_LOBBY_SCREEN);
	s_factory_menu_entity.RegisterProduct<ChangeAwardScreen>(MENU_LAYER::CHANGE_AWARD_SCREEN);
	s_factory_menu_entity.RegisterProduct<CommonScreen>(MENU_LAYER::COMMON_SCREEN);
	s_factory_menu_entity.RegisterProduct<FriendScreen>(MENU_LAYER::FRIEND_SCREEN);
}

BaseScreen* ScreenManager::GetMenuUI(MENU_LAYER name)
{
	if (name == MENU_LAYER::HUD_SCREEN)
	{
		return GetGameScreen();
	}

	if (pListLayer.size() > 0 && pListLayer.find(name) != pListLayer.end())
	{
		return pListLayer.at(name);
	}
	return 0;
}

BaseScreen* ScreenManager::GetMenuUIByName(RKString name)
{
	if (name == "game_screen" || name=="hud")
	{
		return GetGameScreen();
	}

	if (pListLayer.size() > 0)
	{
		for (auto it = pListLayer.begin(); it != pListLayer.end(); it++)
		{
			if (it->second->GetNameUI() == name)
			{
				return it->second;
			}
		}
	}
	return 0;
}

void ScreenManager::OnShowDialog(RKString title ,
	RKString str ,
	TYPE_DIALOG type ,
	const char * strCB ,
	const std::function<void(const char * str, const char* btn_name)> & func,
	const std::function<void(const char * str, const char* btn_name)> & func1,
	const std::function<void(const char * str, const char* btn_name)> & func2, 
	RKString title_1 , RKString title_2 )
{
	/*
	static_cast<DialogScreen *>(GetMenuUI(DIALOG_SCREEN))->OnShowDialog(
		title, 
		str, 
		type, 
		strCB, 
		func,
		func1,
		func2,
		title_1,
		title_2
		);
    SetActiveUI(DIALOG_SCREEN , 0); //push to top stack sreen
	*/

	static_cast<CommonScreen*>(p_common_screen)->OnShowDialog(
		title,
		str,
		type,
		strCB,
		func,
		func1,
		func2,
		title_1,
		title_2);
}

void ScreenManager::OnHideDialog()
{
	/*
	if (GetCurrentMenuUI(0) == DIALOG_SCREEN)
	{
		GetMenuUI(DIALOG_SCREEN)->OnHide();
	}
	*/
	static_cast<CommonScreen*>(p_common_screen)->OnHideDialog();
//	DeActiveUI(DIALOG_SCREEN);
}

bool ScreenManager::ChangeLanguage(LANGUAGE_TYPE lang)
{
	bool result = LangMgr->SetNewLanguage(lang);
	if (result)
	{
		for (auto it = pListLayer.begin(); it != pListLayer.end(); it++)
		{
			it->second->onLanguageChanged(lang);
		}
	}
	return result;
}

int ScreenManager::GetStateLoading()
{
	return static_cast<LoadingScreen*>(GetMenuUI(MENU_LAYER::LOADING_SCREEN))->GetStateLoading();
}

void ScreenManager::SetStateLoading(int state)
{
	static_cast<LoadingScreen*>(GetMenuUI(MENU_LAYER::LOADING_SCREEN))->SetStateLoading(state);
}

GamePlayMgr * ScreenManager::GetGamePlayMgr()
{
	return p_gameplay_mgr;
}

void ScreenManager::SwithToGameScreen(GameType game_type, int max_player)
{
	SwitchToMenu(HUD_SCREEN, GetCurrentMenuUI());
}

void ScreenManager::LoadTextureAsGame(int id_game)
{
	auto detail_game = GetGameDetailByType((GameType)id_game);
	if (detail_game.id != GameType::NULL_TYPE)
	{
		bool contain_frame_sheet = false;
		for (auto str : p_texture_have_load)
		{
			if (str == "frame_sheet")
			{
				contain_frame_sheet = true;
				break;
			}
		}
		if (!contain_frame_sheet)
		{
			TextureMgr->LoadTextureFrame("frame_sheet");
			p_texture_have_load.push_back("frame_sheet");
		}

		auto list_texture_load = detail_game.texture_load;
		for (auto str : list_texture_load)
		{
			bool contain_tex = false;
			for (auto s : p_texture_have_load)
			{
				if (s == str)
				{
					contain_tex = true;
					break;
				}
			}
			if (!contain_tex)
			{
				TextureMgr->LoadTextureAtlas(str);
				p_texture_have_load.push_back(str);
			}
		}

	}
}

void ScreenManager::InitGamePlay(int game_id)
{
	if (p_gameplay_mgr)
	{
		delete p_gameplay_mgr;
		p_gameplay_mgr = nullptr;
	}
	p_gameplay_mgr = new GamePlayMgr();


	XMLMgr->OnLoadXMLData<Utility::xml::LoadDataXML>("CardDec",
				std::mem_fn(&Utility::xml::LoadDataXML::OnLoadListWideObjectDecXML), XMLMgr);

	LoadTextureAsGame(game_id);
		
	PASSERT2(p_gameplay_mgr != nullptr, "gameplay mgr must not be null here");
	p_gameplay_mgr->Init(game_id);
}

void ScreenManager::DeInitGamePlay()
{
	if (p_gameplay_mgr)
	{
		delete p_gameplay_mgr;
		p_gameplay_mgr = nullptr;
	}
	XMLMgr->ReleaseAllListWideObjectDecXML();
	GameController::currentGameScreen = nullptr;
}

void ScreenManager::SendDataToScreen(MENU_LAYER screen, int id,void *data, LIST_AGR())
{
	auto ui_screen = GetMenuUI(screen);
	PASSERT2(screen == MENU_LAYER::LOBBY_SCREEN || ui_screen != nullptr, "screen is null");
	switch (screen)
	{
		case MENU_LAYER::MAIN_SCREEN:
		{
			if (id == -1)
			{
				if (RKString((const char*)data) == "GET_GAME_LIST")
				{
					static_cast<MainScreen*>(ui_screen)->onReceivedGameList(data1);
				}
				else if (RKString((const char *)data) == "GET_AVA_CAT")
				{
					static_cast<MainScreen*>(ui_screen)->onReceiveAvatarCatList(data1);
				}
				else if (RKString((const char *)data) == "GET_AVA")
				{
					static_cast<MainScreen*>(ui_screen)->onReceiveAvatarList(data1);
				}
				else if (RKString((const char *)data) == "BUY_AVA")
				{
					static_cast<MainScreen*>(ui_screen)->onBuyingAvatarResult(data1);
				}
				else if (RKString((const char*)data) == "GET_STATISTIC_WAITING_BOARD_USER")
				{
					static_cast<MainScreen*>(ui_screen)->onReceiveListWait(data1, data2, data3);
				}
				else if (RKString((const char*)data) == "COUNT_QUEST_LIST")
				{
					static_cast<MainScreen*>(ui_screen)->SetNumWinfree(*(int*)data1);
				}
				else if (RKString((const char *)data) == "UPDATE_UNREAD_MSG")
				{
					static_cast<MainScreen*>(ui_screen)->SetNumUnReadMsg(*(int*)data1);
				}
				else if (RKString((const char *)data) == "SERVER_MSG")
				{
					static_cast<MainScreen*>(ui_screen)->SetMsgServer((const char *)data1);
				}
			}
			else if (id == 19)
			{
				
			}
			else if (id == IwinProtocol::SERVER_INFO)
			{
				RKString msg = (const char*)data;
				static_cast<MainScreen*>(ui_screen)->OnReceiveAnnouncementFromServer(msg);
			}
			else if (id == IwinProtocol::LOGIN_SUCESS)
			{
				static_cast<MainScreen*>(ui_screen)->onLoginSuccess();
			}
			else
			{

			}

			break;
		}
		case MENU_LAYER::PROFILE_SCREEN:
		{
			if (id == 19)
			{
				
			}
			else if (id == IwinProtocol::USER_DATA)
			{
				static_cast<ProfileScreen*>(ui_screen)->onSetAchievement(data1);
			}
			else if (id == -1)
			{
				if (RKString((const char *)data) == "GET_AVA_CAT")
				{
					static_cast<ProfileScreen*>(ui_screen)->onReceiveAvatarCatList(data1);
				}
				else if (RKString((const char *)data) == "GET_AVA")
				{
					static_cast<ProfileScreen*>(ui_screen)->onReceiveAvatarList(data1);
				}
				else if (RKString((const char *)data) == "BUY_AVA")
				{
					static_cast<ProfileScreen*>(ui_screen)->onBuyingAvatarResult(data1);
				}
				else if (RKString((const char *)data) == "CHANGE_AVATAR_RESULT")
				{
					static_cast<ProfileScreen*>(ui_screen)->onChangeMyAvatar();
				}
				else if (RKString((const char *)data) == "EVENT_ITEMS_LIST")
				{
					static_cast<ProfileScreen*>(ui_screen)->onEventItems(*((std::vector<Item*>*)data1));
				}
				else if (RKString((const char *)data) == "BOUGHT_ITEMS")
				{
					static_cast<ProfileScreen*>(ui_screen)->onMyItems(*((std::vector<Item*>*)data1),*((int*)data2), *((int*)data3), *((unsigned char*)data2));
				}
				else if (RKString((const char *)data) == "ON_MY_AVATAR")
				{
					static_cast<ProfileScreen*>(ui_screen)->onMyAvatars(*((std::vector<Item*>*)data1));
				}
			}
			break;
		}
		case MENU_LAYER::NEW_LOBBY_SCREEN:
		{
			if (id == 0)
			{
				static_cast<NewLobbyScreen*>(ui_screen)->onSetGameTypeSuccessed();
			}
			else if (id == 1 )
			{
				static_cast<NewLobbyScreen*>(ui_screen)->onReceiveLobbyList((LobbyList*)data);
			}
			else if (id == 2)
			{
				static_cast<NewLobbyScreen*>(ui_screen)->onReceiveOwnerBoardList(*(std::vector<BoardOwner>*)data1, to_data(int, data2), to_data(int, data3), to_data(int, data4), to_data(int, data5));
			}
			else if (id == 3)
			{
				static_cast<NewLobbyScreen*>(ui_screen)->onReceiveChatList((ChatLobbyList*)data);
			}		
			else if (id == 19)
			{
				
			}
			break;
		}
		case MENU_LAYER::LOBBY_SCREEN:
		{
			if (id == IwinProtocol::JOIN_BOARD)
			{

			}
			else if (id == IwinProtocol::SET_GAME_TYPE)
			{
				if (!ui_screen)
				{

				}
				else
				{
					static_cast<LobbyScreen*>(ui_screen)->onSetGameTypeSuccessed();
				}
			}
			else if (id == -1)
			{
				if (RKString((const char*)data) == "GET_OWNER_BOARD_LIST")
				{
					static_cast<LobbyScreen*>(ui_screen)->onReceiveOwnerBoardList(*((std::vector<BoardOwner>*)data1), to_data(int, data2), to_data(int, data3), to_data(int, data4), to_data(int, data5));
				}
				else if (RKString((const char*)data) == "SET_GAME_TYPE")
				{
					if (!ui_screen)
					{
						static_cast<LoadingScreen*>(GetMenuUI(LOADING_SCREEN))->SetStateServerReturn(1);
					}
					else
					{
						static_cast<LobbyScreen*>(ui_screen)->onSetGameTypeSuccessed();
					}
				}
			}
			else if (id == 19)
			{
				
			}
			break;
		}
		case MENU_LAYER::LOGIN_SCREEN:
		{
			if (id == IwinProtocol::GET_SMS_SYNTAX)
			{
				static_cast<LoginScreen*>(ui_screen)->onSmsSyntaxInfo(to_data(short, data1), to_str(data2), to_str(data3), to_str(data4));
			}
			else if (id == IwinProtocol::CHECK_USERNAME)
			{
				static_cast<LoginScreen*>(ui_screen)->onCallBackCheckUserName(to_str(data));
			}
			else if (id == -1)
			{
				if (RKString((const char*)data) == "GET_CAPTCHA")
				{
					static_cast<LoginScreen*>(ui_screen)->onReceiveCaptcha(*((CaptchaInfo*)data1));
				}
				else if (RKString((const char*)data) == "CHECK_CAPTCHA")
				{
					static_cast<LoginScreen*>(ui_screen)->onReceiveCaptchaResult(*((CaptchaResult*)data1));
				}
			}
			break;
		}
		case MENU_LAYER::HUD_SCREEN:
		{
			if (id == IwinProtocol::JOIN_BOARD)
			{
				if (!p_gameplay_mgr)
				{
					PASSERT2(p_gameplay_mgr != nullptr , "game screen not init yet!");
				}
				else
				{
					p_gameplay_mgr->GetCurrentGameScreen()->showGameScreen(to_data(short, data), to_data(short, data1), to_data(int, data2), to_data(int, data3), to_data(std::vector<Player*>, data4));
				}
			}
			if (id == IwinProtocol::GET_PROFILE)
			{
				static_cast<HUDScreen*>(ui_screen)->onSetData(data);
			}
			else if (id == -1)
			{
				if (RKString((const char *)data) == "GET_AVA_CAT")
				{
					static_cast<HUDScreen*>(ui_screen)->onReceiveAvatarCatList(data1);
				}
				else if (RKString((const char *)data) == "GET_AVA")
				{
					static_cast<HUDScreen*>(ui_screen)->onReceiveAvatarList(data1);
				}
				else if (RKString((const char *)data) == "BUY_AVA")
				{
					static_cast<HUDScreen*>(ui_screen)->onBuyingAvatarResult(data1);
				}
			}
			break;
		}
		case MENU_LAYER::LOADING_SCREEN:
		{
			if (id == -1)
			{
				SwitchToMenu(LOADING_SCREEN, LOGIN_SCREEN);
				SetStateLoading(11);
				static_cast<LoadingScreen*>(GetMenuUI(LOADING_SCREEN))->SetMessageHandle(data);
				static_cast<LoadingScreen*>(GetMenuUI(LOADING_SCREEN))->SetStateServerReturn(2);
			}
			break;
		}
		case MENU_LAYER::IAP_SCREEN:
		{
			if (id == -1)
			{
				if (RKString((const char*)data) == "GET_CAPTCHA")
				{
					static_cast<IAPScreen*>(ui_screen)->onReceiveCaptcha(*((CaptchaInfo*)data1));
				}
				else if (RKString((const char*)data) == "CHECK_CAPTCHA")
				{
					static_cast<IAPScreen*>(ui_screen)->onReceiveCaptchaResult(*((CaptchaResult*)data1));
				}
			}
			else if (id == 2)
			{
				static_cast<IAPScreen*>(ui_screen)->onReceivePayment((Payment*)data);
			}
			break;
		}
		case MENU_LAYER::EVENT_SCREEN:
		{
			if (id == -1)
			{
				const char * id_msg = (const char *)data;
				const char * json_msg = (const char *)data1;
				if (RKString(id_msg) == IwinCommand::GET_QUEST_CATEGORY_LIST)
				{
					static_cast<EventScreen*>(ui_screen)->OnReceiveListQuestCataloryJson(json_msg);
				}
				else if (RKString(id_msg) == IwinCommand::GET_QUESTS_BY_CATEGORY_ID)
				{
					static_cast<EventScreen*>(ui_screen)->OnReceiveQuestByCalaloryJson(json_msg);
				}
			}
			break;

		}
		case MENU_LAYER::LEADERBOARD_SCREEN:
		{
			if (id == IwinProtocol::STRONGEST_LIST || id == IwinProtocol::RICHEST_LIST || id == IwinProtocol::RICHEST_RUBY_LIST)
			{
				static_cast<LeaderboardScreen*>(ui_screen)->OnReceiveListPlayer(id, *((unsigned char*)data), *(std::vector<Player * >*)data1);
			}
			else if (id == -1)
			{
				if (RKString((const char*)data) == "LEADERBOARD_FRIEND")
				{
					int _id = *(int*)data1;
					static_cast<LeaderboardScreen*>(ui_screen)->OnReceiveListFriend(_id, *((unsigned char*)data2), *(std::vector<Player * >*)data3);
				}
			}
			break;
		}
		case MENU_LAYER::CHANGE_AWARD_SCREEN:
		{
			if (id == -1)
			{
				if (RKString((const char*)data) == "GET_CAPTCHA")
				{
					static_cast<ChangeAwardScreen*>(ui_screen)->onReceiveCaptcha(*((CaptchaInfo*)data1));
				}
				else if (RKString((const char*)data) == "CHECK_CAPTCHA")
				{
					static_cast<ChangeAwardScreen*>(ui_screen)->onReceiveCaptchaResult(*((CaptchaResult*)data1));
				}
			}
			else if (id == 1)
			{
				static_cast<ChangeAwardScreen*>(ui_screen)->onReceiveRubyToCard((RubyToVNDInfo*)data);
			}
			else if (id == 2)
			{
				PaymentResult* result = (PaymentResult*)data;
				if (result->getCode() < 0)
				{
					Utils::showDialogNotice(result->getMessage());
				}
			}
			else if (id == 3)
			{
				static_cast<ChangeAwardScreen*>(ui_screen)->onReceiveListHistory((PaymentHistoryList*)data);
			}
			else if (id == 4)
			{
				static_cast<ChangeAwardScreen*>(ui_screen)->onReceiveCancelCashout((CancelResult*)data);
			}
			break;
		}
		case MENU_LAYER::FRIEND_SCREEN:
		{
			if (id == 16)
			{
				static_cast<FriendScreen*>(ui_screen)->onReceiveServerWarning((char*)data);
			}
			break;
		}
	}
}

void ScreenManager::SetAvatarDefault(void * data, void * str)
{
	Image * img = (Image*)data;
	if (p_avatar_default)
	{
		CC_SAFE_DELETE(img);
		return;
	}
	p_avatar_default = GameMgr->CreateAvatarFromImg(data, str);
}

void ScreenManager::ForceLogOutToLoginScreen()
{
	MENU_LAYER current_screen = GetCurrentMenuUI();
	if (current_screen == MENU_LAYER::DIALOG_SCREEN)
	{
		PASSERT2(p_CurretUIActive.size() > 1, "something wrong !@");
		current_screen = p_CurretUIActive.at(1);
	}
	if (current_screen == LOGIN_SCREEN || current_screen == LOADING_SCREEN)
	{
		return;
	}
	static_cast<MainScreen*>(GetMenuUI(MAIN_SCREEN))->SetNeedToReloadFriend();
	GlobalService::sendSingleCommand(IwinProtocol::LOGOUT);
	GlobalService::CloseSocket();
	static_cast<MainScreen*>(GetMenuUI(MENU_LAYER::MAIN_SCREEN))->SetStateLoadData(1);
	static_cast<FriendScreen*>(GetMenuUI(MENU_LAYER::FRIEND_SCREEN))->CLEAR_LIST_WHEN_LOGOUT();
	SwitchToMenu(LOGIN_SCREEN, current_screen);

}

void ScreenManager::SaveDataInt(RKString name, int data)
{
	File::SaveMgr->GetDataSave()->SetIntByName(name, data);
}

bool  ScreenManager::GetDataSaveInt(RKString name, int & result)
{
	return File::SaveMgr->GetDataSave()->GetIntByName(name, result);
}

void ScreenManager::SaveDataFloat(RKString name, float data)
{
	File::SaveMgr->GetDataSave()->SetFloatByName(name, data);
}

bool ScreenManager::GetDataSaveFloat(RKString name , float & result)
{
	return File::SaveMgr->GetDataSave()->GetFloatByName(name , result);
}

void ScreenManager::SaveDataString(RKString name, RKString data)
{
	File::SaveMgr->GetDataSave()->SetCharByName(name, data);
}

bool ScreenManager::GetDataSaveString(RKString name , RKString & result)
{
	return File::SaveMgr->GetDataSave()->GetCharByName(name , result);
}

void ScreenManager::SaveTheGame()
{
	File::SaveMgr->SaveTheGameSave();
}

void ScreenManager::MyInfoChanged()
{
	for (auto it = pListLayer.begin(); it != pListLayer.end(); it++)
	{
		it->second->onMyInfoChanged();
	}
}

void ScreenManager::onReceiveFriendList()
{
	for (auto it = pListLayer.begin(); it != pListLayer.end(); it++)
	{
		it->second->OnReceiveFriendList();
	}
}

BaseScreen * ScreenManager::GetCommonScreen()
{
	return p_common_screen;
}

ListGameDetail ScreenManager::GetGameDetailByType(GameType type)
{
	if ((int)(p_gamelist_detail.size()) <= 0 || p_gamelist_detail.find(type) == p_gamelist_detail.end())
	{
		return p_gamelist_detail.at(GameType::NULL_TYPE);
	}
	return p_gamelist_detail.at(type);
}

ListGameDetail ScreenManager::GetGameDetailByIdx(int idx)
{
	if (idx < 0 || (size_t)idx >= p_gamelist_detail.size())
	{
		return ListGameDetail();
	}
	int count = 0;
	auto it = p_gamelist_detail.begin();
	for (; it != p_gamelist_detail.end(); it++)
	{
		if (count == idx)
		{
			return it->second;
		}
		count++;
	}
	return ListGameDetail();
}

int ScreenManager::GetSizeListGameDetail()
{
	return p_gamelist_detail.size();
}


void ScreenManager::LoadXMLListGame(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto root_list_game = Root->FirstChildElement("list_game");
	if (!root_list_game)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	p_gamelist_detail.insert(std::pair<GameType, ListGameDetail>(GameType::NULL_TYPE, ListGameDetail()));
	auto game_xml = root_list_game->FirstChildElement("game");
	do
	{
		if (game_xml)
		{
			GameType type = (GameType)atoi(game_xml->Attribute("id"));

			auto contain = game_xml->FirstChildElement("contain");
			PASSERT2(contain != nullptr, "error !");

			RKString name = contain->Attribute("name");
			RKString dec = contain->Attribute("dec");
			RKString str = contain->Attribute("str");
			RKString img = contain->Attribute("img");

			ListGameDetail game_detail(type, name, dec, str, img);

			std::vector<RKString> list_texture_load;
			auto tex_load_xml = game_xml->FirstChildElement("name_tex");
			do
			{
				if (tex_load_xml)
				{
					list_texture_load.push_back(tex_load_xml->Attribute("name"));
					tex_load_xml = tex_load_xml->NextSiblingElement("name_tex");
				}
			} while (tex_load_xml);

			game_detail.texture_load = list_texture_load;

			p_gamelist_detail.insert(std::pair<GameType, ListGameDetail>(type, game_detail));

			game_xml = game_xml->NextSiblingElement();
		}
	} while (game_xml);
}

User* ScreenManager::GetMyUser()
{
	return p_user;
}

void ScreenManager::onSetUser(void * data)
{

	p_user = ((User*)data);
	MyInfoChanged();
}

void ScreenManager::showScreen(int screenId, int gameId)
{
	iwinmesage::SCREEN_ID cur_id = (SCREEN_ID)screenId;
	switch (cur_id)
	{
	case iwinmesage::SCREEN_ID::SCREEN_MAIN_MENU:
	{
		//ScrMgr->SwitchToMenu()
	}
	break;
	case iwinmesage::SCREEN_ID::SCREEN_PROFILE:
	{
		ScrMgr->SwitchToMenu(MENU_LAYER::PROFILE_SCREEN, ScrMgr->GetCurrentMenuUI());
	}
	break;
	}
}

std::vector<ChatMessage*> ScreenManager::GET_MESSAGE(int state)
{
	return static_cast<FriendScreen*>(GetMenuUI(FRIEND_SCREEN))->GET_MESSAGE(state);
}

void  ScreenManager::PUSH_MESSAGE(int state, std::vector<ChatMessage*> vec)
{
	static_cast<FriendScreen*>(GetMenuUI(FRIEND_SCREEN))->PUSH_MESSAGE(state , vec);
}

void ScreenManager::ShowShop(bool visible)
{
	p_shop_screen->setVisible(visible);
	p_shop_screen->SetLayerInteractive(visible);
	if (visible) 
	{ 
		p_shop_screen->OnBeginFadeIn(); 
		ScrMgr->SetScreenIDDirty();
	}
	else 
	{
		p_shop_screen->OnBeginFadeOut(); 
	}
}

void ScreenManager::ShowChangeAward(bool visible)
{
	p_change_award_screen->setVisible(visible);
	p_change_award_screen->SetLayerInteractive(visible);
	if (visible)
	{
		p_change_award_screen->OnBeginFadeIn();
		ScrMgr->SetScreenIDDirty();
	}
	else
	{
		p_change_award_screen->OnBeginFadeOut();
	}
}

std::vector<int> ScreenManager::GetTableScreenLoad()
{
	return p_table_screen_load;
}

bool ScreenManager::IsRubyState()
{
	return static_cast<MainScreen*>(GetMenuUI(MAIN_SCREEN))->IsRubyState();
}

iwinmesage::SCREEN_ID ScreenManager::ConvertCurrentMenuToScreenID()
{
	MENU_LAYER current_menu = this->GetCurrentMenuUI();
	if (current_menu == LOADING_SCREEN)
	{
		return SCREEN_ID::SCREEN_NONE;
	}

	if (p_change_award_screen && p_change_award_screen->isVisible())
	{
		return SCREEN_ID::SCREEN_CHARGE_MONEY;
	}
	if (p_shop_screen && this->p_shop_screen->isVisible())
	{
		if (static_cast<MainScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::MAIN_SCREEN))->IsRubyState())
		{
			return SCREEN_ID::SCREEN_SHOP_RUBY;
		}
		return SCREEN_ID::SCREEN_SHOP_WIN;
	}
	auto current_state_common = GetCommonScr->GetStateCommonScr();
	if (current_state_common != 5)
	{
		if (current_state_common == 0)
		{
			return SCREEN_ID::SCREEN_RICHLIST;
		}
		else if (current_state_common == 1)
		{
			return SCREEN_ID::SCREEN_STRONGLIST;
		}
		else if (current_state_common == 2)
		{
			return SCREEN_ID::SCREEN_RICHLIST;
		}
		else if (current_state_common == 3)
		{
			return SCREEN_ID::SCREEN_TAIXIU;
		}
		else if (current_state_common == 4)
		{
			return SCREEN_ID::SCREEN_TAIXIU_RUBY;
		}
	}

	auto cur_screen = this->GetMenuUI(current_menu);

	switch (current_menu)
	{
	case MENU_NONE:
	{

	}
	break;
	case LOADING_SCREEN:
	{

	}
	break;
	case DIALOG_SCREEN:
	{
	}
	break;
	case LOGIN_SCREEN:
	{
		auto current_state = static_cast<LoginScreen*>(cur_screen)->GetStateLoginMenu();
		if (current_state == 0)
		{
			return SCREEN_ID::SCREEN_LOGIN_IWIN_ACCOUNT;
		}
		else if (current_state == 1)
		{
			return SCREEN_ID::SCREEN_REGISTER_IWIN_ACCOUNT;
		}
	}
	break;
	case MAIN_SCREEN:
	{
		auto current_state = static_cast<MainScreen*>(cur_screen)->GetStateMainScreen();
		if (current_state == 0)
		{
			return SCREEN_ID::SCREEN_SETTING;
		}
		else if (current_state == 1)
		{
			return SCREEN_ID::SCREEN_LOGOUT;
		}
		return SCREEN_ID::SCREEN_MAIN_MENU;
	}
	break;
	case PROFILE_SCREEN:
	{
		if (static_cast<ProfileScreen*>(cur_screen)->IsShopAvatarVisible())
		{
			return SCREEN_ID::SCREEN_AVATAR_SHOP;
		}
		return SCREEN_ID::SCREEN_PROFILE;
	}
	break;
	case LOBBY_SCREEN:
	{
		return SCREEN_ID::SCREEN_PICKROOM;
	}
	break;
	case LEADERBOARD_SCREEN:
	{
		auto current_state_lb = static_cast<LeaderboardScreen*>(cur_screen)->CurrentStateLeaderboard();
		if (current_state_lb == 0)
		{
			return SCREEN_ID::SCREEN_RICHLIST;
		}
		else if (current_state_lb == 1)
		{
			return SCREEN_ID::SCREEN_STRONGLIST;
		}
		else if (current_state_lb == 2)
		{
			return SCREEN_ID::SCREEN_RICHLIST;
		}
	}
	break;
	case EVENT_SCREEN:
	{
		return SCREEN_ID::SCREEN_WINFREE;
	}
	break;
	case FRIEND_SCREEN:
	{
		auto current_state = static_cast<FriendScreen*>(cur_screen)->GetStateFriendMenu();
		if (current_state == 0)
		{
			return SCREEN_ID::SCREEN_SEARCH_FRIEND;
		}
		else if (current_state == 1)
		{
			return SCREEN_ID::SCREEN_CHAT;
		}
		return SCREEN_ID::SCREEN_FRIENDLIST;
	}
	break;
	case IAP_SCREEN:
	{
		if (static_cast<MainScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::MAIN_SCREEN))->IsRubyState())
		{
			return SCREEN_ID::SCREEN_SHOP_RUBY;
		}
		return SCREEN_ID::SCREEN_SHOP_WIN;
	}
	break;
	case HUD_SCREEN:
	{
		auto current_state = static_cast<HUDScreen*>(cur_screen)->GetStateHUDScreen();
		if (current_state == 0)
		{
			return SCREEN_ID::SCREEN_SETTING;
		}
		return SCREEN_ID::SCREEN_PLAY_GAME;
	}
	break;
	case NEW_LOBBY_SCREEN:
	{
		auto current_state = static_cast<NewLobbyScreen*>(cur_screen)->GetStateNewLobbyMenu();
		if (current_state == 0)
		{
			return SCREEN_ID::SCREEN_NEW_LOBBY_TABLE;
		}
		else if (current_state == 1)
		{
			return SCREEN_ID::SCREEN_NEW_LOBBY_ROOM;
		}

		return SCREEN_ID::SCREEN_PICKROOM;
	}
	break;
	case CHANGE_AWARD_SCREEN:
	{
		return SCREEN_ID::SCREEN_CHARGE_MONEY;
	}
	break;
	case COMMON_SCREEN:
		break;
	case MENU_COUNT:
		break;
	default:
		break;
	}

	return SCREEN_ID::SCREEN_NONE;

}

void ScreenManager::setTaiXiuUnsupportedList(iwinmesage::TaiXiuUnsupportedList * unsupport)
{
	if (p_taixiu_unsupportlist)
	{
		delete p_taixiu_unsupportlist;
		p_taixiu_unsupportlist = nullptr;
	}
	p_taixiu_unsupportlist = unsupport;
	updateButtonTaiXiu();

	if (!p_taixiu_unsupportlist->getIsRubyEnabled() && !p_taixiu_unsupportlist->getIsWinEnabled())
	{
		GetCommonScr->OnShowBtnTaiXiu(false);
	}
	else 
	{
		GetCommonScr->OnShowBtnTaiXiu(true);
	}
}
void ScreenManager::updateButtonTaiXiu()
{
	auto current_game_screen = GetCurrentMenuUI();
	if (GetCommonScr)
	{
		if (current_game_screen == MENU_LAYER::LOGIN_SCREEN
			|| current_game_screen == MENU_LAYER::LOADING_SCREEN
			|| (GetCommonScr->HaveLayoutInter() && !GetCommonScr->HaveTaixiuLayout())
			//|| GameController.game.getServerLayer().isChangeServer()
			)
		{
			GetCommonScr->OnShowBtnTaiXiu(false);
		}
		else
		{
			GetCommonScr->OnShowBtnTaiXiu(true);
		}
	}


	if (p_taixiu_unsupportlist != nullptr && p_taixiu_unsupportlist->getScreenList().size() > 0 ) 
	{
		auto list_screen = p_taixiu_unsupportlist->getScreenList();

		for (auto ls : list_screen)
		{
			if (current_game_screen != HUD_SCREEN)
			{
				if (ls == GameController::currentScreenId)
				{
					GetCommonScr->OnShowBtnTaiXiu(false);
				}
			}
			else 
			{
				if (p_taixiu_unsupportlist->getGameList().size() > 0)
				{
					auto list_game = p_taixiu_unsupportlist->getGameList();
					for (auto lg : list_game)
					{
						if (GameController::getCurrentGameType() != -1
							&& lg == (int)GameController::getCurrentGameType())
						{
							GetCommonScr->OnShowBtnTaiXiu(false);
						}
					}

				}
			}
		}
	}
}

void ScreenManager::setBtnTaixiuDirty()
{
	btn_taixiu_dirty = true;
}
void ScreenManager::LuckyCircleVisiable(bool isShow)
{
	static_cast<CommonScreen*>(p_common_screen)->LuckyCricleVisible(isShow);
}

void ScreenManager::SetScreenIDDirty()
{
	p_have_screen_id_dirty = true;
}

void ScreenManager::OnReceiveTransferMoney(s64 Money, std::string info, int moneyType)
{
	if (GetCommonScr->GetStateCommonScr() == 3 || GetCommonScr->GetStateCommonScr() == 4)
	{
		GetCommonScr->OnReceiveTransferMoney(Money, info, moneyType);
	}
	else
	{
		ScrMgr->OnShowDialog("notice", info, DIALOG_ONE_BUTTON);
	}

	ScrMgr->MyInfoChanged();
}

void ScreenManager::OnConnectToServer()
{
    p_have_connect_server = true;
}