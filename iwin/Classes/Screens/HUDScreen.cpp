#include "HUDScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wSlider.h"

#include "Models/Player.h"
#include "Common/GameController.h"
#include "Network/Game/GameLogicHandler.h"

#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"
#include "Network/Game/GameService.h"

#include "Screens/ProfileScreen.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "Object/PlayerGroup.h"
#include "Common/ChatBoard.h"
const char * list_chat[10] = {
	"Nhanh lên pa !",
	"Cho chết ! Hihi !",
	"Thua đi cưng",
	"Hehe",
	"Sợ rồi à",
	"Nhanh đi nghĩ gì lâu vậy",
	"1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm",
	"1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm",
	"1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm",
	"1dasdjkasjakfjdlsfdlsfdklsfdklsfkldsfldsfm"
};


HUDScreen::HUDScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_player_layout = nullptr;
	p_panel_igm = nullptr;
	p_panel_show_profile = nullptr;
	p_panel_ig_setting = nullptr;
	p_panel_bottom = nullptr;
	p_on_count_down = false;

	p_panel_chat_root = nullptr;
	p_btn_recent_chat = nullptr;
	p_btn_emo_chat = nullptr;
	p_list_recent_chat = nullptr;
	p_list_emoticon_chat = nullptr;
	p_panel_send_gift = nullptr;

	p_max_player = 0;
	p_list_pos_dec.clear();

	p_text_input_chat = nullptr;

	p_thread_parse_ui = nullptr;

	p_current_idx_add_1 = -1;
	p_time_add_1 = p_origin_time_add_1 = 0;
	p_size_object_need_to_add_1 = 0;

	p_sample_dec = nullptr;
	p_list_player_waiting.clear();
	p_list_avatar_download.clear();
	_chat_board = nullptr;

}

HUDScreen::~HUDScreen()
{
	XMLMgr->RemoveUIMEnuWidgetDec("hud_screen");
	p_list_player_layout = nullptr;
	SAFE_DELETE_VECTOR(p_list_player_group)

	p_panel_igm = nullptr;
	p_panel_show_profile = nullptr;
	p_panel_ig_setting = nullptr;
	p_panel_bottom = nullptr;
	p_panel_send_gift = nullptr;

	p_panel_chat_root = nullptr;
	p_btn_recent_chat = nullptr;
	p_btn_emo_chat = nullptr;
	p_list_recent_chat = nullptr;
	p_list_emoticon_chat = nullptr;
	p_text_input_chat = nullptr;


	p_max_player = 0;
	p_list_pos_dec.clear();
	SAFE_DELETE(p_thread_parse_ui);
	SAFE_DELETE(p_sample_dec);
	SAFE_DELETE_VECTOR(p_list_player_waiting);
	static_cast<ListViewTableWidget*>(p_list_player_invite)->ResetListView();
	p_list_avatar_download.clear();
	SAFE_DELETE(_chat_board);
}

int HUDScreen::Init()
{
	XMLMgr->OnLoadXMLData1("UIWidget_table",[&](TiXmlDocument * objectXMl){XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, "common");});

	RKString _menu = "hud_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	return 1;
}

