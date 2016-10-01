#include "LeaderboardScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wLoadingBar.h"

#include "Network/JsonObject/Game/GameList.h"
#include "MainScreen.h"
#include "Models/Player.h"
#include "Network/IwinProtocol.h"
#include "Network/Global/GLobalService.h"

LeaderboardScreen::LeaderboardScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_player_leaderboard = nullptr;
	p_list_choose_leaderboard = nullptr;
	p_list_friend_leaderboard = nullptr;

	p_panel_list_choose = nullptr;
	p_panel_list_player = nullptr;

	p_tab_change_btn = nullptr;
	p_layout_top_list_choose = nullptr;
	p_layout_top_list_player = nullptr;

	p_is_on_tab_world = true;
	p_already_set_listgamelb = false;
	p_current_list_player_lb.clear();
	p_current_list_friend_lb.clear();

	p_current_page_ = 0;
	p_current_rank_idx = 0;
	p_current_rank_idx_friend = 0;

	p_list_avatar_download.clear();
	p_list_avatar_friend.clear();
	p_panel_user = nullptr;

	p_layout_pull_update_player = nullptr;
	p_layout_pull_loadmore_player = nullptr;
}


LeaderboardScreen::~LeaderboardScreen()
{
	p_list_player_leaderboard = nullptr;
	p_list_player_leaderboard = nullptr;
	p_list_friend_leaderboard = nullptr;

	p_panel_list_choose = nullptr;
	p_panel_list_player = nullptr;
	p_layout_top_list_choose = nullptr;
	p_layout_top_list_player = nullptr;

	p_tab_change_btn = nullptr;
}

