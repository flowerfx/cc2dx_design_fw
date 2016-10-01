#include "LeaderboardLayer.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "InputManager.h"
#include "Screens/CommonScreen.h"

LeaderboardLayer::LeaderboardLayer()
{
	p_lb_layout_leaderboard = nullptr;
	p_lb_layout_leaderboard_player = nullptr;
	p_lb_layout_pull_update_player = nullptr;
	p_lb_layout_pull_loadmore_player = nullptr;

	_common_screen = nullptr;
}

LeaderboardLayer::~LeaderboardLayer()
{
	LB_ResetLeaderboard(true);
}

void LeaderboardLayer::LB_OnTouchMenuBegin(cocos2d::Point p)
{
	if (p_lb_layout_leaderboard->Visible() && !static_cast<CommonScreen*>(_common_screen)->IsPlayerInfoVisible())
	{
		_common_screen->CheckTouchOutSideWidget(p, p_lb_layout_leaderboard,
			[&](void) {
			p_lb_layout_leaderboard->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_lb_layout_leaderboard->ForceFinishAction();

				static_cast<CommonScreen*>(this->_common_screen)->SetHaveLayoutInteractive(false);
				static_cast<CommonScreen*>(this->_common_screen)->OnScreenComeFromEnable();
			}));
		});
	}
}

void LeaderboardLayer::LB_InitValueLB(BaseScreen * common_screen)
{
	_common_screen = common_screen;

	p_lb_current_stage_lb = 0; //0 is richest lb, 1 is strongest lb, 2 is richest ruby lb

	p_lb_current_page_lb = 0;

	p_lb_current_rank_lb_idx = 0;

	p_lb_layout_leaderboard = _common_screen->GetWidgetChildByName("common_screen.leaderboard_layer");
	//
	p_lb_layout_leaderboard_player = p_lb_layout_leaderboard->GetWidgetChildByName(".list_player_iwin_lb");

	p_lb_layout_pull_update_player = p_lb_layout_leaderboard->GetWidgetChildByName(".layout_pull_update");
	p_lb_layout_pull_loadmore_player = p_lb_layout_leaderboard->GetWidgetChildByName(".layout_pull_loadmore");

	auto list_player_lb = static_cast<ListViewTableWidget*>(p_lb_layout_leaderboard_player);
	list_player_lb->addEventListener(
		[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		auto current_drag = list_player_lb->current_stage_drag();

		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = list_player_lb->GetCurrentIdxTo();
			LB_OnParsePlayerToLeaderboard(_widget, current_idx_to);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			size_t current_idx = list_player_lb->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < this->p_lb_current_list_player_.size())
			{
				Player * player = this->p_lb_current_list_player_[current_idx];
				this->p_lb_layout_leaderboard->SetInteractive(false);
				static_cast<CommonScreen*>(_common_screen)->OnShowPlayerInfo(player);

			}
			else
			{
				PASSERT2(false, "have problem here!");
			}

		}

		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_lb_layout_pull_update_player->Visible(true);
				this->p_lb_layout_pull_update_player->SetActionCommandWithName("ORIGIN_POS");
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_UP)
			{
				this->p_lb_layout_pull_loadmore_player->Visible(true);
				this->p_lb_layout_pull_loadmore_player->SetActionCommandWithName("ORIGIN_POS");
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(0)->Visible(true);
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(1)->Visible(false);
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_lb_layout_pull_update_player->GetPosition();
				curpos -= list_player_lb->GetDeltaPos();
				this->p_lb_layout_pull_update_player->SetPosition(curpos);

				float percentmove = list_player_lb->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
			else
			{
				Vec2 curpos = this->p_lb_layout_pull_loadmore_player->GetPosition();

				curpos -= list_player_lb->GetDeltaPos();
				this->p_lb_layout_pull_loadmore_player->SetPosition(curpos);

				float percentmove = list_player_lb->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}

		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_UP)
			{
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(0)->Visible(false);
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(1)->Visible(true);
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(180.f);
			}
		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				if (list_player_lb->HaveEndDrag())
				{
					//reload list here
					this->LB_ResetLeaderboard(true);
					if (p_lb_current_stage_lb == 0)
					{
						GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
					}
					else if (p_lb_current_stage_lb == 1)
					{
						GlobalService::requestStrongest(this->p_lb_current_page_lb++, this->p_current_game_id);
					}
					else if (p_lb_current_stage_lb == 2)
					{
						GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
					}
				}
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				if (list_player_lb->HaveEndDrag())
				{
					if (p_lb_current_stage_lb == 0)
					{
						GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
					}
					else if (p_lb_current_stage_lb == 1)
					{
						GlobalService::requestStrongest(this->p_lb_current_page_lb++, this->p_current_game_id);
					}
					else if (p_lb_current_stage_lb == 2)
					{
						GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
					}
				}
			}
			GetSound->PlaySoundEffect("SFX_UPDATELIST");
		}
		else if (type == EventType_SetCell::ON_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				Vec2 curpos = this->p_lb_layout_pull_update_player->GetPosition();
				curpos -= list_player_lb->GetDeltaPos();
				this->p_lb_layout_pull_update_player->SetPosition(curpos);

				float percentmove = list_player_lb->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_lb_layout_pull_update_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				Vec2 curpos = this->p_lb_layout_pull_loadmore_player->GetPosition();
				curpos -= list_player_lb->GetDeltaPos();
				this->p_lb_layout_pull_loadmore_player->SetPosition(curpos);

				float percentmove = list_player_lb->GetPercentMoveDistance();
				float ro = (100.f - percentmove) * 180.f / 100.f;
				this->p_lb_layout_pull_loadmore_player->GetWidgetChildAtIdx(2)->SetRotate(ro);
			}
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_TOP_DOWN)
			{
				this->p_lb_layout_pull_update_player->Visible(false);
			}
			else if (current_drag == EventType_DragCell::DRAG_BOT_DOWN)
			{
				this->p_lb_layout_pull_loadmore_player->Visible(false);
			}

		}
	});
	list_player_lb->InitTable();
}