int	HUDScreen::InitComponent()
{
	//p_panel_send_gift = GetWidgetChildByName("HUD_screen.gift_player_layout.gift_player");
	p_panel_igm = GetWidgetChildByName("HUD_screen.panel_in_game_menu");
	p_panel_show_profile = GetWidgetChildByName("HUD_screen.show_profile_layout");
	p_panel_ig_setting = GetWidgetChildByName("HUD_screen.panel_in_game_setting");
	p_list_player_layout = GetWidgetChildByName("HUD_screen.layout_list_user_card");
	p_panel_chat_root = GetWidgetChildByName("HUD_screen.layout_chat");
	p_btn_recent_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.btn_recent_chat");
	p_btn_emo_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.btn_emotion");
	p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(true);
	p_list_recent_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.list_btn_recent_chat");
	p_list_emoticon_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.list_btn_emoticon_chat");
	p_text_input_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.panel_input_chat.text_input_chat");

	p_layout_invite_player = GetWidgetChildByName("HUD_screen.invite_play_layout");
	p_list_player_invite = p_layout_invite_player->GetWidgetChildByName(".list_player_invite");
	p_top_title_invite = p_layout_invite_player->GetWidgetChildByName(".top_title");
	if (p_list_player_invite)
	{
		auto list_player_lb = static_cast<ListViewTableWidget*>(p_list_player_invite);
		list_player_lb->addEventListener(
			[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = list_player_lb->GetCurrentIdxTo();
				OnParsePlayerToInviteList(_widget, current_idx_to);
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				size_t current_idx = list_player_lb->GetCurrentIdxSelected();
				if (current_idx >= 0 && current_idx < p_list_player_waiting.size())
				{
					auto p = p_list_player_waiting[current_idx];
					p->is_check = !p->is_check;

					_widget->GetWidgetChildByName(".on_check")->Visible(p->is_check);
					_widget->GetWidgetChildByName(".off_check")->Visible(!p->is_check);

					ModAllCheck(p->is_check);
				}
				else
				{
					PASSERT2(false, "have problem here!");
				}

			}
		});
		list_player_lb->InitTable();
	}

	//////////////
	p_current_idx_add_2 = 0;
	p_time_add_2 = p_origin_time_add_2 = 0.5;
	p_size_object_need_to_add_2 = 10;
	////
	p_panel_count_down = GetWidgetChildByName("HUD_screen.countdown_timer");
	//auto widget_rotate = GetWidgetChildByName("HUD_screen.top_left_layout.btn_purchase_iwin.hl_grow");
	//widget_rotate->SetActionCommandWithName("rotate_hl");
	p_panel_count_down->SetZOrder(1000);

	auto p_scrollview_chat = p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat.scrollview_chat");
	auto p_scrollitem_chat = p_panel_chat_root->GetWidgetChildByName(".chat_text");
	_chat_board = new ChatBoard();
	_chat_board->init((ScrollViewWidget*)(p_scrollview_chat), p_scrollitem_chat);

	return 1;
}

int HUDScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_thread_parse_ui && !p_thread_parse_ui->IsThreadRunning())
	{
		//parse list emotion;
		LoadListAsTime1(dt);
	}
	//parse btn
	LoadListAsTime2(dt);

	if (p_on_count_down)
	{
		p_timer_count_down -= dt;
		if (p_timer_count_down <= 0)
		{
			p_timer_count_down = 0;
			p_on_count_down = false;
			p_panel_count_down->SetVisible(false);
		}
		static_cast<TextWidget*>(p_panel_count_down->GetWidgetChildByName(".timer_txt"))->SetText(std::to_string((int)p_timer_count_down));
	}

	//update player group
	for (auto p : p_list_player_group)
	{
		p->Update(dt);
	}

	return 1;
}

void HUDScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void HUDScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void HUDScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	if (p_panel_igm->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_igm,
			[&](void)
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_igm->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_igm);
			}));
		});
	}
	else if (p_panel_chat_root->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_chat_root->GetWidgetChildByName(".panel_chat"),
			[&](void)
		{
			p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
				this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create([this] {
					this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->ForceFinishAction();
					this->p_panel_chat_root->SetActionCommandWithName("ON_HIDE");
					this->p_panel_chat_root->GetParentLayer()->SetInteractive(true);
				}));
			}
			));
		});
	}
	else if (p_layout_invite_player->Visible())
	{
		CheckTouchOutSideWidget(p, p_layout_invite_player,
			[&](void)
		{
			p_layout_invite_player->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_layout_invite_player->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_layout_invite_player);
			}));
		});
	}
	else if (p_panel_ig_setting->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_ig_setting,
			[&](void)
		{
			this->p_panel_ig_setting->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_ig_setting->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_ig_setting);
			}));
		});
	}
	else if (p_panel_show_profile->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_show_profile,
			[&](void)
		{
			this->p_panel_show_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_show_profile->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_show_profile);
			}));
		});
	}
}

void HUDScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_TIME_PROCESS)
	{
		if (_widget->GetParentWidget()->GetName() == "layout_timer")
		{
			_widget->GetParentWidget()->Visible(false);
		}
		else if (_widget->GetParentWidget()->GetName() == "countdown_timer")
		{
			_widget->GetParentWidget()->Visible(false);
			p_on_count_down = false;
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_in_game_menu")
		{
			if (!p_panel_igm->Visible())
			{
				p_panel_igm->SetActionCommandWithName("ON_SHOW");
				SetInteractiveInsteadOf(false, p_panel_igm);
			}
			else
			{
				p_panel_igm->SetActionCommandWithName("ON_HIDE");
				SetInteractiveInsteadOf(true, p_panel_igm);
			}
		}
		else if (name == "guide_ui_btn")
		{

			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				p_panel_igm->ForceFinishAction();
				SetInteractiveInsteadOf(true, p_panel_igm);
			}
			));
		}
		else if (name == "btn_recent_chat")
		{
			p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(true);
			p_btn_emo_chat->GetWidgetChildByName(".icon")->Visible(false);
			p_list_emoticon_chat->Visible(false);
			p_list_recent_chat->Visible(true);
		}
		else if (name == "btn_emotion")
		{
			p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(false);
			p_btn_emo_chat->GetWidgetChildByName(".icon")->Visible(true);
			p_list_emoticon_chat->Visible(true);
			p_list_recent_chat->Visible(false);
		}
		else if (name == "btn_chat")
		{
			if (!p_panel_chat_root->Visible())
			{
				p_panel_chat_root->SetActionCommandWithName("ON_SHOW", CallFunc::create(
					[this]()
				{
					p_panel_chat_root->ForceFinishAction();
					p_panel_chat_root->GetParentLayer()->SetInteractive(false);
					p_panel_chat_root->SetInteractive(true);

					p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_SHOW", CallFunc::create([this] () {
						p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
						p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_SHOW");
					}));

				}));
			}			
		}
		else if (name == "clear_text_input_chat_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_chat");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}	
		else if ((name == "btn_purchase_win") || (name == "btn_purchase_ruby"))
		{
			ScrMgr->ShowShop(true);
		}
		else if (name.Contains("btn_add_user_"))
		{
			if (p_layout_invite_player->Visible())
			{
				p_layout_invite_player->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
					this->p_layout_invite_player->ForceFinishAction();
					this->SetInteractiveInsteadOf(true, this->p_layout_invite_player);
				}));
			}
			else
			{
				p_layout_invite_player->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_layout_invite_player->ForceFinishAction();
					this->SetInteractiveInsteadOf(false, this->p_layout_invite_player);
					GameService::requestWaitingListInGame();
				}));
			}
		}
		else if (name == "btn_check_invite_all_friend")
		{
			auto parent = _widget->GetParentWidget();
			if (parent->GetWidgetChildByName(".on_check")->Visible() == true)
			{
				parent->GetWidgetChildByName(".on_check")->Visible(false);
				parent->GetWidgetChildByName(".off_check")->Visible(true);
				CheckAllWaitingList(true);
			}
			else
			{
				parent->GetWidgetChildByName(".on_check")->Visible(true);
				parent->GetWidgetChildByName(".off_check")->Visible(false);
				CheckAllWaitingList(false);
			}
		}
		else if (name == "invite_btn")
		{
			p_layout_invite_player->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_layout_invite_player->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_layout_invite_player);

				for (auto p : p_list_player_waiting)
				{
					if (p->is_check)
					{
						GameService::requestInvite(p->player->IDDB);
					}
				}

			}));
		}
		else if (name == "setting_ui_btn")
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_igm->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_igm);
				this->p_panel_ig_setting->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_panel_ig_setting->ForceFinishAction();
					this->SetInteractiveInsteadOf(false, this->p_panel_ig_setting);
					ScrMgr->SetScreenIDDirty();

					//
					float current_sound_volume = GetSound->GetVolume();
					static_cast<SliderWidget*>(p_panel_ig_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".sound_volume.slider_sound"))->SetPercent(current_sound_volume * 100.f);

					float current_music_volume = GetSound->GetVolume(false);
					static_cast<SliderWidget*>(p_panel_ig_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".music_volume.slider_music"))->SetPercent(current_music_volume * 100.f);
					//
				}));
			}));
		}
		else if (name == "chat_ui_btn")
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_igm->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_igm);
				ScrMgr->SwitchToMenu(FRIEND_SCREEN, MENU_NONE);
				this->setVisible(false);
			}));
		}
		else if (name == "btn_achievement_player" 
			|| name == "btn_info_player"
			|| name == "btn_back_profile_layout")
		{
			this->p_panel_show_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, name]() {
				this->p_panel_show_profile->ForceFinishAction();
				this->SetInteractiveInsteadOf(true, this->p_panel_show_profile);
				if (name != "btn_back_profile_layout")
				{
					ScrMgr->SwitchToMenu(PROFILE_SCREEN, MENU_NONE);
					static_cast<ProfileScreen*>(ScrMgr->GetMenuUI(PROFILE_SCREEN))->ShowLayoutState(name == "btn_info_player" ? 0 : 1);
					this->setVisible(false);
				}
			}));
		}

		else if (name == "btn_close_gift")
		{
			p_panel_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_send_gift->ForceFinishAction();
				this->p_panel_send_gift->GetParentWidget()->Visible(false);
				this->SetInteractiveInsteadOf(true, this->p_panel_send_gift);
			}));
		}
		else if (name.Contains("btn_gift_user_"))
		{
			GameService::getGift(8, (unsigned char)0, (unsigned char)0);
			this->p_panel_send_gift->GetParentWidget()->Visible(true);
			p_panel_send_gift->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
				this->p_panel_send_gift->ForceFinishAction();
				this->SetInteractiveInsteadOf(false, this->p_panel_send_gift);
			}));
		}
		
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "text_input_chat_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_chat_btn")->Visible(false);
		}
		else if (name == "text_input_chat_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_chat_btn")->Visible(true);
			}
		}
		else if (name == "text_input_chat_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_chat_btn")->Visible(true);
		}
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		float value = static_cast<SliderWidget*>(_widget)->GetPercent();
		if (name == "slider_sound_SLIDEBALL_UP")
		{
			float value1 = value / 100.f;
			GetSound->SetVolume(value1);
			ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, value);
			ScrMgr->SaveTheGame();
		}
		else if (name == "slider_music_SLIDEBALL_UP")
		{
			float value1 = value / 100.f;
			GetSound->SetVolume(value1, false);
			ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, value);
			ScrMgr->SaveTheGame();
		}
	}
}