int LeaderboardScreen::Init()
{
	RKString _menu = "leaderboard_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	
	p_panel_list_player = GetWidgetChildByName("leaderboard_ui.main_leaderboard_layout");
	p_list_player_leaderboard = p_panel_list_player->GetWidgetChildByName(".leaderboard.world_leaderboard.list_player_lb");
	p_list_friend_leaderboard = p_panel_list_player->GetWidgetChildByName(".leaderboard.friend_leaderboard.list_player_lb");

	auto lb_player = static_cast<ListViewTableWidget*>(p_list_player_leaderboard);
	lb_player->addEventListener(
		[this, lb_player](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		auto current_drag = lb_player->current_stage_drag();
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = lb_player->GetCurrentIdxTo();
			OnParsePlayerToLeaderboard(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			int current_idx_cell = lb_player->GetCurrentIdxSelected();
			if (current_idx_cell < 0 || (size_t)current_idx_cell >= p_current_list_player_lb.size())
			{
				PASSERT2(current_idx_cell >= 0 && (size_t)current_idx_cell < p_current_list_player_lb.size(), "idx out of range!");
				return;
			}
			Player * player = p_current_list_player_lb[current_idx_cell];
			auto scr_common = GetCommonScr;
			scr_common->SetScreenComeFrom(this);
			scr_common->OnShowPlayerInfo(player);
		}

		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_player->Visible(true);
				this->p_layout_pull_update_player->SetActionCommandWithName("ORIGIN_POS");
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_UP)
			{
				this->p_layout_pull_loadmore_player->Visible(true);
				this->p_layout_pull_loadmore_player->SetActionCommandWithName("ORIGIN_POS");
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update_player->GetPosition();
				curpos -= lb_player->GetDeltaPos();
				this->p_layout_pull_update_player->SetPosition(curpos);

				float percentmove = lb_player->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
			else
			{
				Vec2 curpos = this->p_layout_pull_loadmore_player->GetPosition();

				curpos -= lb_player->GetDeltaPos();
				this->p_layout_pull_loadmore_player->SetPosition(curpos);

				float percentmove = lb_player->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}

		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_UP)
			{
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				if (lb_player->HaveEndDrag())
				{
					//reload list here
					ResetLeaderboard(0);
					if (p_current_stage_lb == 0)
					{
						GlobalService::requestRichestWin(this->p_current_page_++);
					}
					else if (p_current_stage_lb == 1)
					{
						GlobalService::requestStrongest(this->p_current_page_++, this->p_current_game_id);
					}
					else if (p_current_stage_lb == 2)
					{
						GlobalService::requestRichestRuby(this->p_current_page_++);
					}
				}
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				if (lb_player->HaveEndDrag())
				{
					if (p_current_stage_lb == 0)
					{
						GlobalService::requestRichestWin(this->p_current_page_++);
					}
					else if (p_current_stage_lb == 1)
					{
						GlobalService::requestStrongest(this->p_current_page_++, this->p_current_game_id);
					}
					else if (p_current_stage_lb == 2)
					{
						GlobalService::requestRichestRuby(this->p_current_page_++);
					}
				}
			}
			GetSound->PlaySoundEffect("SFX_UPDATELIST");
		}
		else if (type == EventType_SetCell::ON_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update_player->GetPosition();
				curpos -= lb_player->GetDeltaPos();
				this->p_layout_pull_update_player->SetPosition(curpos);

				float percentmove = lb_player->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_loadmore_player->GetPosition();
				curpos -= lb_player->GetDeltaPos();
				this->p_layout_pull_loadmore_player->SetPosition(curpos);

				float percentmove = lb_player->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_player->Visible(false);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				this->p_layout_pull_loadmore_player->Visible(false);
			}

		}
	});

	lb_player->InitTable();


	auto lb_friend = static_cast<ListViewTableWidget*>(p_list_friend_leaderboard);
	lb_friend->addEventListener(
		[this, lb_friend](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = lb_friend->GetCurrentIdxTo();
			OnParsePlayerToLeaderboard(_widget, current_idx_to, false);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			int current_idx_cell = lb_friend->GetCurrentIdxSelected();
			if (current_idx_cell < 0 || (size_t)current_idx_cell >= p_current_list_friend_lb.size())
			{
				PASSERT2(current_idx_cell >= 0 && (size_t)current_idx_cell < p_current_list_friend_lb.size(), "idx out of range!");
				return;
			}
			Player * player = p_current_list_friend_lb[current_idx_cell];
			auto scr_common = GetCommonScr;
			scr_common->SetScreenComeFrom(this);
			scr_common->OnShowPlayerInfo(player);
		}

		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_friend->Visible(true);
				this->p_layout_pull_update_friend->SetActionCommandWithName("ORIGIN_POS");
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}

		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update_friend->GetPosition();
				curpos -= lb_friend->GetDeltaPos();
				this->p_layout_pull_update_friend->SetPosition(curpos);

				float percentmove = lb_friend->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}

		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				if (lb_friend->HaveEndDrag())
				{
					GetSound->PlaySoundEffect("SFX_UPDATELIST");
					//reload list here
					ResetLeaderboard(1);
					if (p_current_stage_lb == 0)
					{
						GlobalService::requestRichFriendList();
					}
					else if (p_current_stage_lb == 1)
					{
						GlobalService::requestStrongFriendList(this->p_current_game_id);
					}
					else if (p_current_stage_lb == 2)
					{
						GlobalService::requestRichFriendList();
					}
				}
			}

		}
		else if (type == EventType_SetCell::ON_DROP_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_layout_pull_update_friend->GetPosition();
				curpos -= lb_friend->GetDeltaPos();
				this->p_layout_pull_update_friend->SetPosition(curpos);

				float percentmove = lb_friend->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_layout_pull_update_friend->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}

		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (lb_friend->current_stage_drag() == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_layout_pull_update_friend->Visible(false);
			}
		}
	});

	lb_friend->InitTable();


	CallInitComponent(true);

	return 1;
}

