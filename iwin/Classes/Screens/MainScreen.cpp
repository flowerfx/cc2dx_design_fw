#include "MainScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wPageView.h"
#include "UI/AdvanceUI/wRadioBtn.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wCheckBox.h"
#include "UI/AdvanceUI/wSlider.h"

#include "Network/Global/Globalservice.h"
#include "Common/Common.h"
#include "Network/JsonObject/Game/GameList.h"

#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"

#include "Network/Core/ServerMgr.h"
#include "Common/FlyMoney.h"

#include "LobbyScreen.h"
#include "NewLobbyScreen.h"

MainScreen::MainScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_panel_setting = nullptr;
	p_panel_list_game = nullptr;
	p_gamelist = nullptr;
	p_list_lang_choose = nullptr;

	p_panel_notice_12 = nullptr;
	p_layout_anoun_server = nullptr;
	p_stage_get_data = 1;
	p_have_announcement = false;

	current_state_panel_game = 0;
	p_annouce_is_running = false;

	p_panel_info_game = nullptr;
	p_list_game_id.clear();

	p_current_time_request_list_wait = 3.0f;
	p_already_parse_list_wait = true;

	p_isCallRequestFriend = false;
	p_msg_server = "";
}


MainScreen::~MainScreen()
{
	p_panel_setting = nullptr;
	p_panel_list_game = nullptr;

	p_gamelist = nullptr;
	p_list_lang_choose = nullptr;
	p_panel_notice_12 = nullptr;
	p_layout_anoun_server = nullptr;
	p_stage_get_data = 1;
	p_have_announcement = false;
	p_isCallRequestFriend = false;
}

int MainScreen::Init()
{
	RKString _menu = "main_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	p_panel_setting = GetWidgetChildByName("main_screen.panel_setting");
	p_panel_choose_server = GetWidgetChildByName("main_screen.panel_choose_server");
	p_list_server = p_panel_choose_server->GetWidgetChildByName(".list_server");
	p_list_lang_choose = p_panel_setting->GetWidgetChildByName(".scroll_options.language.list_choose_lang");
	for (int i = 0; i <= LangMgr->GetNumberLang(); i++)
	{
		auto widget = static_cast<ListViewWidget*>(p_list_lang_choose)->CreateWidgetRefer(i);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".title"))->SetText(LangMgr->GetLangDec((Utility::LANGUAGE_TYPE)(i + 1)));
	}

	auto list_server = GetServerMgr()->getServerList();
	for (size_t i = 0; i < list_server.size(); i++)
	{
		auto layout_ = static_cast<ListViewWidget*>(p_list_server)->CreateWidgetRefer(i);
		static_cast<TextWidget*>(layout_->GetWidgetChildByName(".server_choose_btn.server_name"))->SetText(list_server.at(i)._name);
		if (i == 0)
		{
			layout_->GetWidgetChildByName(".stick_icon")->Visible(true);
			layout_->GetWidgetChildByName(".highlights")->Visible(true);
			static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(GetServerMgr()->getCurServer()._name);
		}
	}

	CallInitComponent(true);

	return 1;
}

int	MainScreen::InitComponent()
{
	p_panel_list_game = GetWidgetChildByName("main_screen.list_game_panel");
	p_bottom_menu = GetWidgetChildByName("main_screen.bottom_menu");
	p_panel_list_btn_choose_game = GetWidgetChildByName("main_screen.panel_list_btn_choose_game");
	panel_avatar_user = p_bottom_menu->GetWidgetChildByName(".btn_avatar.avatar_default");
	p_panel_list_btn = p_bottom_menu->GetWidgetChildByName(".panel_list_btn");

	auto cur_lang = LangMgr->GetCurLanguage();
	RKString dec_lang = LangMgr->GetLangDec(cur_lang);
	static_cast<ListViewWidget*>(p_list_lang_choose)->ForceVisibleAllChildLayout(".hight_light", false);
	p_list_lang_choose->GetWidgetChildAtIdx(cur_lang - 1)->GetWidgetChildByName(".hight_light")->Visible(true);
	static_cast<TextWidget*>(
		p_list_lang_choose->GetParentWidget()->GetWidgetChildByName(".btn_input_lang.text_input_1"))->SetText(
			dec_lang
		);

	p_panel_notice_12 = GetWidgetChildByName("main_screen.panel_notice_server.panel_notice_12");
	p_layout_anoun_server = p_panel_notice_12->GetParentWidget()->GetWidgetChildByName(".layout_anoun_server");

	int idx_select = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_INVITE_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".notice_invite_play.check_box_invite_play"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
		}
		else
		{
		}
	}
	if (ScrMgr->GetDataSaveInt(STR_SAVE_BRIVATE_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".brivate_mode.check_box_brivate"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
		}
		else
		{
		}
	}
	if (ScrMgr->GetDataSaveInt(STR_SAVE_FB_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".facebook_login.check_box_fb_login"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
		}
		else
		{
		}
	}

	p_panel_info_game = GetWidgetChildByName("main_screen.panel_info_game");
	p_list_game_win.clear();
	p_list_game_ruby.clear();
	return 1;
}

int MainScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_have_announcement)
	{
		SetAnnouncementAction();
		p_have_announcement = false;
	}

	if (p_layout_anoun_server->Visible())
	{
		auto w_text = p_layout_anoun_server->GetWidgetChildAtIdx(1);
		if (w_text->GetPosition_BottomRight().x < 0)
		{
			p_annouce_is_running = false;
			w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
				w_text->ForceFinishAction();
				this->p_layout_anoun_server->SetActionCommandWithName("ON_HIDE");
			}));
		}
	}

	if (p_annouce_is_running)
	{
		p_layout_anoun_server->Visible(true);
	}

	if (p_already_parse_list_wait)
	{
		p_current_time_request_list_wait -= dt;
		if (p_current_time_request_list_wait <= 0)
		{
			GlobalService::requestListWait();
			p_current_time_request_list_wait = 3.0;
			p_already_parse_list_wait = false;
		}
	}

	return 1;
}

void MainScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void MainScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void MainScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	if (p_panel_choose_server->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_choose_server,
			[&](void){
				p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
					[this](){
					this->p_panel_choose_server->ForceFinishAction();
					this->p_panel_choose_server->Visible(false);
				}));
		});
	}
	else if (p_panel_setting->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_setting,
			[&](void) {
			p_panel_setting->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_panel_setting->ForceFinishAction();
				this->p_panel_setting->Visible(false);
				ScrMgr->SaveTheGame();
			}));
		});
	}
}

void MainScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "load_list_btn")
		{
            if(!GameController::myInfo)
            {
                ScrMgr->OnConnectToServer();
                return;
            }
            
			GlobalService::getNewGameList();

			if (!GetUser || GetUser->id == -1)
			{
				GlobalService::requestProfileOf(GameController::myInfo->IDDB);
				GlobalService::getAllUserAchievement(GameController::myInfo->IDDB);
			}
			p_stage_get_data = 0;
		}
		else if (name == "btn_setting")
		{
			if (!p_panel_setting->Visible())
			{
				p_panel_setting->Visible(true);
				p_panel_setting->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					p_panel_setting->ForceFinishAction();
					ScrMgr->SetScreenIDDirty();
					SetInteractiveInsteadOf(false, p_panel_setting);

					float current_sound_volume = GetSound->GetVolume();
					static_cast<SliderWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".sound_volume.slider_sound"))->SetPercent(current_sound_volume * 100.f);

					float current_music_volume = GetSound->GetVolume(false);
					static_cast<SliderWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".music_volume.slider_music"))->SetPercent(current_music_volume * 100.f);
				}));
			}
		}
		else if (name == "btn_leaderboard")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(LEADERBOARD_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_purchase_win")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			//ScrMgr->SwitchToMenu(IAP_SCREEN, MAIN_SCREEN);
			ScrMgr->ShowShop(true);
		}
		else if (name == "btn_event")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(EVENT_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_friend")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(FRIEND_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_iwin_game")
		{
			if (current_state_panel_game != 0 && p_gamelist)
			{			
				OnParseCustomUIDec(p_list_game_win, "test", p_list_sample_dec);
				OnShowHLBtnGameAtIdx(0);
			}

		}
		else if (name == "btn_ruby_game")
		{
			if (current_state_panel_game != 1 && p_gamelist)
			{
				OnParseCustomUIDec(p_list_game_ruby, "test", p_list_sample_dec);
				OnShowHLBtnGameAtIdx(1);
			}
		}
		else if (name == "btn_avatar")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(MENU_LAYER::PROFILE_SCREEN, MENU_LAYER::MAIN_SCREEN);
			//GlobalService::requestProfileOf(GameController::myInfo->IDDB);
		}
		else if (name == "log_out_btn")
		{
			p_have_layout_logout_appear = true;
			ScrMgr->OnShowDialog("sign_out", "do_you_want_to_log_ou",
				TYPE_DIALOG::DIALOG_TWO_BUTTON,
				"",
				[&](const char * call_back, const char* btn_name)
			{
				p_panel_setting->SetActionCommandWithName("ON_HIDE", CallFunc::create(
					[this]()
				{
					this->p_panel_setting->ForceFinishAction();
					this->p_panel_setting->Visible(false);
					this->p_have_layout_logout_appear = false;

					ScrMgr->ForceLogOutToLoginScreen();
				}));
			},
				[&](const char * call_back, const char* btn_name)
			{
				this->p_have_layout_logout_appear = false;
			});
		}
		else if (name == "server_choose_btn")
		{
			int idx_list_select = static_cast<ListViewWidget*>(p_list_server)->GetCurrentIdxSelected();
			if (idx_list_select >= 0)
			{
				auto current_layout = p_list_server->GetWidgetChildAtIdx(idx_list_select);
				current_layout->GetWidgetChildByName(".stick_icon")->Visible(true);
				current_layout->GetWidgetChildByName(".highlights")->Visible(true);


				int number_child_in_list = static_cast<ListView*>(p_list_server->GetResource())->getChildrenCount();
				for (int i = 0; i < number_child_in_list; i++)
				{
					if (i == idx_list_select)
					{
						continue;
					}
					auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
					current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
					current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);
				}
				GetServerMgr()->selectServer(idx_list_select);
				RKString name_server = GetServerMgr()->getCurServer()._name;

				static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(name_server);
			}

			static_cast<ListViewWidget*>(p_list_server)->SetCurrentIdxSelected(-1);
		}
		else if (name == "btn_server")
		{
			if (p_panel_choose_server->Visible())
			{
				p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
					[this]()
				{
					this->p_panel_choose_server->ForceFinishAction();
					this->p_panel_choose_server->Visible(false);
					SetInteractiveInsteadOf(true, p_panel_choose_server);
				}
				));
			}
			else
			{
				p_panel_choose_server->SetActionCommandWithName("ON_SHOW");
				p_panel_choose_server->Visible(true);
				SetInteractiveInsteadOf(false, p_panel_choose_server);
			}
		}
		else if (name == "btn_input_lang" )
		{
			if (p_list_lang_choose->Visible())
			{
				p_list_lang_choose->SetActionCommandWithName("ON_HIDE");
			}
			else
			{
				p_list_lang_choose->SetActionCommandWithName("ON_SHOW");
			}
		}
		else if (name == "btn_12")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildByName(".panel_notice_12");
			if(!widget->Visible())
			{
				widget->SetActionCommandWithName("ON_SHOW");
			}
		}
		else if (name == "close_annouce_panel")
		{
			if (p_annouce_is_running && p_layout_anoun_server->Visible())
			{
				p_annouce_is_running = false;
				auto w_text = p_layout_anoun_server->GetWidgetChildAtIdx(1);
				w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
					w_text->ForceFinishAction();
					this->p_layout_anoun_server->SetActionCommandWithName("ON_HIDE");
				}));
				GetCommonScr->ForceStopAnnouce(true);
			}
		}
		else if (name == "change_pass_btn")
		{
			p_panel_setting->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_panel_setting->ForceFinishAction();
				this->p_panel_setting->Visible(false);
				ScrMgr->SaveTheGame();

				auto scr_common = GetCommonScr;
				scr_common->SetScreenComeFrom(this);
				//scr_common->SetUser(this->p_user);
				scr_common->OnShowChangePassLayout();

			}));
		}
	
	}
	else if (type_widget == UI_TYPE::UI_LISTVIEW)
	{
		if (name == "list_choose_lang")
		{
			int idx_select = static_cast<ListViewWidget*>(p_list_lang_choose)->GetCurrentIdxSelected();
			static_cast<ListViewWidget*>(p_list_lang_choose)->ForceVisibleAllChildLayout(".hight_light", false);
			_widget->GetParentWidget()->GetWidgetChildByName(".hight_light")->Visible(true);
			//RKString current_text = static_cast<TextWidget*>(_widget->GetParentWidget()->GetWidgetChildByName(".title"))->GetText();
			auto lang_select = (LANGUAGE_TYPE)(idx_select + 1);
			RKString current_text = LangMgr->GetLangDec(lang_select);
			bool result = ScrMgr->ChangeLanguage(lang_select);
			if (result)
			{
				static_cast<TextWidget*>(
					p_list_lang_choose->GetParentWidget()->GetWidgetChildByName(".btn_input_lang.text_input_1"))->SetText(
						current_text
					);
				ScrMgr->SaveDataInt(STR_SAVE_LANGUAGE, lang_select);
			}
		}
		else
		{
			auto p_radio_btn_layer = static_cast<RadioBtnWidget*>(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
			if (name.Contains("_MOVE_NEXT"))
			{
				static_cast<RadioBtnWidget*>(p_radio_btn_layer)->MoveToNextBtn();
			}
			else if (name.Contains("_MOVE_BACK"))
			{
				static_cast<RadioBtnWidget*>(p_radio_btn_layer)->MovePreviousBtn();
			}
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		float value = static_cast<SliderWidget*>(_widget)->GetPercent();
		RKString state_name_save = "";
		if (name == "slider_sound_SLIDEBALL_UP")
		{
			state_name_save = STR_SAVE_SOUND_VOLUME;
			float value1 = value / 100.f;
			GetSound->SetVolume(value1);
		}
		else if (name == "slider_music_SLIDEBALL_UP")
		{
			state_name_save = STR_SAVE_MUSIC_VOLUME;
			float value1 = value / 100.f;
			GetSound->SetVolume(value1, false);
		}
		ScrMgr->SaveDataFloat(state_name_save, value);
		ScrMgr->SaveTheGame();
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		auto sep = name.SplitLast("_");
		RKString state = sep.GetAt(1);
		RKString _name = sep.GetAt(0);
		RKString state_name_save = "";
		if (_name == "check_box_invite_play")
		{
			if (state == "SELECTED")
			{

			}
			else if (state == "UNSELECTED")
			{

			}
			state_name_save = STR_SAVE_INVITE_STATE;
		}
		else if (_name == "check_box_brivate")
		{
			if (state == "SELECTED")
			{

			}
			else if (state == "UNSELECTED")
			{

			}
			state_name_save = STR_SAVE_BRIVATE_STATE;
		}
		else if (_name == "check_box_fb_login")
		{
			if (state == "SELECTED")
			{

			}
			else if (state == "UNSELECTED")
			{

			}
			state_name_save = STR_SAVE_FB_STATE;
		}
		ScrMgr->SaveDataInt(state_name_save, state == "SELECTED" ? 1 : 0);
		ScrMgr->SaveTheGame();
	}
}

void MainScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(MAIN_SCREEN);
	float current_sound_volume = 0;
	float current_music_volume = 0;
	p_panel_notice_12->Visible(false);

	if (p_annouce_is_running)
	{
		p_layout_anoun_server->GetWidgetChildAtIdx(1)->GetResource()->pause();
	}

	ScrMgr->SaveTheGame();
	p_menu_show_next = MENU_NONE;
}


void MainScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void MainScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
    //ScrMgr->OnHideDialog();

}

void MainScreen::OnBeginFadeIn()
{
	auto list_server = GetServerMgr()->getServerList();
	RKString name_server = GetServerMgr()->getCurServer()._name;
	for (size_t i = 0; i < list_server.size(); i++)
	{
		auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
		current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
		current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);

		RKString current_name = static_cast<TextWidget*>(current_layout_t->GetWidgetChildByName(".server_choose_btn.server_name"))->GetText();
		if (current_name == name_server)
		{
			current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(true);
			current_layout_t->GetWidgetChildByName(".highlights")->Visible(true);
		}
	}

	static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(name_server);

	
	if (p_annouce_is_running)
	{
		p_layout_anoun_server->GetWidgetChildAtIdx(1)->GetResource()->resume();
	}

	if (!p_isCallRequestFriend) 
	{
		GlobalService::requestFriendList();
		p_isCallRequestFriend = true;
	}
}



/*
void MainScreen::onMyInfoChanged()
{
	//Player myinfo = GameController.myInfo;
	//userData = GameController.myProfile;
	//setNickNameAndFullName();
	//UIFactory.packLayout(lblMyAccountName);

	//btnMyMoney.setText(Util.getShortMoney(myInfo->getMoney()));
	//btnMyRuby.setText(Util.getShortMoney(myInfo->getRuby()));

	//UIFactory.packLayout(btnMyMoney);
	//UIFactory.packLayout(btnMyRuby);

	//boolean notLoginOrGuest = !GameController.isLoggedIn() || GameController.getLoginType() == AccountTypes.guest;

	//lblMyMoney.setVisible(!notLoginOrGuest);
	//lblMyAccountName.setVisible(!notLoginOrGuest);
	//btnLogin.setVisible(notLoginOrGuest);
	//lblNotLogin.setVisible(notLoginOrGuest);
	//if (notLoginOrGuest) {
	//	imgMyAvatar.setColor(CLR_AVATAR_BACKGROUND_NOT_LOGIN);
	//	imgMyAvatar.setAvatarDefaultAvatar();

	//	//			imgPromotionNotification.setVisible(false);
	//	// star.setVisible(false);
	//}
	//else {
	//	imgMyAvatar.setColor(Color.WHITE);
	//	imgMyAvatar.setAvatar(myInfo->name, myInfo->avatar);

	//	//			imgPromotionNotification.setVisible(true);
	//	// star.setVisible(true);
	//}
}
*/
void MainScreen::onLoginSuccess()
{
	onMyInfoChanged();// goi de gan lai thong tin cua minh
	if (GameController::isLoggedIn()) 
	{
		// send thong tin cho Parse
		/*
		if (GameController::getLoginType() == AccountTypes::facebook) 
		{
			Platform.instance.getFacebookServices().getFacebookMyInfo(new IActionListener(){

				@Override
				public void actionPerformed(Object arg0) {
				if (arg0 != null) {
					FacebookMyInfo facebookMyInfo = (FacebookMyInfo)arg0;
					String fbId = "";
					String email = "";
					if (!Utility.isNullOrEmpty(facebookmyInfo->id)) {
						fbId = facebookmyInfo->id;
					}
					if (!Utility.isNullOrEmpty(facebookmyInfo->email)) {
						email = facebookmyInfo->email;
					}
					ParseServices.sendInstallation(GameController.myInfo->name,
						DeviceService.getManager().getFingerPrint(), 1, fbId);
					ParseServices.logIn(GameController.myInfo->name, "iwin", email);

					Platform.instance.sendOneSignalInfo(GameController.myInfo->name, 2, fbId);
				}
			}
			});
		}
		else if (GameController.getLoginType() == AccountTypes.google) 
		{
			if (GoogleGameServices.isSupportLoginGoogleGameServices()) {
				if (!Utility.isNullOrEmpty(GoogleGameServices.getManager().getCurrentAccount().id)
					&& !Utility.isNullOrEmpty(GoogleGameServices.getManager().getCurrentAccount().email)) {
					ParseServices.sendInstallation(GameController.myInfo->name,
						DeviceService.getManager().getFingerPrint(), 2,
						GoogleGameServices.getManager().getCurrentAccount().id);
					ParseServices.logIn(GameController.myInfo->name, "iwin",
						GoogleGameServices.getManager().getCurrentAccount().email);
					Platform.instance.sendOneSignalInfo(GameController.myInfo->name, 3,
						GoogleGameServices.getManager().getCurrentAccount().email);
				}
			}
			else {
				if (!Utility.isNullOrEmpty(GooglePlusUtils.getCurrentAccount().id)
					&& !Utility.isNullOrEmpty(GooglePlusUtils.getCurrentAccount().email)) {
					ParseServices.sendInstallation(GameController.myInfo->name,
						DeviceService.getManager().getFingerPrint(), 2, GooglePlusUtils.getCurrentAccount().id);
					ParseServices.logIn(GameController.myInfo->name, "iwin",
						GooglePlusUtils.getCurrentAccount().email);
					Platform.instance.sendOneSignalInfo(GameController.myInfo->name, 3,
						GooglePlusUtils.getCurrentAccount().email);
				}
			}
		}
		else if (GameController.getLoginType() == AccountTypes.appleId) {
			if (!Utility.isNullOrEmpty(GameCenterService.getManager().getAppleId())) {
				ParseServices.sendInstallation(GameController.myInfo->name,
					DeviceService.getManager().getFingerPrint(), 3,
					GameCenterService.getManager().getAppleId());
				ParseServices.logIn(GameController.myInfo->name, "iwin", "");
				Platform.instance.sendOneSignalInfo(GameController.myInfo->name, 1,
					GameCenterService.getManager().getAppleId());
			}
		}
		else {
			ParseServices.sendInstallation(GameController.myInfo->name, DeviceService.getManager().getFingerPrint(),
				0, "");
			ParseServices.logIn(GameController.myInfo->name, "iwin", "");
			Platform.instance.sendOneSignalInfo(GameController.myInfo->name, 0, "");
		}*/
		if (GameController::getLoginType() != AccountTypes::quest) 
		{
			GlobalService::getCountQuestList();
		}
		GlobalService::getTaiXiuInfo(GameController::myInfo->name);
        //
        if (GameController::myInfo)
        {
            GlobalService::requestProfileOf(GameController::myInfo->IDDB);
            GlobalService::getAllUserAchievement(GameController::myInfo->IDDB);
            GlobalService::getNewGameList();
        }
        
    }
}