void HUDScreen::OnDeactiveCurrentMenu()
{
	//GetWidgetChildByName("HUD_screen.panel_game_screen")->GetResource()->removeAllChildrenWithCleanup(true);

	ScrMgr->CloseCurrentMenu(HUD_SCREEN);
	SAFE_DELETE_MAP(p_list_pos_dec)
	SAFE_DELETE_VECTOR(p_list_player_group)
	
	static_cast<ListViewWidget*>(p_list_recent_chat)->ClearChild();
	static_cast<ListViewWidget*>(p_list_emoticon_chat)->ClearChild();
	p_list_player_layout->ClearChild();

	p_menu_show_next = MENU_NONE;

	ScrMgr->DeinitGameScreen();
}

void HUDScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}


void HUDScreen::start_thread_parse_ui()
{
	SAFE_DELETE(p_thread_parse_ui);
	p_thread_parse_ui = new ThreadWrapper();
	p_thread_parse_ui->CreateThreadWrapper("thread_parse_ui",
		([](void * data) -> uint32
		{
			HUDScreen * scr = (HUDScreen*)data;
			scr->p_thread_parse_ui->OnCheckUpdateThreadWrapper
			([scr]()
			{
				XMLMgr->OnLoadXMLData<HUDScreen>("ListPlayer", std::mem_fn(&HUDScreen::LoadXMLEmotion), scr);
			});
			return 1;
		}),(void*)this);
}

void HUDScreen::OnBeginFadeIn()
{
	if (_chat_board)
	{
		_chat_board->reset();
	}
}

PlayerGroup * HUDScreen::GetPlayerGroupAtIdx(int idx)
{
	if (p_list_player_group.size() <= 0 || (size_t)idx >= p_list_player_group.size())
		return nullptr;
	return p_list_player_group.at(idx);
}

void HUDScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	start_thread_parse_ui();
}

void HUDScreen::OnShow()
{
	Screen::OnShow();

	GameType current_gameplay = GameController::getCurrentGameType();

	int max_player = GameController::getMaxPlayerOf(current_gameplay);
}

void HUDScreen::SetMaxPlayer(int max_player)
{
	p_max_player = max_player;
}

void HUDScreen::showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players)
{
	GameController::showGameScreen(roomID, boardID, ownerID, money, players);
}

void HUDScreen::OnHideCountDown() {

	p_on_count_down = false;
	p_timer_count_down = 0.f;
	p_panel_count_down->Visible(false);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_1"))->StopTimeProcess();
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_2"))->StopTimeProcess();


}

void HUDScreen::OnShowCountDown(float time, float cur_time)
{
	if (p_on_count_down)
	{
		return;
	}

	p_timer_count_down = cur_time;
	if (p_timer_count_down <= 0)
	{
		p_timer_count_down = time - cur_time;
	}

	p_on_count_down = true;

	p_panel_count_down->Visible(true);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_1"))->SetTimer(time,cur_time);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_2"))->SetTimer(time,cur_time);
}

