#include "LobbyScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wButton.h"

#include "Network/Global/Globalservice.h"
#include "Common/GameController.h"
#include "Social/SocialManager.h"
#include "Utils.h"
#include "../Network/Game/GameService.h"

LobbyScreen::LobbyScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_player_lobby = nullptr;
	p_panel_create_room = nullptr;
	p_list_number_player_choose = nullptr;
	p_list_player_in_lobby.clear();
	p_layout_right = nullptr;
	p_panel_avatar = nullptr;

	p_list_avatar_download.clear();
}

LobbyScreen::~LobbyScreen()
{
	p_list_player_lobby = nullptr;
	p_panel_create_room = nullptr;
	p_list_number_player_choose = nullptr;
	p_list_player_in_lobby.clear();

	p_layout_right = nullptr;
	p_panel_avatar = nullptr;

	CleanUpListAvatar();

}

int LobbyScreen::Init()
{
	RKString _menu = "lobby_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	CallInitComponent(true);

	return 1;
}

int	LobbyScreen::InitComponent()
{
	p_list_player_lobby = GetWidgetChildByName("lobby_ui.main_lobby_layout.left_layout.player_lobby_layout.list_player");
	p_panel_create_room = GetWidgetChildByName("lobby_ui.create_room_layout.create_room_panel");
	p_list_number_player_choose = p_panel_create_room->GetWidgetChildByName(".layout_number_player.list_choose");

	p_layout_right = GetWidgetChildByName("lobby_ui.main_lobby_layout.right_layout");

	p_panel_avatar = p_layout_right->GetWidgetChildByName(".panel_avatar.avatar_default");
	p_layout_pull_reload_player = p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".layout_pull_update");
	auto list_player_lobby = static_cast<ListViewTableWidget*>(p_list_player_lobby);
	list_player_lobby->addEventListener(
		[this, list_player_lobby](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = list_player_lobby->GetCurrentIdxTo();
			SetInfoWidgetList(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			size_t current_idx = list_player_lobby->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < p_list_player_in_lobby.size())
			{
				auto idx_player = p_list_player_in_lobby.at(current_idx);

				int room_id = idx_player.getRoomId();
				int board_id = idx_player.getBoardId();
				bool password = idx_player.getPassword();
				if (!password)
				{
					GlobalService::joinBoard(room_id, board_id, "");
				}
			}
			else
			{
				PASSERT2(false, "have problem here!");
			}

		}
		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_reload_player->Visible(true);
				this->p_layout_pull_reload_player->SetActionCommandWithName("ORIGIN_POS");
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_reload_player->GetPosition();
				curpos -= list_player_lobby->GetDeltaPos();
				this->p_layout_pull_reload_player->SetPosition(curpos);

				float percentmove = list_player_lobby->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				if (list_player_lobby->HaveEndDrag())
				{
					GetSound->PlaySoundEffect("SFX_UPDATELIST");
					//reload list here
					WidgetEntity* layout_find_bet = p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".layout_find_bet");
					int money = atoi(static_cast<TextFieldWidget*>(layout_find_bet->GetWidgetChildByName(".panel_bet_number_input.title_bet_number_input_text"))->GetText().GetString());
					doRequestRoom(money);
				}
			}
		}
		else if (type == EventType_SetCell::ON_DROP_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_reload_player->GetPosition();
				curpos -= list_player_lobby->GetDeltaPos();
				this->p_layout_pull_reload_player->SetPosition(curpos);

				float percentmove = list_player_lobby->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_reload_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (list_player_lobby->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_reload_player->Visible(false);
			}

		}

	});
	list_player_lobby->InitTable();

	WidgetEntity* layout_find_bet = p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".layout_find_bet");
	p_slider_choose_bet = layout_find_bet->GetWidgetChildByName(".slider_bet_array");
	p_slider_bet_create_room = p_panel_create_room->GetWidgetChildByName(".layout_slider_bet.slider_bet_array_create_game");

	return 1;
}

int LobbyScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
	return 1;
}

void LobbyScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void LobbyScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void LobbyScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_create_game" )
		{
			GetWidgetChildByName("lobby_ui.background_texture")->Visible(true);
			p_panel_create_room->GetParentWidget()->Visible(true);
			p_panel_create_room->GetParentWidget()->SetActionCommandWithName("ON_SHOW");
			GetLayerChildByName("lobby_ui")->SetInteractive(false);
			p_panel_create_room->GetParentWidget()->SetInteractive(true);
		}
		else if (name == "btn_back" )
		{
			ScrMgr->SwitchToMenu(MAIN_SCREEN, LOBBY_SCREEN);
		}
		else if (name == "close_create_room_btn" )
		{
			if (p_list_number_player_choose->Visible())
			{
				p_list_number_player_choose->Visible(false);
			}
			p_panel_create_room->GetParentWidget()->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_panel_create_room->GetParentWidget()->ForceFinishAction();
				this->p_panel_create_room->GetParentWidget()->Visible(false);
				GetWidgetChildByName("lobby_ui.background_texture")->Visible(false);
			}
			));
			GetLayerChildByName("lobby_ui")->SetInteractive(true);
		}
		else if (name == "close_create_room_btn" )
		{

			p_panel_create_room->GetParentWidget()->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_panel_create_room->GetParentWidget()->ForceFinishAction();
				this->p_panel_create_room->GetParentWidget()->Visible(false);
				this->GetLayerChildByName("lobby_ui")->SetInteractive(true);
			}
			));
		}
		else if (name == "btn_create_room")
		{
			if (p_list_number_player_choose->Visible())
			{
				p_list_number_player_choose->Visible(false);
			}
			std::string money = static_cast<TextFieldWidget*>(p_panel_create_room->GetWidgetChildByName(".layout_number_bet.panel_bet_number_input.bet_number_input_create_game"))->GetText().GetString();
			std::string num_players = static_cast<TextWidget*>(p_panel_create_room->GetWidgetChildByName(".layout_number_player._title_number"))->GetText().GetString();

			std::string pass = static_cast<TextFieldWidget*>(p_panel_create_room->GetWidgetChildByName(".layout_password.panel_pass_number_input.title_pasword_input_text"))->GetText().GetString();
			//GlobalService::requestCreateBoard(0, atoi(money.c_str()), atoi(num_players.c_str()), pass );
			//GlobalService::requestCreateBoard(0, 0, atoi(num_players.c_str()), pass );
			p_panel_create_room->GetParentWidget()->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this, num_players, pass, money]()
			{

				this->p_panel_create_room->GetParentWidget()->ForceFinishAction();
				this->p_panel_create_room->GetParentWidget()->Visible(false);
				GetWidgetChildByName("lobby_ui.background_texture")->Visible(false);
				this->GetLayerChildByName("lobby_ui")->SetInteractive(true);
				GlobalService::requestCreateBoard(0, Utils::GetNumberFromFormat(money.c_str()), atoi(num_players.c_str()), pass);
			}
			));
		}
		else if (name == "btn_bet_search" )
		{
			doRequestRoom(p_current_money_bet);
		}
		else if (name == "choose_number_player_btn" )
		{
			if (p_list_number_player_choose->Visible())
			{
				p_list_number_player_choose->SetActionCommandWithName("ON_HIDE");
			}
			else
			{
				p_list_number_player_choose->SetActionCommandWithName("ON_SHOW");
			}
		}
		else if (name == "choose_btn" )
		{
			static_cast<ListViewWidget*>(p_list_number_player_choose)->ForceVisibleAllChildLayout(".hight_light", false);
			_widget->GetParentWidget()->GetWidgetChildByName(".hight_light")->Visible(true);
			RKString current_text = static_cast<TextWidget*>(_widget->GetParentWidget()->GetWidgetChildByName(".title"))->GetText();
			static_cast<TextWidget*>(
				p_list_number_player_choose->GetParentWidget()->GetWidgetChildByName("._title_number"))->SetText(
					current_text
				);
			p_list_number_player_choose->SetActionCommandWithName("ON_HIDE");
		}
		else if (name == "btn_play_now")
		{
			GameService::joinAnyBoard();
		}
		else if (name == "btn_top_rich")
		{
			ShowRichestWin();
		}
		else if (name == "btn_top_player")
		{
			ShowStrongest();
		}
		else if (name == "btn_clear_bet" || name=="btn_clear_bet_create_game")
		{
			static_cast<TextFieldWidget*>(_widget->GetParentLayer()->GetWidgetChildAtIdx(0))->ClearText();
			_widget->Visible(false);
		}
		
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		if (name == "slider_bet_array_PERCENTAGE_CHANGED")
		{
			float value = static_cast<SliderWidget*>(_widget)->GetPercent();
			int part = static_cast<SliderWidget*>(_widget)->GetPart();
			p_current_money_bet = Utils::GetBetValueNew((value / 100.f) * part);

			WidgetEntity* layout_find_bet = p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".layout_find_bet");
			static_cast<TextFieldWidget*>(layout_find_bet->GetWidgetChildByName(".panel_bet_number_input.title_bet_number_input_text"))->SetText(Utils::formatNUmber_dot(p_current_money_bet));
		}
		else if (name == "slider_bet_array_create_game_PERCENTAGE_CHANGED")
		{
			float value = static_cast<SliderWidget*>(_widget)->GetPercent();
			int part = static_cast<SliderWidget*>(_widget)->GetPart();
			auto _current_money_bet = Utils::GetBetValueNew((value / 100.f) * part);

			static_cast<TextFieldWidget*>(p_panel_create_room->GetWidgetChildByName(".layout_number_bet.panel_bet_number_input.bet_number_input_create_game"))->SetText(Utils::formatNUmber_dot(_current_money_bet));
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "title_bet_number_input_text_DETACH_IME")
		{
			s64  money = Utils::GetNumberFromFormat(static_cast<TextFieldWidget*>(_widget)->GetText().GetString());
			bool setmax = false;
			if (money >= (int)(GameController::myInfo->money))
            {
				p_current_money_bet = Utils::getMaxValidBetNew(GameController::myInfo->money);
				setmax = true;
            }
			else
            {
				p_current_money_bet = Utils::getMaxValidBetNew(money);
            }
			static_cast<TextFieldWidget*>(_widget)->SetText(Utils::formatNUmber_dot(p_current_money_bet));
			float percent = 0;
			if (static_cast<SliderWidget*>(p_slider_choose_bet)->GetPart() > 0)
			{
				percent = (Utils::GetIndexOfValue(p_current_money_bet) * 100) / static_cast<SliderWidget*>(p_slider_choose_bet)->GetPart();
				if (percent > 100 || setmax)
				{
					percent = 100;
				}
			}
			static_cast<SliderWidget*>(p_slider_choose_bet)->SetPercent(percent);
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else if (name == "title_bet_number_input_text_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
		}
		else if (name == "title_bet_number_input_text_INSERT_TEXT")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();				
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
		}
		else if (name == "title_bet_number_input_text_DELETE_BACKWARD")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		}

		else if (name == "bet_number_input_create_game_DETACH_IME")
		{
			s64  money = Utils::GetNumberFromFormat(static_cast<TextFieldWidget*>(_widget)->GetText().GetString());
			bool setmax = false;
			s64 current_money_bet = 0;
			if (money >= (int)(GameController::myInfo->money))
			{
				current_money_bet = Utils::getMaxValidBetNew(GameController::myInfo->money);
				setmax = true;
			}
			else
			{
				current_money_bet = Utils::getMaxValidBetNew(money);
			}
			static_cast<TextFieldWidget*>(_widget)->SetText(Utils::formatNUmber_dot(current_money_bet));
			float percent = 0;
			if (static_cast<SliderWidget*>(p_slider_bet_create_room)->GetPart() > 0)
			{
				float percent = (Utils::GetIndexOfValue(current_money_bet) * 100) / static_cast<SliderWidget*>(p_slider_bet_create_room)->GetPart();
				if (percent > 100 || setmax)
				{
					percent = 100;
				}
			}
			static_cast<SliderWidget*>(p_slider_bet_create_room)->SetPercent(percent);
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else if (name == "bet_number_input_create_game_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
		}
		else if (name == "bet_number_input_create_game_INSERT_TEXT")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
		}
		else if (name == "bet_number_input_create_game_DELETE_BACKWARD")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		}

	}
	
}



void LobbyScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(LOBBY_SCREEN);


	p_list_number_player_choose->ClearChild(0);
	p_list_number_player_choose->Visible(false);

	p_current_idx_add_ = -1;
	p_menu_show_next = MENU_NONE;
}

void LobbyScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void LobbyScreen::OnBeginFadeIn()
{
	int number_max_player = GameController::getMaxPlayerOf(GameController::getCurrentGameType());
	int begin_player = 2;

	for (int i = number_max_player; i >= begin_player; i--)
	{
		auto widget = static_cast<ListViewWidget*>(p_list_number_player_choose)->CreateWidgetRefer(i);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".title"))->SetText(std::to_string(i), true);
		widget->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, widget->GetWidgetChildByName(".choose_btn"));
	}
	static_cast<ListViewWidget*>(p_list_number_player_choose)->ForceVisibleAllChildLayout(".hight_light", false);
	p_list_number_player_choose->GetWidgetChildAtIdx(0)->GetWidgetChildByName(".hight_light")->Visible(true);
	static_cast<TextWidget*>(
		p_list_number_player_choose->GetParentWidget()->GetWidgetChildByName("._title_number"))->SetText(
		std::to_string(number_max_player), true
	);

	auto w_hl_1 = p_layout_right->GetWidgetChildByName(".layout_play_now.line_glow_1");
	w_hl_1->SetActionCommandWithName("MOVE");
	auto w_hl_2 = p_layout_right->GetWidgetChildByName(".layout_play_now.line_glow_2");
	w_hl_2->SetActionCommandWithName("MOVE");
	auto star_hl = p_layout_right->GetWidgetChildByName(".layout_play_now.star_glow");
	star_hl->SetActionCommandWithName("ROTATE");

	s64 current_money = GameController::myInfo->money;
	p_max_money = 0;
	p_min_money = 0;
	p_delta_choose_bet = Utils::GetRangeBetWithMoney(p_min_money, p_max_money, current_money, 0);
	static_cast<SliderWidget*>(p_slider_choose_bet)->SetPart(p_delta_choose_bet);
	static_cast<SliderWidget*>(p_slider_bet_create_room)->SetPart(p_delta_choose_bet);
	p_current_money_bet = p_min_money;

	static_cast<TextWidget*>(p_slider_bet_create_room->GetParentWidget()->GetWidgetChildByName(".bet_title_begin"))->SetText(Utils::formatNumber(p_min_money));
	static_cast<TextWidget*>(p_slider_bet_create_room->GetParentWidget()->GetWidgetChildByName(".bet_title_end"))->SetText(Utils::formatNumber(p_max_money));

}

void LobbyScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	//float value = static_cast<SliderWidget*>(_widget)->GetPercent();
	doRequestRoom(0);
}

//@PDT: TODO
/**
* thuc hien gui len server tim ban
*
* @param money
*/
void LobbyScreen::doRequestRoom(int money) {
	//Gdx.app.log(TAG, "doRequestRoom: " + money);
	//if (GameController::gameId > -1) {
	//	// ph?i set game type r?i m?i request
	//	listPickRoom.hideCenterMessage();
	//	showLoadingCircle();
		GlobalService::requestOwnerBoardList(money);
	//}
	//else {
	//	listPickRoom.onRefreshComplete();
	//}

}


//@PDT: TODO
void LobbyScreen::onSetGameTypeSuccessed()
{
	//showLoadingCircle(false, groupCenter);

	//setTitle(AbstractScreen.VIEWPORT_WIDTH > 480
	//	? GlobalService.currentServer.name + " - " + GameController.getGameName(GameController.gameId)
	//	: sortStringName(GlobalService.currentServer.name) + " - "
	//	+ GameController.getGameName(GameController.gameId));

	//// TrackingService.trackScreen(getScreenName());

	//GameController.isLoadResourceGameFinish = false;

	////

	//loadResourceGame();

	////

	//final long myMoney = GameController.myInfo->getMoney();
	//sliderSearchBetMoney.setRange(0, getMaxAvailableBetIndex(myMoney));

	//int findBoardMoney = autoJoinGameFindBoardBetMoney > -1 ? autoJoinGameFindBoardBetMoney : -1;
	//autoJoinGameFindBoardBetMoney = -1;// gan lai -1 de lan sau khong lay
	//								   // gia tri nay nua

	//int index;
	//if (findBoardMoney > -1) {
	//	currentBet = findBoardMoney;
	//	// lần đâu tiên vô từ tự động đăng nhập, set mức cược này
	//	index = getMaxAvailableBetIndex(findBoardMoney);
	//	sliderSearchBetMoney.setValue(index);
	//	txtSearchBetMoney.setText("" + Util.formatNumber_dot(findBoardMoney));
	//	txtSearchBetMoney.setCursorPosition(0);
	//}
	//else {
	//	index = Math.round(sliderSearchBetMoney.getMaxValue() / 3);
	//	boolean update = sliderSearchBetMoney.setValue(index);
	//	if (!update) {
	//		ChangeEvent event = Pools.obtain(ChangeEvent.class);
	//		onSliderSearchBetMoneyChanged.changed(event, sliderSearchBetMoney);
	//		Pools.free(event);
	//	}
	//}
	//listPickRoom.doRefreshTop();

	//if (GameController.getGameEventList() == null) {
	//	GlobalService.getGameEventList();
	//	showLoadingCircle();
	//	groupCenter.setTouchable(Touchable.disabled);
	//}
	//else {
	//	updateGameEvent();
	//}

	//doRequestRoom(0);

	ScrMgr->setBtnTaixiuDirty();
}