void MainScreen::onReceivedGameList(void* gamelist)
{
	if (p_gamelist)
	{
		delete p_gamelist;
		p_gamelist = nullptr;
	}

	p_list_game_ruby.clear();
	p_list_game_win.clear();

	for (size_t i = 0; i < p_panel_list_game->GetNumberChildWidget(); i++)
	{
		p_panel_list_game->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(0)->ClearChild();
	}

	p_gamelist = (GameList*)gamelist;
	p_list_game_ruby = p_gamelist->getGameHasNewLobby();
	for (auto g : p_gamelist->getGameCard())
	{
		p_list_game_win.push_back(g);
	}
	for (auto g : p_gamelist->getGameChess())
	{
		p_list_game_win.push_back(g);
	}
	for (auto g : p_gamelist->getGameOther())
	{
		p_list_game_win.push_back(g);
	}

	XMLMgr->OnLoadXMLData<MainScreen>("ListGameDec", std::mem_fn(&MainScreen::LoadXMLListGame), this);

	if (!GetUser || GetUser->id == -1)
	{
		p_bottom_menu->GetWidgetChildByName(".load_list_btn")->Visible(true);
		p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(false);
		p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(false);
	}
	else
	{
		p_bottom_menu->GetWidgetChildByName(".load_list_btn")->Visible(false);
		if (current_state_panel_game == 0)
		{
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(true);
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(false);
		}
		else
		{
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(true);
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(false);
		}
	}
}