void HUDScreen::LoadListAsTime2(float dt)
{
	if (p_time_add_2 >= 0.f && p_current_idx_add_2 > -1)
	{
		p_time_add_2 -= dt;
		if (p_time_add_2 <= 0.f)
		{
			p_time_add_2 = 0.f;
			if (p_current_idx_add_2 > -1)
			{
				OnCreateBtnRecentChat(list_chat[p_current_idx_add_2]);

				p_current_idx_add_2++;
				p_time_add_2 = p_origin_time_add_2;
				if (p_current_idx_add_2 >= p_size_object_need_to_add_2)
				{
					p_current_idx_add_2 = -1;
				}
			}
		}

	}
}

void HUDScreen::LoadListAsTime1(float dt)
{
	if (p_time_add_1 >= 0.f && p_current_idx_add_1 > -1)
	{
		p_time_add_1 -= dt;
		if (p_time_add_1 <= 0.f)
		{
			p_time_add_1 = 0.f;
			if (p_current_idx_add_1 > -1)
			{
				OnCreateBtnEmoticonChat(p_current_idx_add_1 + 1, p_sample_dec->Clone());

				p_current_idx_add_1++;
				p_time_add_1 = p_origin_time_add_1;
				if (p_current_idx_add_1 >= p_size_object_need_to_add_1)
				{
					p_current_idx_add_1 = -1;
				}
			}
		}

	}
}

void HUDScreen::LoadXMLEmotion(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("list_widget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			RKString name = sample_ui_dec_xml->Attribute("name");
			if (name == "btn_emotion_")
			{
				auto _widget_root = XMLMgr->OnParseWidgetEntity(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
				p_sample_dec = _widget_root;
				break;
			}
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

	//add emoticon list
	/*for (int i = 1; i <= 60; i++)
	{
		OnCreateBtnEmoticonChat(i, p_sample_dec->Clone());
	}*/

	p_current_idx_add_1 = 0;
	p_time_add_1 = p_origin_time_add_1 = 0.25;
	p_size_object_need_to_add_1 = 60;

	//CC_SAFE_DELETE(p_sample_dec);
}


void HUDScreen::OnCreateBtnRecentChat(RKString text)
{
	auto list_chat = static_cast<ListViewWidget*>(p_list_recent_chat);

	int current_layout = (int)(list_chat->GetResource()->getChildrenCount());
	

	if (current_layout == 0)
	{
		auto _widget_layout = list_chat->CreateWidgetRefer(current_layout);

		static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_0"))->SetCallBack(
			[text, this](cocos2d::Ref * ref, ui::Widget::TouchEventType touch_type)
		{
			if (touch_type == ui::Widget::TouchEventType::ENDED)
			{
				this->HandleChatText(text);
			}
		});

		auto current_text = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
		static_cast<TextWidget*>(current_text)->SetText(text);
	}
	else
	{
		auto _widget_layout = list_chat->GetWidgetChildAtIdx(current_layout - 1);
		auto current_text_1 = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
		auto current_text_2 = _widget_layout->GetWidgetChildByName(".btn_chat_1.notice_content");

		RKString cur_tex_1 = static_cast<TextWidget*>(current_text_1)->GetText();
		RKString cur_tex_2 = static_cast<TextWidget*>(current_text_2)->GetText();
		if (cur_tex_2 == "test_WIP" && cur_tex_1 != "test_WIP")
		{
			static_cast<TextWidget*>(current_text_2)->SetText(text);
			static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_1"))->SetCallBack(
				[text, this](cocos2d::Ref * ref, ui::Widget::TouchEventType touch_type)
			{
				if (touch_type == ui::Widget::TouchEventType::ENDED)
				{
					this->HandleChatText(text);
				}
			});
		}
		else if (cur_tex_2 != "test_WIP" && cur_tex_1 != "test_WIP")
		{
			auto _widget_layout = list_chat->CreateWidgetRefer(current_layout);
			auto current_text = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
			static_cast<TextWidget*>(current_text)->SetText(text);

			static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_0"))->SetCallBack(
				[text, this](cocos2d::Ref * ref, ui::Widget::TouchEventType touch_type)
			{
				if (touch_type == ui::Widget::TouchEventType::ENDED)
				{
					this->HandleChatText(text);
				}
			});
		}
		else
		{
			PASSERT2(false, "this case is not handle!");
		}
	}
}