void LeaderboardLayer::LB_OnSetPlayerEachLB(ubyte page,
	std::vector<Player * > list_player,
	int & idx_rank,
	WidgetEntity * list_table,
	std::vector<Player*> * list_player_receive,
	cocos2d::Map<int, Sprite*>* list_avatar)
{
	for (size_t i = 0; i < list_player.size(); i++)
	{
		list_player_receive->push_back(list_player[i]);

		size_t current_rank_idx = (size_t)idx_rank;
		idx_rank++;
		//download avatar in save into the list
		auto idx_player = list_player[i];
		{
			int id_avatar = idx_player->userProfile->avatarID;
			GameMgr->HandleAvatarDownload(idx_player->userProfile->avatarID, idx_player->name,
				[&, this, current_rank_idx, id_avatar, list_avatar, list_table, idx_player](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				list_avatar->insert(idx_player->IDDB, sprite_img);


				auto widget_lb = static_cast<ListViewTableWidget*>(list_table);

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
							this->_common_screen->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
							break;
						}
					}
				}
			});
		}
	}
	size_t current_cell_number = static_cast<ListViewTableWidget*>(list_table)->GetNumberSizeOfCell();
	current_cell_number += list_player.size();
	static_cast<ListViewTableWidget*>(list_table)->SetNumberSizeOfCell(current_cell_number);

}

Player* LeaderboardLayer::LB_GetPlayerByIdx(int idx)
{
	std::vector<Player*> list_player = p_lb_current_list_player_;

	if (idx < 0 || (size_t)idx >= list_player.size())
	{
		PASSERT2(idx >= 0 && (size_t)idx < list_player.size(), "idx out of range!");
		return nullptr;
	}

	return list_player[idx];
}

void LeaderboardLayer::LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player)
{
	LB_OnSetPlayerEachLB(page, list_player,
		p_lb_current_rank_lb_idx,
		p_lb_layout_leaderboard_player,
		&p_lb_current_list_player_,
		&p_lb_list_avatar_player_download);
}

void LeaderboardLayer::LB_OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx, bool isPlayer)
{
	Player * player = LB_GetPlayerByIdx(idx);

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

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".stt_id"))->SetText(std::to_string(idx + 1) + ".", true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(player->name, true);
	if (p_lb_current_stage_lb == 0)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(true);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber_win(player->money), true);
	}
	else if (p_lb_current_stage_lb == 1)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(false);
	}
	else if (p_lb_current_stage_lb == 2)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(true);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber_ruby(player->ruby), true);
	}

	auto tx = widget->GetWidgetChildByName(".status_id");
	if (player->userProfile->status != "")
	{
		tx->Visible(true);
		static_cast<TextWidget*>(tx)->SetText(player->userProfile->status, true);
	}
	else
	{
		tx->Visible(false);
	}

	auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");

	if (p_lb_list_avatar_player_download.size() > 0
		&& p_lb_list_avatar_player_download.find(player->IDDB) != p_lb_list_avatar_player_download.end())
	{
		this->_common_screen->ParseSpriteToPanel(panel_avatar, p_lb_list_avatar_player_download.at(player->IDDB), 100);
	}
}

void LeaderboardLayer::LB_ResetLeaderboard(bool cleanup)
{
	p_lb_current_page_lb = 0;

	p_lb_current_rank_lb_idx = 0;

	if (cleanup)
	{
		static_cast<ListViewTableWidget*>(p_lb_layout_leaderboard_player)->ResetListView();
		p_lb_list_avatar_player_download.clear();
		SAFE_DELETE_VECTOR(p_lb_current_list_player_);
	}
}

void LeaderboardLayer::LB_ShowRichestWin()
{
	if (!p_lb_layout_leaderboard->Visible())
	{
		static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
		static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);


		p_lb_current_stage_lb = 0;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_stage_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(2)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(4)->Visible(true);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(3)->Visible(false);

		p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");
	}
}

void LeaderboardLayer::LB_ShowRichestRuby()
{
	if (!p_lb_layout_leaderboard->Visible())
	{
		static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
		static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);

		p_lb_current_stage_lb = 2;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_page_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(2)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(4)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(3)->Visible(true);


		p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");
	}
}

void LeaderboardLayer::LB_ShowStrongest(ubyte current_game_id)
{
	if (!p_lb_layout_leaderboard->Visible())
	{
		p_current_game_id = current_game_id;

		static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
		static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);

		p_lb_current_stage_lb = 1;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_stage_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestStrongest(this->p_lb_current_page_lb++, current_game_id);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(2)->Visible(true);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(4)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(3)->Visible(false);

		p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");
	}
}

int LeaderboardLayer::GetStateLB()
{
	if (p_lb_layout_leaderboard->Visible())
	{
		return p_lb_current_stage_lb;
	}
	return -1;
}

bool LeaderboardLayer::LB_SetInteractive()
{
	if (this->p_lb_layout_leaderboard->Visible())
	{
		this->p_lb_layout_leaderboard->SetInteractive(true);
		return true;
	}
	return false;
}