int	LeaderboardScreen::InitComponent()
{
	p_panel_list_choose = GetWidgetChildByName("leaderboard_ui.list_game_leaderboard");

	p_list_choose_leaderboard = p_panel_list_choose->GetWidgetChildByName(".list_leaderboard");
	p_panel_user = p_panel_list_player->GetWidgetChildByName(".left_layout");

	p_tab_change_btn = GetWidgetChildByName("leaderboard_ui.top_panel.layout_top_lb.tab_panel");

	p_layout_top_list_choose = GetWidgetChildByName("leaderboard_ui.top_panel.layout_top_list_lb");
	p_layout_top_list_player = GetWidgetChildByName("leaderboard_ui.top_panel.layout_top_lb");

	p_current_stage_lb = 0; //0 is richest lb, 1 is strongest lb, 2 is richest ruby lb
	p_current_game_id = 0;

	p_layout_pull_update_player = p_list_player_leaderboard->GetParentWidget()->GetWidgetChildAtIdx(1);
	p_layout_pull_loadmore_player = p_list_player_leaderboard->GetParentWidget()->GetWidgetChildAtIdx(2);

	p_layout_pull_update_friend = p_list_friend_leaderboard->GetParentWidget()->GetWidgetChildAtIdx(1);

	return 1;
}

int LeaderboardScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	auto main_screen = static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN));
	auto game_list = main_screen->GetGameList();
	if (game_list && !p_already_set_listgamelb)
	{
		int idx = 0;
		auto _widget = static_cast<ListViewWidget*>(p_list_choose_leaderboard)->CreateWidgetRefer(idx);
		RKString text_ = LangMgr->GetString("richest");
		if (ScrMgr->IsRubyState())
		{
			text_ = text_ + " Ruby";
		}
		else
		{
			text_ = text_ + " Win";
		}

		static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(text_ , true);
		static_cast<ButtonWidget*>(_widget->GetWidgetChildAtIdx(0))->SetCallBack([this , text_](Ref* ref, Widget::TouchEventType touch_type) {
			if (touch_type == Widget::TouchEventType::ENDED)
			{
				this->p_panel_list_choose->SetActionCommandWithName("ON_HIDE");
				this->p_layout_top_list_choose->SetActionCommandWithName("ON_HIDE");
				this->p_panel_list_player->SetActionCommandWithName("ON_SHOW");
				this->p_layout_top_list_player->SetActionCommandWithName("ON_SHOW");

				this->ResetLeaderboard(2);

				if (ScrMgr->IsRubyState())
				{
					GlobalService::requestRichestRuby(this->p_current_page_++);
					GlobalService::requestRichFriendList();
					p_current_stage_lb = 2;

				}
				else
				{
					GlobalService::requestRichestWin(this->p_current_page_++);
					GlobalService::requestRichFriendList();
					p_current_stage_lb = 0;
				}
				ScrMgr->SetScreenIDDirty();

				static_cast<TextWidget*>(this->p_layout_top_list_player->GetWidgetChildAtIdx(0))->SetText(text_, true);
			}
		});
		idx++;
		auto game_ = game_list->getGameCard();
		for (size_t i = 0; i < game_.size(); i++)
		{
			auto detail = ScrMgr->GetGameDetailByType((GameType)(game_[i]));
			CreateListGameLeaderboard(detail, idx);
			idx++;
		}
		game_ = game_list->getGameChess();
		for (size_t i = 0; i < game_.size(); i++)
		{
			auto detail = ScrMgr->GetGameDetailByType((GameType)(game_[i]));
			CreateListGameLeaderboard(detail, idx);
			idx++;
		}
		game_ = game_list->getGameOther();
		for (size_t i = 0; i < game_.size(); i++)
		{
			auto detail = ScrMgr->GetGameDetailByType((GameType)(game_[i]));
			CreateListGameLeaderboard(detail, idx);
			idx++;
		}

		p_already_set_listgamelb = true;
	}

	return 1;
}

void LeaderboardScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void LeaderboardScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void LeaderboardScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_back")
		{
			if (p_panel_list_choose->Visible())
			{
				ScrMgr->SwitchToMenu(p_menu_come_from, LEADERBOARD_SCREEN);
			}
			else
			{
				p_panel_list_choose->SetActionCommandWithName("ON_SHOW");
				p_panel_list_player->SetActionCommandWithName("ON_HIDE");

				p_layout_top_list_choose->SetActionCommandWithName("ON_SHOW");
				p_layout_top_list_player->SetActionCommandWithName("ON_HIDE");
			}
		}
		else if (name == "btn_world")
		{
			if (!p_is_on_tab_world)
			{
				p_tab_change_btn->SetActionCommandWithName("MOVE_LEFT");
				p_list_player_leaderboard->GetParentWidget()->SetActionCommandWithName("ON_SHOW");
				p_list_friend_leaderboard->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
				_widget->SetColor(Color3B::YELLOW);
				_widget->GetParentWidget()->GetWidgetChildByName(".btn_friend")->SetColor(Color3B::WHITE);
				p_is_on_tab_world = true;
			}
		}
		else if (name == "btn_friend")
		{
			if (p_is_on_tab_world)
			{
				p_tab_change_btn->SetActionCommandWithName("MOVE_RIGHT");
				p_list_player_leaderboard->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
				p_list_friend_leaderboard->GetParentWidget()->SetActionCommandWithName("ON_SHOW");
				_widget->SetColor(Color3B::YELLOW);
				_widget->GetParentWidget()->GetWidgetChildByName(".btn_world")->SetColor(Color3B::WHITE);
				p_is_on_tab_world = false;
			}
		}

	}
}

void LeaderboardScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(LEADERBOARD_SCREEN);
	p_menu_show_next = MENU_NONE;
}

void LeaderboardScreen::OnBeginFadeIn()
{
	auto _widget = static_cast<ListViewWidget*>(p_list_choose_leaderboard)->GetWidgetChildAtIdx(0);
	if (_widget)
	{
		RKString text_ = LangMgr->GetString("richest");
		if (ScrMgr->IsRubyState())
		{
			text_ = text_ + " Ruby";
		}
		else
		{
			text_ = text_ + " Win";
		}

		static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(text_, true);
	}
}

void LeaderboardScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void LeaderboardScreen::CreateListGameLeaderboard(ListGameDetail lg, int idx)
{
	auto detail = lg;
	auto _widget = static_cast<ListViewWidget*>(p_list_choose_leaderboard)->CreateWidgetRefer(idx);
	RKString text_refix = LangMgr->GetString("strongest");
	RKString text_ = LangMgr->GetString(detail.name);
	int game_id = detail.id;
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(text_refix + " " + text_, true);
	static_cast<ButtonWidget*>(_widget->GetWidgetChildAtIdx(0))->SetCallBack([this , text_ , game_id](Ref* ref, Widget::TouchEventType touch_type) {
		if (touch_type == Widget::TouchEventType::ENDED)
		{
			this->p_panel_list_choose->SetActionCommandWithName("ON_HIDE");
			this->p_layout_top_list_choose->SetActionCommandWithName("ON_HIDE");
			this->p_panel_list_player->SetActionCommandWithName("ON_SHOW");
			this->p_layout_top_list_player->SetActionCommandWithName("ON_SHOW");
			p_current_stage_lb = 1;
			ScrMgr->SetScreenIDDirty();
			this->p_current_game_id = game_id;
			this->ResetLeaderboard(2);
			RKString refix = LangMgr->GetString("charts");
			static_cast<TextWidget*>(this->p_layout_top_list_player->GetWidgetChildAtIdx(0))->SetText(refix + "-" + text_, true);
			
			GlobalService::requestStrongest(this->p_current_page_++, this->p_current_game_id);
			GlobalService::requestStrongFriendList(this->p_current_game_id);

		}
	});

}