void HUDScreen::OnCreateBtnEmoticonChat(int idx , xml::UILayerWidgetDec* xml_dec)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	xml_dec->p_menu_widget = menu_widget_xml;

	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen"
	)->GetChildWidgetByName("layout_chat")->GetChildWidgetByName("panel_chat"
	)->GetChildWidgetByName("list_btn_emoticon_chat")->GetChildWidgetByName("layout_emoticon_chat_");

	auto layer_root = GetLayerChildByName("HUD_screen");

	auto list_emotion = static_cast<ListViewWidget*>(p_list_emoticon_chat);
	int current_layout = (int)(list_emotion->GetResource()->getChildrenCount());

	WidgetEntity * layout_list_emo = nullptr;

	int cur_index = 0;
	if (current_layout == 0)
	{
		layout_list_emo = list_emotion->CreateWidgetRefer(current_layout);
		cur_index = 0;
	}
	else
	{
		auto layout_ = list_emotion->GetWidgetChildAtIdx(current_layout - 1);
		int number_child = (int)(layout_->GetResource()->getChildrenCount());
		if (number_child < 10)
		{
			layout_list_emo = layout_;
			cur_index = number_child;
		}
		else
		{
			layout_list_emo = list_emotion->CreateWidgetRefer(current_layout);
			cur_index = 0;
		}
	}
	PASSERT2(layout_list_emo != nullptr, "have problem in get list emo layout !");
	
	float pos_x = 45 * (cur_index + 1) + 44 * cur_index;
	float pos_y = 33;
	Vec2 design_size = menu_widget_xml->DesignSize;
	xml_dec->OriginValue->ReplaceDataVector2("pos_ratio", Vec2(pos_x / design_size.x, pos_y / design_size.y));
	xml_dec->FirstValue->ReplaceDataVector2("pos_ratio", Vec2(pos_x / design_size.x, pos_y / design_size.y));

	xml_dec->NameWidget = xml_dec->NameWidget + std::to_string(idx);
	xml_dec->GeneralValue->ReplaceDataChar("source",  ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());
	xml_dec->GeneralValue->ReplaceDataChar("press",   ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());
	xml_dec->GeneralValue->ReplaceDataChar("disable", ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());


	auto widget_emo = layer_root->InitWidgetEntity(xml_dec, layer_root, layout_list_emo);
	layout_list_emo->ResizeAndPosChildWidget(widget_emo);

	static_cast<ButtonWidget*>(widget_emo)->SetCallBack(
		[idx , this](cocos2d::Ref * ref, ui::Widget::TouchEventType touch_type) 
	{
		if (touch_type == ui::Widget::TouchEventType::ENDED)
		{
			this->HandleChatText(Utils::getEmotionStr(idx));
		}
	});

	widget_emo->SetMenuParent(layout_list_emo->GetMenuParent());
	widget_emo->SetParentWidget(layout_list_emo);
	widget_emo->SetParentLayer(layout_list_emo->GetParentLayer());
	layout_list_emo->InsertChildWidget(widget_emo);

	SAFE_DELETE(xml_dec);
}

void HUDScreen::onSetData(void* data)
{
	User * user = (User*)data;

	for (size_t i = 0; i < p_list_player_group.size(); i++)
	{
		if(p_list_player_group[i]->GetIDDB() == user->id)
		{
			p_list_player_group[i]->OnSetData(user);
			return;
		}
	}
}


void HUDScreen::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::onReceiveAvatarList(void * data)
{
	AvatarList * value = (AvatarList *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::OnParsePlayerToInviteList(WidgetEntity * widget, int idx, bool isPlayer/* = true*/)
{
	if (idx < 0 || idx >= p_list_player_waiting.size())
		return;
	Player * player = p_list_player_waiting[idx]->player;

	if (!player)
	{
		PASSERT2(false, "player is null");
		return;
	}

	if (idx % 2 == 0)
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(true);
	}
	else
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(false);
	}

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(player->name, true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber_win(player->money), true);

	//static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_ruby_id"))->SetText(Utils::formatNumber_ruby(player->ruby), true);

	widget->GetWidgetChildByName(".on_check")->Visible(p_list_player_waiting[idx]->is_check);
	widget->GetWidgetChildByName(".off_check")->Visible(!p_list_player_waiting[idx]->is_check);

	auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(player->IDDB) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(player->IDDB), 100);
	}
}