void LobbyScreen::SetInfoWidgetList(WidgetEntity * layout, int i)
{
	if (i % 2 == 0)
	{
		layout->GetWidgetChildByName(".hl_panel")->Visible(true);
	}
	else
	{
		layout->GetWidgetChildByName(".hl_panel")->Visible(false);
	}
	if (i < 0 || (size_t)i >= p_list_player_in_lobby.size())
		return;
	auto idx_player = p_list_player_in_lobby.at(i);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_id"))->SetText(idx_player.getUsername() , true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber(idx_player.getMoney()) + " Win", true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_game_achieve.number_win"))->SetText(std::to_string(idx_player.getWin()), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_game_achieve.number_lose"))->SetText(std::to_string(idx_player.getLose()), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_game_achieve.number_draw"))->SetText(std::to_string(idx_player.getDraw()), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_game_achieve.number_leave"))->SetText(std::to_string(idx_player.getLeave()), true);

	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_game_info.number_1"))->SetText(Utils::formatNumber(idx_player.getBoardMoney()), true);
	SetBoardLevel(layout->GetWidgetChildByName(".layout_game_info.number_2"), idx_player.getBoardLevel());
	SetNumberPlayerInBoard(layout->GetWidgetChildByName(".layout_game_info"), idx_player.getCountPlayer(), idx_player.getCountMaxPlayer());

	auto panel_avatar = layout->GetWidgetChildByName(".panel_avatar");

	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(idx_player.getUsername()) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(idx_player.getUsername()), 100);
	}		

	layout->GetWidgetChildByName(".icon_lock")->Visible(idx_player.getPassword());
}