void LeaderboardScreen::OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player)
{
	for (size_t i = 0; i < list_player.size(); i++)
	{

		p_current_list_player_lb.push_back(list_player[i]);
	
		size_t current_rank_idx = (size_t)p_current_rank_idx;
		p_current_rank_idx++;
		//download avatar in save into the list
			auto idx_player = list_player[i];
			{
				int id_avatar = idx_player->userProfile->avatarID;
				GameMgr->HandleAvatarDownload(idx_player->userProfile->avatarID, idx_player->name,
					[this , current_rank_idx, id_avatar, idx_player](void * data, void * str, int tag)
				{
					Image * img = (Image *)data;
					if (img && img->getWidth() <= 0)
					{
						CC_SAFE_DELETE(img);
						return;
					}
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
					this->p_list_avatar_download.insert(current_rank_idx, sprite_img);
					auto widget_lb = static_cast<ListViewTableWidget*>(p_list_player_leaderboard);
					if (current_rank_idx < widget_lb->GetListWidgetSample().size())
					{
						for (size_t i = 0; i < widget_lb->GetListWidgetSample().size(); i++)
						{
							auto widget = widget_lb->GetListWidgetSample()[i];
							auto current_name = static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->GetText();

							if (RKString(idx_player->name) != current_name)
								continue;

							if (widget)
							{
								auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
								this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
							}
							break;
						}
					}
				});
			}
	}

	switch (id)
	{
	case IwinProtocol::RICHEST_LIST:
	case IwinProtocol::RICHEST_RUBY_LIST:
	case IwinProtocol::STRONGEST_LIST:
	{
		size_t current_cell_number = static_cast<ListViewTableWidget*>(p_list_player_leaderboard)->GetNumberSizeOfCell();
		current_cell_number += list_player.size();
		static_cast<ListViewTableWidget*>(p_list_player_leaderboard)->SetNumberSizeOfCell(current_cell_number);
		break;
	}
	default:
		break;
	}
}

void LeaderboardScreen::OnReceiveListFriend(int id, ubyte page, std::vector<Player * > list_player)
{
	//
	//id = 0 is richest , id = 1 richest ruby , id = 2 strongest
	//
	for (size_t i = 0; i < list_player.size(); i++)
	{

		p_current_list_friend_lb.push_back(list_player[i]);

		size_t current_rank_idx = (size_t)p_current_rank_idx_friend;
		p_current_rank_idx_friend++;
		//download avatar in save into the list
		auto idx_player = list_player[i];
		{
			int id_avatar = idx_player->userProfile->avatarID;
			GameMgr->HandleAvatarDownload(idx_player->userProfile->avatarID, idx_player->name,
				[this, current_rank_idx, id_avatar , idx_player](void * data, void * str, int tag)
			{
				const char * str_ = (const char *)(str);
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
				this->p_list_avatar_friend.insert(current_rank_idx, sprite_img);
				auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_leaderboard);
				if (current_rank_idx < widget_lb->GetListWidgetSample().size())
				{
					for (size_t i = 0; i < widget_lb->GetListWidgetSample().size(); i++)
					{
						auto widget = widget_lb->GetListWidgetSample()[i];
						auto current_name = static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->GetText();

						if (RKString(idx_player->name) != current_name)
							continue;

						if (widget)
						{
							auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
							this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						}
						break;
					}
				}
			});
		}
	}

	switch (id)
	{
	case 0:
	case 1:
	case 2:
	{
		size_t current_cell_number = static_cast<ListViewTableWidget*>(p_list_friend_leaderboard)->GetNumberSizeOfCell();
		current_cell_number += list_player.size();
		static_cast<ListViewTableWidget*>(p_list_friend_leaderboard)->SetNumberSizeOfCell(current_cell_number);
		break;
	}
	default:
		break;
	}
}