void HUDScreen::OnReceiveWaitingList(std::vector<Player*> l)
{
	SAFE_DELETE_VECTOR(p_list_player_waiting);
	for (auto p : l)
	{
		p_list_player_waiting.push_back(new PlayerWaiting(p));
	}

	static_cast<ListViewTableWidget*>(p_list_player_invite)->SetNumberSizeOfCell(p_list_player_waiting.size());

	//download avatar in save into the list
	for (size_t i = 0; i < p_list_player_waiting.size(); i++)
	{
		auto idx_player = p_list_player_waiting[i];
		{
			GameMgr->HandleAvatarDownload(idx_player->player->avatar, idx_player->player->name,
				[this, idx_player, i](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (this->p_list_player_waiting.size() > 0)
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
					this->p_list_avatar_download.insert(idx_player->player->IDDB, sprite_img);

					auto widget_lb = static_cast<ListViewTableWidget*>(p_list_player_invite);
					if ((size_t)i < widget_lb->GetListWidgetSample().size())
					{
						auto widget = widget_lb->GetListWidgetSample()[i];
						if (widget)
						{
							auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
							this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						}
					}
				}
			});
		}
	}

}

void HUDScreen::CheckAllWaitingList(bool ischeck)
{
	for (auto p : p_list_player_waiting)
	{
		p->is_check = ischeck;
	}
	auto list_widget = static_cast<ListViewTableWidget*>(p_list_player_invite)->GetListWidgetSample();

	for (auto widget : list_widget)
	{
		widget->GetWidgetChildByName(".on_check")->Visible(ischeck);
		widget->GetWidgetChildByName(".off_check")->Visible(!ischeck);
	}
}

bool HUDScreen::ModAllCheck(bool is_check)
{
	if (is_check)
	{
		bool have_un_check = false;
		for (auto p : p_list_player_waiting)
		{
			if (!p->is_check)
			{
				have_un_check = true;
				break;
			}
		}
		if (have_un_check)
		{
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(true);
		}
		else
		{
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(true);
		}
	}
	else
	{
		bool have_check = false;
		for (auto p : p_list_player_waiting)
		{
			if (p->is_check)
			{
				have_check = true;
				break;
			}
		}
		if (have_check)
		{
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(true);
		}
		else
		{
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(true);
		}
	}

	return true;
}

void HUDScreen::PushTextChat(RKString name, RKString msg)
{
	_chat_board->addChat(name.GetString(), msg.GetString());
}

void HUDScreen::HandleChatText(RKString text)
{
	RKString cur_text = static_cast<TextFieldWidget*>(p_text_input_chat)->GetText();
	cur_text += text;
	static_cast<TextFieldWidget*>(p_text_input_chat)->SetText(cur_text);
}

void HUDScreen::OnShowProfileLayout()
{
	this->p_panel_show_profile->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
		this->p_panel_show_profile->ForceFinishAction();
		this->SetInteractiveInsteadOf(false, this->p_panel_show_profile);
	}));
}