void MainScreen::LoadXMLListGame(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("sample_ui_dec");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	//std::map<RKString , xml::UILayerWidgetDec*> p_list_sample_dec;

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget , _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);
	
	if (p_list_game_win.size() > 0)
	{
		OnParseCustomUIDec(p_list_game_win, "test", p_list_sample_dec);
		OnShowHLBtnGameAtIdx(0);
	}

	if (p_msg_server != "")
	{
		ScrMgr->OnShowDialog("notice", p_msg_server, DIALOG_ONE_BUTTON);
		p_msg_server = "";
	}
}

void MainScreen::OnParseCustomUIDec(vector<int> list_game, RKString name, std::map<RKString, xml::UILayerWidgetDec*> list_sample_dec)
{
	int size_game = list_game.size();
	if (size_game <= 0)
	{
		return;
	}
	p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->ClearChild();

	auto page_ = static_cast<ListViewWidget*>(p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0));
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	auto parent_layer_ = menu_widget_xml->GetElementChildByName("main_screen")->GetChildWidgetByName("list_game_panel")->GetChildWidgetByName("layout_list_game_")->GetChildWidgetByName("list_game_");

	auto layer_game = GetLayerChildByName("main_screen");
	int actual_size = 0;
	for (int i = 0; i < size_game; i++)
	{
		int id = list_game.at(i);

		if (ScrMgr->GetGameDetailByType((GameType)id).id == GameType::NULL_TYPE)
		{
			continue;
		}
		actual_size++;
		auto dec_game = ScrMgr->GetGameDetailByType((GameType)id);

		auto ui_sample = (list_sample_dec.at("ico_")->Clone());

        ui_sample->NameWidget = "ico_" + std::string(dec_game.str.GetString());
		auto btn_ = ui_sample->GetChildWidgetByName("panel_card")->GetChildWidgetByName("btn_");
		btn_->NameWidget = "btn_playgame_" + std::to_string(id);
		btn_->GeneralValue->ReplaceDataChar("source", dec_game.img.GetString());
		btn_->GeneralValue->ReplaceDataChar("press", dec_game.img.GetString());
		btn_->GeneralValue->ReplaceDataChar("disable", dec_game.img.GetString());
		btn_->p_menu_widget = menu_widget_xml;

		RKString game_name = dec_game.name;
		RKString game_name_server = ServerMgr::getInstance()->getCurServer()._name;

		auto text_game_name = ui_sample->GetChildWidgetByName("panel_card")->GetChildWidgetByName("name_game");
		text_game_name->GeneralValue->ReplaceDataChar("source", game_name.GetString());
		text_game_name->p_menu_widget = menu_widget_xml;

		ui_sample->p_parent_layer = parent_layer_;
		ui_sample->p_menu_widget = menu_widget_xml;
		ui_sample->RecorrectLayerParent(ui_sample);
		ui_sample->RecorrectMenuParent(menu_widget_xml);

		auto _widget_child = layer_game->InitWidgetEntity(ui_sample, layer_game, page_);

		auto btn_w = _widget_child->GetWidgetChildByName(".panel_card.btn_playgame_" + std::to_string(id));
		static_cast<ButtonWidget*>(btn_w)->SetCallBack(
			[this , id, game_name , game_name_server](Ref* ref, Widget::TouchEventType touch_type)
		{
			if (touch_type == Widget::TouchEventType::ENDED)
			{
				GameType current_type = (GameType)id;
				GameController::doPlay((short)current_type);
				if (this->p_gamelist->hasNewLobby(current_type))
				{
					ScrMgr->SwitchToMenu(MENU_LAYER::NEW_LOBBY_SCREEN, MENU_LAYER::MAIN_SCREEN);
					RKString game_ = LangMgr->GetString(game_name);
					auto ui_screen = ScrMgr->GetMenuUI(MENU_LAYER::NEW_LOBBY_SCREEN);
					static_cast<NewLobbyScreen*>(ui_screen)->setTitle((game_name_server + "-" + game_).GetString());
					static_cast<NewLobbyScreen*>(ui_screen)->SetCurrentGameID(id);
				}
				else
				{
					ScrMgr->SwitchToMenu(MENU_LAYER::LOBBY_SCREEN, MENU_LAYER::MAIN_SCREEN);
					RKString game_ = LangMgr->GetString(game_name);
					static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetTextTitle(game_name_server + "-" + game_);
					static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetCurrentGameID(id);
				}
			}
		});

		if(p_list_game_id.size() > 0 && p_list_game_id.find(id) != p_list_game_id.end())
		{
			auto current_obj_data = p_list_game_id.at(id);
			static_cast<TextWidget*>(btn_w->GetWidgetChildByName(".page_.number_player"))->SetText(std::to_string(current_obj_data->user));
			static_cast<TextWidget*>(btn_w->GetWidgetChildByName(".page_.number_room"))->SetText(std::to_string(current_obj_data->room));
		}

		page_->ResizeAndPosChildWidget(_widget_child);
		page_->PushBackItem(_widget_child);

		delete ui_sample;
		ui_sample = nullptr;
	}

	if (size_game > 0)
	{
		auto size_each_panel = page_->GetWidgetChildAtIdx(0)->GetSize();
		int number_panel_each_screen = (GetGameSize().width / size_each_panel.x);
		if (number_panel_each_screen >= actual_size)
		{
			number_panel_each_screen = actual_size;
		}
		page_->SetNumberEachSlide(number_panel_each_screen);

		auto p_radio_btn_layer = page_->GetParentWidget()->GetWidgetChildAtIdx(1);
		int number_radio_btn = (actual_size / number_panel_each_screen) + ((actual_size % number_panel_each_screen) > 0 ? 1 : 0);
		static_cast<RadioBtnWidget*>(p_radio_btn_layer)->SetNumberRadioBtn(number_radio_btn);
	}

}

void MainScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		//PASSERT2(false, "user is null");
		return;
	}
	RKString nick_name = user->nickName;
	long money = GameController::myInfo->money;
	long ruby = GameController::myInfo->ruby;

	auto w_name = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".title_username");
	auto w_money = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_win");
	auto w_ruby = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_ruby");

	static_cast<TextWidget*>(w_name)->SetText(user->fullname + " (" + nick_name + ")");

	static_cast<TextWidget*>(w_money->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(money));
	static_cast<TextWidget*>(w_ruby->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(ruby));

	//check user is active or not
	bool isActive = GameController::myInfo->isActived;

	if (isActive)
	{
		p_panel_list_btn->GetWidgetChildByName(".btn_friend")->SetActionCommandWithName("POS_2");
		p_panel_list_btn->GetWidgetChildByName(".btn_event")->Visible(true);
		GlobalService::getCountQuestList();
	}
	else
	{
		p_panel_list_btn->GetWidgetChildByName(".btn_friend")->SetActionCommandWithName("POS_1");
		p_panel_list_btn->GetWidgetChildByName(".btn_event")->Visible(false);
	}

	GameMgr->HandleAvatarDownload(
		user->avatarID, user->nickName + "_main_scr", [this](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(this->panel_avatar_user, data, str, tag);
	});

	ScrMgr->setBtnTaixiuDirty();
}

void MainScreen::SetAnnouncementAction()
{
	auto tex = static_cast<TextWidget*>(p_layout_anoun_server->GetWidgetChildAtIdx(1));
	tex->SetText(p_announce_text, true);
	p_layout_anoun_server->SetActionCommandWithName("ON_SHOW", CallFunc::create(
		[this, tex]()
	{
		this->p_layout_anoun_server->ForceFinishAction();
		this->p_annouce_is_running = true;
		tex->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this, tex]()
		{
			tex->ForceFinishAction();
			this->p_layout_anoun_server->SetActionCommandWithName("ON_HIDE");
			p_annouce_is_running = false;
		}));
	}));
}

void MainScreen::OnReceiveAnnouncementFromServer(RKString text)
{
	if (!p_layout_anoun_server->Visible())
	{
		p_have_announcement = true;
		text.ReplaceAll("\n", " ");
		
		p_announce_text = text;
		GetCommonScr->OnReceiveAnnouncementFromServer(text);
	}
}

iwinmesage::GameList* MainScreen::GetGameList() 
{
	return p_gamelist;
}