void LeaderboardScreen::OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx, bool isPlayer)
{
	Player * player = nullptr;
	if (isPlayer)
	{
		if (idx < 0 || (size_t)idx >= p_current_list_player_lb.size())
		{
			PASSERT2(idx >= 0 && (size_t)idx < p_current_list_player_lb.size(), "idx out of range!");
			return;
		}
		player = p_current_list_player_lb[idx];
	}
	else
	{
		if (idx < 0 || (size_t)idx >= p_current_list_friend_lb.size())
		{
			PASSERT2(idx >= 0 && (size_t)idx < p_current_list_friend_lb.size(), "idx out of range!");
			return;
		}
		player = p_current_list_friend_lb[idx];
	}

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

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".stt_id"))->SetText(std::to_string(idx + 1) + "." , true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(player->name , true);
	if (ScrMgr->IsRubyState())
	{
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatMoney_dot_ruby(player->ruby), true);
	}
	else
	{
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatMoney_dot_win(player->money), true);
	}

	int current_percent = player->getPercent();
	auto process_bar = widget->GetWidgetChildByName(".progress_bar_panel");
	static_cast<LoadingBarWidget*>(process_bar->GetWidgetChildByName(".progress_bar_value"))->SetPercent(current_percent);
	static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_percent_value"))->SetText(std::to_string(current_percent) + "%" , true);
	RKString level = LangMgr->GetString("level");
	static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_level"))->SetText(level + " " + std::to_string(player->getLevel()) , true);
	
	auto tx = widget->GetWidgetChildByName(".status_id");
	if (player->userProfile->status != "")
	{
		tx->Visible(true);
		static_cast<TextWidget*>(tx)->SetText(player->userProfile->status , true);
	}
	else
	{
		tx->Visible(false);
	}

	auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
	if (isPlayer)
	{
		if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(idx) != p_list_avatar_download.end())
		{
			this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(idx), 100);
		}
	}
	else
	{
		if (p_list_avatar_friend.size() > 0 && p_list_avatar_friend.find(idx) != p_list_avatar_friend.end())
		{
			this->ParseSpriteToPanel(panel_avatar, p_list_avatar_friend.at(idx), 100);
		}
	}

}

void LeaderboardScreen::CleanUpListAvatar()
{

	p_list_avatar_download.clear();

	p_list_avatar_friend.clear();
}

void LeaderboardScreen::ResetLeaderboard(int state)
{
	if (state != 1)
	{
		static_cast<ListViewTableWidget*>(p_list_player_leaderboard)->ResetListView();
		p_current_rank_idx = 0;
		SAFE_DELETE_VECTOR(p_current_list_player_lb);
		p_list_avatar_download.clear();
	}
	if (state != 0)
	{
		static_cast<ListViewTableWidget*>(p_list_friend_leaderboard)->ResetListView();
		p_current_rank_idx_friend = 0;
		SAFE_DELETE_VECTOR(p_current_list_friend_lb);
		p_list_avatar_friend.clear();
	}
	p_current_page_ = 0;
}

void LeaderboardScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}
	Player * player = GameController::myInfo;
	auto layout = p_panel_user;

	RKString nick_name = user->nickName;

	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_win_number"))->SetText(Utils::formatNumber_win(GameController::myInfo->money), true);
	
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_ruby_number"))->SetText(Utils::formatNumber_ruby(GameController::myInfo->ruby), true);
	
	static_cast<TextWidget*>(layout->GetWidgetChildByName(".title_id"))->SetText(nick_name, true);
	
	int current_percent = player->getPercent();
	RKString level = LangMgr->GetString("level");
	auto process_bar = layout->GetWidgetChildByName(".progress_bar_panel");
	static_cast<LoadingBarWidget*>(process_bar->GetWidgetChildByName(".progress_bar_value"))->SetPercent(current_percent);
	static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_percent_value"))->SetText(std::to_string(current_percent) + "%", true);
	static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_level"))->SetText(level + " " + std::to_string(player->getLevel()), true);

	auto panel_avatar = layout->GetWidgetChildByName(".panel_avatar");

	GameMgr->HandleAvatarDownload(user->avatarID, user->nickName,
		[this, panel_avatar](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(panel_avatar, data, str, 100);
	});
}

int LeaderboardScreen::CurrentStateLeaderboard()
{
	return p_current_stage_lb;
}