void HUDScreen::setGiftCategories(std::vector<Category*> categories)
{
	/*
	if (categories != null && categories.size > 0) {
			if (giftDialog == null) {
				giftDialog = new GiftDialog();
				giftDialog.sendGiftAction = new IActionListener() {

					@Override
					public void actionPerformed(Object o) {
						int type = (Integer) ((Object[]) o)[0];
						Item item = (Item) ((Object[]) o)[1];
						if (item != null) {
							Array<Integer> userIDs = new Array<Integer>();
							if (type == 0) {
								if (giftDialog.sendGiftuserID != -1)
									userIDs.add(giftDialog.sendGiftuserID);
							} else {
								for (int i = GameController.currentGameScreen.players.size; --i >= 0;) {
									userIDs.add(GameController.currentGameScreen.players.get(i).IDDB);
								}
							}
							if (userIDs.size > 0) {
								GameService.giveItem(GameController.currentGameScreen.roomID,
										GameController.currentGameScreen.boardID, item.id, userIDs);
							}
							giftDialog.remove();
						} else {
							DialogUtil.startOKDlg(L.gL(332));
						}
					}
				};
				giftDialog.setGiftCategorys(categories);
			} else {
				giftDialog.initForResuse();
			}
			giftDialog.sendGiftuserID = sendUserId;

			giftDialog.show(GameController.currentGameScreen);
		}
		// giftDialog.setGiftCategorys(categories);
	
	*/
}
void HUDScreen::setGiftItems(int categoryID, int totalpage, std::vector<Item*> items)
{
	/*
	giftDialog.setGiftItems(categoryID, totalpage, items);
	*/
}
void HUDScreen::sendGiftTo(int from, std::vector<int> tos, int itemID)
{
	/*

	int startX, startY;
		PlayerGroup pg = getPlayerGroup(from);
		if (pg == null) {
			startX = (int) (rectBoard.getX() + rectBoard.getWidth() / 2);
			startY = (int) rectBoard.getY();
		} else {
			startX = (int) (pg.getX() + pg.getWidth() / 2);
			startY = (int) (pg.getY() + pg.getHeight() / 2);
		}
		final Image[] images = new Image[tos.size];
		TextureRegion t = ImageService.getItemImage(itemID, new IRequestActionCallBack() {

			@Override
			public void onSucceed(Object o) {
				for (int i = images.length; --i >= 0;) {
					TextureRegionDrawable aa = new TextureRegionDrawable((TextureRegion) o);
					images[i].setDrawable(aa);
					images[i].setSize(aa.getMinWidth(), aa.getMinHeight());
				}
			}

			@Override
			public void onFail(Object o) {

			}
		});
		// t.setRegionWidth(width);

		TextureRegionDrawable tt = new TextureRegionDrawable(t);

		for (int i = 0; i < tos.size; i++) {
			images[i] = new Image(tt);
			final PlayerGroup pg1 = getPlayerGroup(tos.get(i));
			images[i].setPosition(startX, startY);
			images[i].setSize(GameController.game.commonAsset.gamescreen_game_common_tang_qua.getMinWidth(),
					GameController.game.commonAsset.gamescreen_game_common_tang_qua.getMinHeight());

			if (pg1 != null) {
				if (pg1.getPlayer() != null)
					pg1.getPlayer().giftID = itemID;
				final Image temp = images[i];
				images[i]
						.addAction(
								Actions.sequence(
										Actions.moveTo(pg1.getX() + pg1.getGift().getX(),
												pg1.getY() + pg1.getGift().getY(), GIFT_FLY_TIME),
										new TemporalAction() {

											@Override
											protected void update(float percent) {
												if (pg1.getPlayer() != null && pg1.getPlayer().IDDB != -1) {
													pg1.getGift().setDrawable(temp.getDrawable());
													pg1.getGift().setVisible(true);
												}

												// if (VIEWPORT_HEIGHT < 480)
												// pg1.getGift().setSize(
												// GameController.game.commonAsset.gamescreen_game_common_tang_qua.getMinWidth()
												// * 0.5f,
												// GameController.game.commonAsset.gamescreen_game_common_tang_qua.getMinHeight()
												// * 0.5f);
												// else
												// pg1.getGift().setSize(temp.getWidth(),
												// temp.getHeight());
												// pg1.getGift().debug();

												// pg1.getGift().debug();
												temp.remove();

												removeGiftToUpdate(temp);
												GiftInfo gi = new GiftInfo();
												gi.giftActor = pg1.getGift();
												addGiftToUpdate(gi);
											}
										}));
			} else if (myInfo != null && tos.get(i) == myInfo.IDDB) {
				final Image temp = images[i];
				images[i].addAction(Actions.sequence(Actions.moveTo(myGift.getX(), myGift.getY(), GIFT_FLY_TIME),
						new TemporalAction() {

							@Override
							protected void update(float percent) {
								myGift.setDrawable(temp.getDrawable());
								myGift.setVisible(true);

								myGift.setSize(temp.getWidth(), temp.getHeight());
								gameLayer.addActor(myGift);
								temp.remove();
								removeGiftToUpdate(temp);
								GiftInfo gi = new GiftInfo();
								gi.giftActor = myGift;
								addGiftToUpdate(gi);
							}
						}));
			}
			GiftInfo gi = new GiftInfo();
			gi.giftActor = images[i];
			addGiftToUpdate(gi);

			giftMgr.add(images[i]);
			gameLayer.addActor(images[i]);
		}
	
	*/
}

int HUDScreen::GetStateHUDScreen()
{
	if (p_panel_ig_setting->IsVisible())
	{
		return 0;
	}
	return 1;
}