void LobbyScreen::onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave)
{
	if (p_list_player_in_lobby.size() > 0)
	{
		static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
		CleanUpListAvatar();
		p_list_player_in_lobby.clear();
	}

	p_list_player_in_lobby = boards;
	//parse user

	static_cast<ListViewTableWidget*>(p_list_player_lobby)->SetNumberSizeOfCell(p_list_player_in_lobby.size());

	//download avatar in save into the list
	for (size_t i = 0; i < p_list_player_in_lobby.size(); i++)
	{
		auto idx_player = p_list_player_in_lobby[i];
		{
			GameMgr->HandleAvatarDownload(idx_player.getAvatar(), idx_player.getUsername(),
				[this, idx_player, i](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (this->p_list_player_in_lobby.size() > 0)
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
					this->p_list_avatar_download.insert(idx_player.getUsername(), sprite_img);

					auto widget_lb = static_cast<ListViewTableWidget*>(p_list_player_lobby);
					if ((size_t)i < widget_lb->GetListWidgetSample().size())
					{
						auto widget = widget_lb->GetListWidgetSample()[i];
						if (widget)
						{
							auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar");
							this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						}
					}
				}
			});
		}
	}

	auto layout = p_layout_right;
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_win_detail.title_number_detail"))->SetText(StringUtils::format("%d", win ), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_lose_detail.title_number_detail"))->SetText(StringUtils::format("%d", lose ), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_draw_detail.title_number_detail"))->SetText(StringUtils::format("%d", draw ), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".layout_quit_detail.title_number_detail"))->SetText(StringUtils::format("%d", leave ), true);
	
	//parse list player

}

void LobbyScreen::SetTextTitle(RKString title)
{
	static_cast<TextWidget*>(GetWidgetChildByName("lobby_ui.top_panel.title_1"))->SetText(title, true);
}

void LobbyScreen::SetBoardLevel(WidgetEntity * text, int level)
{
	if (level == 0)
	{
		static_cast<TextWidget*>(text)->SetText("popular");
		text->SetColor(Color3B::WHITE);
	}
	else if (level == 1)
	{
		static_cast<TextWidget*>(text)->SetText("millionaire");
		text->SetColor(Color3B(238, 150, 56));
	}
	else
	{
		static_cast<TextWidget*>(text)->SetText("vip");
		text->SetColor(Color3B(245,70,52));
	}
}

void LobbyScreen::SetNumberPlayerInBoard(WidgetEntity * panel, int number_player, int max_player)
{
	for (int i = 0; i < 5; i++)
	{
		panel->GetWidgetChildByName(".layout_player_count_" + std::to_string(i + 2))->Visible(false);
	}
	auto panel_handle = panel->GetWidgetChildByName(".layout_player_count_" + std::to_string(max_player));
	panel_handle->Visible(true);
	for (int i = 1; i <= max_player; i++)
	{
		Color3B color = Color3B::WHITE;
		if (i <= number_player)
		{
			color = Color3B(17,243,70);
		}
		panel_handle->GetWidgetChildByName(".symbol_" + std::to_string(i))->SetColor(color);
	}
}

void LobbyScreen::CleanUpListAvatar()
{
	p_list_avatar_download.clear();
}

void LobbyScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}
	auto layout = p_layout_right;

	RKString nick_name = user->nickName;

	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_win_number"))->SetText(Utils::formatNumber_win(GameController::myInfo->money), true);
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_id"))->SetText(nick_name, true);
	
	auto panel_avatar = layout->GetWidgetChildByName(".panel_avatar");

	GameMgr->HandleAvatarDownload(user->avatarID, user->nickName,
		[this, panel_avatar](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(panel_avatar, data, str, tag);
	});
}

void LobbyScreen::ResetLeaderboard(bool cleanup)
{
	GetCommonScr->LB_ResetLeaderboard(cleanup);
}

void LobbyScreen::ShowRichestWin()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowRichestWin();
}

void LobbyScreen::ShowRichestRuby()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowRichestRuby();
}

void LobbyScreen::ShowStrongest()
{
	auto scr_common = GetCommonScr;
	scr_common->SetScreenComeFrom(this);
	GetCommonScr->LB_ShowStrongest(p_current_game_id);
}

void LobbyScreen::SetCurrentGameID(int id)
{
	p_current_game_id = id;

	if (p_previous_game_id != id)
	{
		ResetLeaderboard();

		static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
		CleanUpListAvatar();
		p_list_player_in_lobby.clear();
	}
	p_previous_game_id = id;
}