void MainScreen::OnShowHLBtnGameAtIdx(int idx)
{
	if (idx >= (int)p_panel_list_btn_choose_game->GetNumberChildWidget())
	{
		PASSERT2(false, "idx out of range");
		return;
	}

	current_state_panel_game = idx;

	for (size_t i = 0; i < 2; i++)
	{
		auto w_panel = p_panel_list_btn_choose_game->GetWidgetChildAtIdx(i);
		w_panel->GetWidgetChildAtIdx(1)->Visible(true); //no choose
		w_panel->GetWidgetChildAtIdx(2)->Visible(false); //choose
	}
	p_panel_list_btn_choose_game->GetWidgetChildAtIdx(idx)->GetWidgetChildAtIdx(1)->Visible(false);
	p_panel_list_btn_choose_game->GetWidgetChildAtIdx(idx)->GetWidgetChildAtIdx(2)->Visible(true);

	auto w_money = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_win");
	auto w_ruby = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_ruby");
	if (idx == 0) //win
	{
		//show display screen
		GetWidgetChildByName("main_background.background_1")->Visible(true);
		GetWidgetChildByName("main_background.background_2")->Visible(false);
		//show btn iap
		w_money->Visible(true);
		w_ruby->Visible(false);
	}
	else
	{
		GetWidgetChildByName("main_background.background_1")->Visible(false);
		GetWidgetChildByName("main_background.background_2")->Visible(true);
		w_money->Visible(false);
		w_ruby->Visible(true);
	}
}

void MainScreen::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::onReceiveAvatarList(void * data)
{
	AvatarList * value = (AvatarList *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::OnParsePlayerNumberToEachGame()
{
	auto page_ = static_cast<ListViewWidget*>(p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0));

	for (size_t i = 0; i < page_->GetNumberChildWidget(); i++)
	{
		auto w_child = page_->GetWidgetChildAtIdx(i);
		auto btn_panel = w_child->GetWidgetChildByName(".panel_card")->GetWidgetChildAtIdx(0);
		RKString name = btn_panel->GetName();
		RKString id = name.SplitLast("_").GetAt(1);
		int p_id = atoi(id.GetString());

		auto current_obj_data = p_list_game_id.at(p_id);
		static_cast<TextWidget*>(btn_panel->GetWidgetChildByName(".page_.number_player"))->SetText(std::to_string(current_obj_data->user));
		static_cast<TextWidget*>(btn_panel->GetWidgetChildByName(".page_.number_room"))->SetText(std::to_string(current_obj_data->room));

	}
}

void MainScreen::onReceiveListWait(void* data1, void* data2, void* data3)
{
	int countRoom = *(int*)data1;
	int countUser = *(int*)data2;

	for (auto it = p_list_game_id.begin(); it != p_list_game_id.end(); it++)
	{
		delete it->second;
	}
	p_list_game_id.clear();

	p_list_game_id = *(std::map<int , GameObjectData*>*) data3;
	
	static_cast<TextWidget*>(p_panel_info_game->GetWidgetChildAtIdx(3))->SetText(std::to_string(countUser));
	static_cast<TextWidget*>(p_panel_info_game->GetWidgetChildAtIdx(4))->SetText(std::to_string(countRoom));
	OnParsePlayerNumberToEachGame();
	p_already_parse_list_wait = true;
}

void MainScreen::forceStopAnnouce(bool value)
{
	p_annouce_is_running = !value;
	p_layout_anoun_server->Visible(!value);
}

void MainScreen::SetNumWinfree(int number)
{
	if (number > 0)
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_event.icon_number");
		w->Visible(true);
		static_cast<TextWidget*>(w->GetWidgetChildAtIdx(0))->SetText(std::to_string(number));
	}
	else
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_event.icon_number");
		w->Visible(false);
	}
}

void MainScreen::SetMsgServer(RKString msg_server)
{
	p_msg_server = msg_server;
}

void MainScreen::SetNumUnReadMsg(int number)
{
	if (number > 0)
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_friend.icon_number");
		w->Visible(true);
		static_cast<TextWidget*>(w->GetWidgetChildAtIdx(0))->SetText(std::to_string(number));
	}
	else
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_friend.icon_number");
		w->Visible(false);
	}
}

void MainScreen::OnReceiveFriendList()
{
	p_isCallRequestFriend = true;
}

void MainScreen::SetNeedToReloadFriend()
{
	p_isCallRequestFriend = false;
}

void MainScreen::SetStateLoadData(int state)
{
	p_stage_get_data = state;
}

bool MainScreen::IsRubyState()
{
	return (current_state_panel_game == 1);
}

int MainScreen::GetStateMainScreen()
{
	if (p_panel_setting->Visible())
	{
		return 0;
	}
	else
	{
		if (p_have_layout_logout_appear)
		{
			return 1;
		}
	}
	return 2;
}