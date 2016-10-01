#include "TaiXiuLayer.h"
#include "Network/JsonObject/taixiu/TaiXiuGameInfo.h"
#include "Network/JsonObject/taixiu/TaiXiuBetResult.h"
#include "Network/JsonObject/taixiu/TaiXiuUserOrderList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserHistoryList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserOrder.h"
#include "Network/JsonObject/taixiu/TaiXiuUserHistory.h"

#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wTimeDigitalWidget.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wSlider.h"

#include "Network/Global/Globalservice.h"
#include "InputManager.h"
#include "Screens/CommonScreen.h"
#define DEFAULT_TX_BET_TIME 60
#define DEFAULT_TX_RES_TIME 15

TaiXiuLayer::TaiXiuLayer()
{

}

TaiXiuLayer::~TaiXiuLayer()
{

}

int TaiXiuLayer::Update(float dt)
{
	if (p_layout_taixiu->Visible())
	{
		OnDragBtnTaiXiu();
		return 1;
	}
	return 0;
}

void TaiXiuLayer::ParseUserUI()
{
	if (p_layout_tai_xiu_panel->Visible())
	{
		User * user = GetUser;
		if (user == nullptr)
		{
			//PASSERT2(false, "user is null");
			return;
		}

		s64 money = GameController::myInfo->money;
		s64 ruby = GameController::myInfo->ruby;

		static_cast<TextWidget*>(p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".taixiu_mymoney_txt"))->SetText(Utils::formatNUmber_dot(money));
		static_cast<TextWidget*>(p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".taixiu_myruby_txt"))->SetText(Utils::formatNUmber_dot(ruby));
	}
}

bool TaiXiuLayer::ProcessWidgetTaixiu(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "taixiu_btn_leaderboard")
		{
			if (!p_layout_taixiu_leaderboard->Visible())
			{
				GlobalService::getTaiXiuLeaderBoard();
				p_layout_taixiu_leaderboard->SetActionCommandWithName("ON_SHOW");
			}
			return true;
		}
		else if (name == "btn_close_lb_taixiu")
		{
			if (p_layout_taixiu_leaderboard->Visible())
			{
				static_cast<ListViewTableWidget*>(p_taixiu_list_player_leaderboard)->ResetListView();
				p_layout_taixiu_leaderboard->SetActionCommandWithName("ON_HIDE");
			}
			return true;
		}
		else if (name == "taixiu_btn_myhistory")
		{
			if (!p_layout_taixiu_myhistory->Visible())
			{
				GlobalService::getTaiXiuMyHistoryList();
				p_layout_taixiu_myhistory->SetActionCommandWithName("ON_SHOW");
			}
			return true;
		}
		else if (name == "btn_close_history_taixiu")
		{
			if (p_layout_taixiu_myhistory->Visible())
			{
				static_cast<ListViewTableWidget*>(p_taixiu_list_myhistory)->ResetListView();
				p_layout_taixiu_myhistory->SetActionCommandWithName("ON_HIDE");
			}
			return true;
		}
		else if (name == "left_panel_btn")
		{
			if (current_stage_taixiu_panel == 1)
			{
				current_stage_taixiu_panel = 0;
				ResetAndSendSerVer();
			}
			return true;
		}
		else if (name == "right_panel_btn")
		{
			if (current_stage_taixiu_panel == 0)
			{
				current_stage_taixiu_panel = 1;
				ResetAndSendSerVer();
			}
			return true;
		}

		else if (name == "btn_choose_tai")
		{
			if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 0)
			{
				if (!p_layout_taixiu_bet->Visible())
				{
					current_stage_bet_taixiu = 0;
					OnShowPanelBetTaixiu(true);
				}
				else
				{
					OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
						this->p_layout_taixiu_bet->ForceFinishAction();
						this->current_stage_bet_taixiu = -1;
					}));
				}
			}
			return true;
		}
		else if (name == "btn_choose_xiu")
		{
			if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 1)
			{
				if (!p_layout_taixiu_bet->Visible())
				{
					current_stage_bet_taixiu = 1;
					OnShowPanelBetTaixiu(true);
				}
				else
				{
					OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
						this->p_layout_taixiu_bet->ForceFinishAction();
						this->current_stage_bet_taixiu = -1;
					}));
				}
			}
			return true;
		}

		else if (name == "full_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(100);
			SetInputMoneyTextBet(GetCurrentMoney());
			return true;
		}
		else if (name == "3quadter_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(75);
			SetInputMoneyTextBet(GetCurrentMoney() * 3 / 4);
			return true;
		}
		else if (name == "half_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(50);
			SetInputMoneyTextBet(GetCurrentMoney() / 2);
			return true;
		}
		else if (name == "quadter_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(25);
			SetInputMoneyTextBet(GetCurrentMoney() / 4);
			return true;
		}
		else if (name == "plus_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			float value = slider->GetPercent();
			int part = slider->GetPart();
			if (value < 100)
			{
				int current_part = (value / 100.f) * part;
				current_part++;
				if (current_part >= part)
				{
					current_part = part;
				}
				if (part <= 0)
				{
					slider->SetPercent(0);
					current_part = 0;
				}
				else
				{
					slider->SetPercent(current_part * 100.f / part);
				}

				s64 current_money_bet = (s64)(((slider->GetPercent() / 100.f) * (float)part) * 1000.f);
				SetInputMoneyTextBet(GetCurrentMoney() <= 1000 ? GetCurrentMoney() : current_money_bet);
			}
			return true;
		}
		else if (name == "div_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			float value = slider->GetPercent();
			int part = slider->GetPart();
			if (value > 0)
			{
				int current_part = (value / 100.f) * part;
				current_part--;
				if (current_part < 0)
				{
					current_part = 0;
				}
				if (part <= 0)
				{
					slider->SetPercent(0);
					current_part = 0;
				}
				else
				{
					slider->SetPercent(current_part * 100.f / part);
				}
				s64 current_money_bet = (s64)(((slider->GetPercent() / 100.f) * (float)part) * 1000.f);
				SetInputMoneyTextBet(current_money_bet);
			}
			return true;
		}
		else if (name == "taxiu_bet_tai_btn" || name == "taxiu_bet_xiu_btn")
		{
			BetMoney(name == "taxiu_bet_tai_btn" ? 0 : 1);
			OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
				this->p_layout_taixiu_bet->ForceFinishAction();
				this->current_stage_bet_taixiu = -1;
			}));
			return true;
		}
		else if (name == "taixiu_btn_help")
		{
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "input_tai_txt_DETACH_IME")
		{
			current_stage_bet_taixiu = -1;
			OnCheckTextInputMoney(_widget);
			return true;
		}
		else if (name == "input_tai_txt_ATTACH_IME")
		{
			current_stage_bet_taixiu = 0;
			OnShowPanelBetTaixiu(true);
			return true;
		}

		else if (name == "input_xiu_txt_DETACH_IME")
		{
			current_stage_bet_taixiu = -1;
			OnCheckTextInputMoney(_widget);
			return true;
		}
		else if (name == "input_xiu_txt_ATTACH_IME")
		{
			current_stage_bet_taixiu = 1;
			OnShowPanelBetTaixiu(true);
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		if (name == "slider_bet_win_PERCENTAGE_CHANGED" || name == "slider_bet_ruby_PERCENTAGE_CHANGED")
		{
			float value = static_cast<SliderWidget*>(_widget)->GetPercent();
			int part = static_cast<SliderWidget*>(_widget)->GetPart();
			s64 current_money_bet = (s64)(((value / 100.f) * (float)part) * 1000.f);
			SetInputMoneyTextBet(current_money_bet);
			return true;
		}
	}

	return false;
}

void TaiXiuLayer::InitTaiXiu(BaseScreen* screen)
{
	_common_screen = screen;

	p_taixiu_gameinfo = nullptr;
	p_taixiu_bet_result = nullptr;
	p_taixiu_userorder_list = nullptr;
	p_taixiu_history_list = nullptr;

	p_layout_taixiu = _common_screen->GetWidgetChildByName("common_screen.taixiu_layout");
	p_layout_tai_xiu_mini_btn = p_layout_taixiu->GetWidgetChildByName(".mini_button_layout");
	p_layout_tai_xiu_panel = p_layout_taixiu->GetWidgetChildByName(".tai_xiu_layout");
	p_layout_taixiu_leaderboard = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_leaderboard_layout");
	p_taixiu_list_player_leaderboard = p_layout_taixiu_leaderboard->GetWidgetChildByName(".taixiu_list_player_leaderboard");
	auto tx_list_player_lb = static_cast<ListViewTableWidget*>(p_taixiu_list_player_leaderboard);
	tx_list_player_lb->InitTable();
	tx_list_player_lb->addEventListener(
		[this, tx_list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = tx_list_player_lb->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_taixiu_userorder_list->getUserOrderList().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->p_taixiu_userorder_list->getUserOrderList().size() - 1;
			}
			OnParseTaiXiuLeaderboard(_widget, this->p_taixiu_userorder_list->getUserOrderList()[current_idx_to], current_idx_to);
		}
	});


	p_layout_taixiu_myhistory = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_myhistory_layout");

	p_taixiu_list_myhistory = p_layout_taixiu_myhistory->GetWidgetChildByName(".taixiu_list_myhistory");
	auto tx_list_myhistory = static_cast<ListViewTableWidget*>(p_taixiu_list_myhistory);
	tx_list_myhistory->InitTable();
	tx_list_myhistory->addEventListener(
		[this, tx_list_myhistory](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = tx_list_myhistory->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_taixiu_history_list->getUserHistoryList().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->p_taixiu_history_list->getUserHistoryList().size() - 1;
			}
			OnParseTaiXiuHistory(_widget, this->p_taixiu_history_list->getUserHistoryList()[current_idx_to], current_idx_to);
		}
	});

	p_layout_taixiu_bet = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_bet_layout");

	current_stage_taixiu_panel = 0;
	current_stage_bet_taixiu = -1;
	p_result_taixiu = p_previous_result_taixiu = 0;
	p_delta_move_btn_tx = 0;
	p_taixiu_already_have_result = false;

	p_main_taixiu_layout = p_layout_tai_xiu_panel->GetWidgetChildByName(".tai_xiu_panel");
	p_panel_taixiu_bg_history = p_main_taixiu_layout->GetWidgetChildByName(".bg_history");
	p_panel_taixiu_bg_top_layout = p_main_taixiu_layout->GetWidgetChildByName(".tai_xiu_top_layout");

	p_text_effect_bonus_money = p_layout_tai_xiu_panel->GetWidgetChildByName(".layout_bonus");
	//create 20 star history

	auto size_bg_his = p_panel_taixiu_bg_history->GetSize();
	float part_each_slide = size_bg_his.x / 20;
	for (int i = 1; i < 20; i++)
	{
		auto star = p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0);
		auto clone_star = star->Clone();
		clone_star->SetPosition(Vec2((part_each_slide * i) + (part_each_slide / 2), size_bg_his.y / 2));

		p_panel_taixiu_bg_history->AddChildWidget(clone_star);
		p_panel_taixiu_bg_history->ResizeAndPosChildWidget(clone_star);
	}

	//circle

	p_taixiu_circle_layout = p_main_taixiu_layout->GetWidgetChildByName(".tai_xiu_circle");
	p_taixiu_circle_result_layout = p_taixiu_circle_layout->GetWidgetChildByName(".circle_result");
	p_taixiu_circle_dice_layout = p_taixiu_circle_result_layout->GetWidgetChildByName(".result_dice");
	p_taixiu_circle_small_result_layout = p_taixiu_circle_result_layout->GetWidgetChildByName(".result_taixiu");
	p_taixiu_circle_cd_layout = p_taixiu_circle_layout->GetWidgetChildByName(".circle_with_cd");
}

void TaiXiuLayer::OnDragBtnTaiXiu()
{
	if (InputMgr->GetNumberTouch() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();

		if (current_state_touch == Control::STATE_PRESS)
		{
			//click outside so visible this
			cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);// NODE(p_layout_tai_xiu_mini_btn)->convertTouchToNodeSpace(InputMgr->GetTouch());
			auto rect = p_layout_tai_xiu_mini_btn->GetBound();
			if (rect.containsPoint(wPos))
			{
				static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
				NODE(p_layout_tai_xiu_mini_btn)->setTag(1);
				p_layout_tai_xiu_mini_btn->GetWidgetChildAtIdx(0)->SetOpacity(255);
				_previous_pos_touch = wPos;
			}
		}
		else if (current_state_touch == Control::STATE_HOLD)
		{
			if (NODE(p_layout_tai_xiu_mini_btn)->getTag() == 1)
			{
				cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);
				Vec2 cur_pos_t = wPos;
				Vec2 delta_pos = Vec2(cur_pos_t.x - _previous_pos_touch.x, cur_pos_t.y - _previous_pos_touch.y);
				p_delta_move_btn_tx += sqrt(delta_pos.x * delta_pos.x + delta_pos.y*delta_pos.y);
				Vec2 cur_pos_card = p_layout_tai_xiu_mini_btn->GetPosition();
				Vec2 new_pos = Vec2(cur_pos_card.x + delta_pos.x, cur_pos_card.y + delta_pos.y);
				p_layout_tai_xiu_mini_btn->SetPosition(new_pos);
				_previous_pos_touch = cur_pos_t;

			}
		}

	}
	else if (InputMgr->GetNumberTouchRelease() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_RELEASE)
		{
			if (NODE(p_layout_tai_xiu_mini_btn)->getTag() == 1)
			{

				p_layout_tai_xiu_mini_btn->GetWidgetChildAtIdx(0)->SetOpacity(128);
				if (p_delta_move_btn_tx <= 0)
				{
					if (p_layout_tai_xiu_panel->Visible())
					{
						p_layout_tai_xiu_panel->Visible(false);
						static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
						static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();
					}
					else
					{
						p_layout_tai_xiu_panel->Visible(true);
						ScrMgr->SetScreenIDDirty();
						GlobalService::getTaiXiuInfo(GameController::myInfo->name);
						p_taixiu_already_have_result = false;
						_common_screen->onMyInfoChanged();

						static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);
						static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
					}
				}
				else
				{
					static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(true);
				}
				p_delta_move_btn_tx = 0;
			}
			NODE(p_layout_tai_xiu_mini_btn)->setTag(0);
		}
		InputMgr->OnClearTouchReleaseList();
	}

}

void TaiXiuLayer::OnShowBtnTaiXiu(bool show)
{
	p_layout_taixiu->Visible(show);
	if (!show)
	{
		p_layout_tai_xiu_panel->Visible(false);
	}
}

bool TaiXiuLayer::HaveTaixiuLayout()
{
	return p_layout_tai_xiu_panel->Visible();
}

void TaiXiuLayer::OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type)
{
	SAFE_DELETE(p_taixiu_gameinfo);
	p_taixiu_gameinfo = taixiu_info;
	auto p_time_panel = p_layout_tai_xiu_mini_btn->GetWidgetChildByName(".taixiu_timer");
	if (p_layout_tai_xiu_panel && p_layout_tai_xiu_panel->Visible())
	{
		auto history = p_taixiu_gameinfo->getHistoryList();
		for (size_t i = 0; i < history.size(); i++)
		{
			p_panel_taixiu_bg_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(0)->Visible(!history[i]);
			p_panel_taixiu_bg_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(1)->Visible(history[i]);
		}
		static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(p_taixiu_gameinfo->getTotalWinTai()));
		static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(p_taixiu_gameinfo->getTotalWinXiu()));

		static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(p_taixiu_gameinfo->getTotalUserTai()));
		static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(p_taixiu_gameinfo->getTotalUserXiu()));

		if (p_taixiu_gameinfo->getDice1() > 0 &&
			p_taixiu_gameinfo->getDice2() > 0 &&
			p_taixiu_gameinfo->getDice3() > 0)
		{
			if (!p_taixiu_already_have_result)
			{
				p_taixiu_already_have_result = true;
				//
				int total_number_dice = p_taixiu_gameinfo->getDice1() + p_taixiu_gameinfo->getDice2() + p_taixiu_gameinfo->getDice3();
				//show result 
				p_time_panel->GetWidgetChildAtIdx(1)->Visible(true);
				p_time_panel->GetWidgetChildAtIdx(0)->Visible(false);

				float total_time = p_taixiu_gameinfo->getTimeShowResultTotal() > 0 ? p_taixiu_gameinfo->getTimeShowResultTotal() : DEFAULT_TX_RES_TIME;
				float remain_time = p_taixiu_gameinfo->getTimeShowResultRemain() > 0 ? p_taixiu_gameinfo->getTimeShowResultRemain() / 1000.f : DEFAULT_TX_RES_TIME;

				p_result_taixiu = p_taixiu_gameinfo->getResult();
				OnShowResultCirlceWithTimer(true, total_time, remain_time);

				if (p_result_taixiu == 0) // tai
				{
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(0)->Visible(true);
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(1)->Visible(false);
					static_cast<TextWidget*>(p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(2))->SetText(std::to_string(total_number_dice), true);
				}
				else //xiu
				{
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(0)->Visible(false);
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(1)->Visible(true);
					static_cast<TextWidget*>(p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(2))->SetText(std::to_string(total_number_dice), true);
				}
				if (remain_time < total_time * 2 / 3)
				{
					OnShowResultCircle();
					auto dice1 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(0));
					dice1->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice1, this]() {
						dice1->ForceFinishAction();
						dice1->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice1() + 5);
					}));
					auto dice2 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(1));
					dice2->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice2, this]() {
						dice2->ForceFinishAction();
						dice2->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice2() + 5);
					}));
					auto dice3 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(2));
					dice3->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice3, this]() {
						dice3->ForceFinishAction();
						dice3->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice3() + 5);
					}));
				}
				else
				{
					auto dice1 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(0));
					dice1->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice1, this]() {
						dice1->ForceFinishAction();
						dice1->RunAnim(CallFunc::create([dice1, this]() {
							dice1->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice1() + 5);
						}));
					}));
					auto dice2 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(1));
					dice2->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice2, this]() {
						dice2->ForceFinishAction();
						dice2->RunAnim(CallFunc::create([dice2, this]() {
							dice2->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice2() + 5);
						}));
					}));
					auto dice3 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(2));
					dice3->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice3, this]() {
						dice3->ForceFinishAction();
						dice3->RunAnim(CallFunc::create([dice3, this]() {
							this->OnShowResultCircle();
							dice3->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice3() + 5);
						}));
					}));
				}

				p_previous_result_taixiu = p_result_taixiu;
			}
		}
		else
		{
			OnShowResultCirlceWithTimer(false,
				p_taixiu_gameinfo->getTimeBetTotal() > 0 ? p_taixiu_gameinfo->getTimeBetTotal() : DEFAULT_TX_BET_TIME,
				p_taixiu_gameinfo->getTimeBetRemain() > 0 ? p_taixiu_gameinfo->getTimeBetRemain() / 1000.f : DEFAULT_TX_BET_TIME);

			p_time_panel->GetWidgetChildAtIdx(1)->Visible(false);
			p_time_panel->GetWidgetChildAtIdx(0)->Visible(true);

			static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->ResetTimer(
				p_taixiu_gameinfo->getTimeBetTotal() > 0 ? p_taixiu_gameinfo->getTimeBetTotal() : DEFAULT_TX_BET_TIME,
				p_taixiu_gameinfo->getTimeBetRemain() > 0 ? p_taixiu_gameinfo->getTimeBetRemain() / 1000.f : DEFAULT_TX_BET_TIME);
		}

		if (p_taixiu_gameinfo->getTotalWinTai() == 0 &&
			p_taixiu_gameinfo->getTotalWinXiu() == 0 &&
			p_taixiu_gameinfo->getTotalUserTai() == 0 &&
			p_taixiu_gameinfo->getTotalUserXiu() == 0)
		{
			//reset timmer ;
			p_time_panel->GetWidgetChildAtIdx(1)->Visible(false);
			p_time_panel->GetWidgetChildAtIdx(0)->Visible(true);
			static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->ResetTimer(DEFAULT_TX_BET_TIME, DEFAULT_TX_BET_TIME);
			//
			OnShowResultCirlceWithTimer(false, DEFAULT_TX_BET_TIME, DEFAULT_TX_BET_TIME);
			//
			p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(false);
			p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(false);
			p_taixiu_circle_layout->GetWidgetChildByName(".circle_time_process")->Visible(false);
			p_taixiu_circle_small_result_layout->Visible(false);
			OnShowResultTaiXiu(0, false);
			p_taixiu_already_have_result = false;

			auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(1);
			static_cast<TextWidget*>(wi)->SetText("0", false);
			wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(1);
			static_cast<TextWidget*>(wi)->SetText("0", false);

		}
	}
	else
	{
		s64 timebet_remain = p_taixiu_gameinfo->getTimeBetRemain();
		s64 timeresult_remain = p_taixiu_gameinfo->getTimeShowResultRemain();
		s64 timebet_total = p_taixiu_gameinfo->getTimeBetTotal();
		s64 timeresult_total = p_taixiu_gameinfo->getTimeShowResultTotal();

		s64 time_remain = 0;
		s64 time_total = 0;
		if (timebet_remain < 0)
		{
			p_time_panel->GetWidgetChildAtIdx(1)->Visible(true);
			p_time_panel->GetWidgetChildAtIdx(0)->Visible(false);
			time_remain = timeresult_remain;
			time_total = timeresult_total > 0 ? timeresult_total : DEFAULT_TX_RES_TIME;
		}
		else
		{
			p_time_panel->GetWidgetChildAtIdx(1)->Visible(false);
			p_time_panel->GetWidgetChildAtIdx(0)->Visible(true);
			time_remain = timebet_remain;
			time_total = timebet_total > 0 ? timebet_total : DEFAULT_TX_BET_TIME;
		}

		static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->SetTimer(time_total, (double)time_remain / 1000,
			[p_time_panel, this](cocos2d::Ref * ref, UI_Widget::EventType_Digital event_type)
		{
			if (event_type == UI_Widget::EventType_Digital::ON_TIMER_ZERO)
			{
				p_time_panel->GetWidgetChildAtIdx(1)->Visible(!p_time_panel->GetWidgetChildAtIdx(1)->Visible());
				p_time_panel->GetWidgetChildAtIdx(0)->Visible(!p_time_panel->GetWidgetChildAtIdx(0)->Visible());

				s64 time_remain = 0;
				s64 time_total = 0;
				if (p_time_panel->GetWidgetChildAtIdx(1)->Visible())
				{
					time_remain = DEFAULT_TX_RES_TIME;//timeresult_remain / 1000;
					time_total = DEFAULT_TX_RES_TIME;//timeresult_total > 0 ? timeresult_total : time_remain;
					GlobalService::getTaiXiuInfo(GameController::myInfo->name);
				}
				else
				{
					time_remain = DEFAULT_TX_BET_TIME;//timebet_remain / 1000;
					time_total = DEFAULT_TX_BET_TIME; //timebet_total > 0 ? timebet_total : time_remain;
					GlobalService::getTaiXiuInfo(GameController::myInfo->name);
				}

				static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->ResetTimer(time_total, (double)(time_remain));
			}

		});
	}
}

void TaiXiuLayer::OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult)
{
	SAFE_DELETE(p_taixiu_bet_result);
	p_taixiu_bet_result = taiXiuBetResult;

	if (p_taixiu_bet_result->getBetWin() == 0)
	{
		ScrMgr->OnShowDialog("notice", p_taixiu_bet_result->getMessage(), DIALOG_ONE_BUTTON);
		return;
	}

	SetNumberBetResult(p_taixiu_bet_result->getBetWin(), p_taixiu_bet_result->getBetChoice());
}

void TaiXiuLayer::OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList)
{
	SAFE_DELETE(p_taixiu_userorder_list);
	p_taixiu_userorder_list = taixiuOderList;
	static_cast<ListViewTableWidget*>(p_taixiu_list_player_leaderboard)->SetNumberSizeOfCell(p_taixiu_userorder_list->getUserOrderList().size());
}

void TaiXiuLayer::OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList)
{
	SAFE_DELETE(p_taixiu_history_list);
	p_taixiu_history_list = taixiuHistoryList;
	static_cast<ListViewTableWidget*>(p_taixiu_list_myhistory)->SetNumberSizeOfCell(p_taixiu_history_list->getUserHistoryList().size());
}


void TaiXiuLayer::ResetAndSendSerVer()
{
	ChangeButtonWinRubyTaiXiu();
	ScrMgr->SetScreenIDDirty();
	// GlobalService.sendScreenIdToServer(ScreenID.SCREEN_TAIXIU_RUBY);
	GlobalService::getTaiXiuInfo(GameController::myInfo->name);
}

void TaiXiuLayer::ChangeButtonWinRubyTaiXiu()
{
	if (current_stage_taixiu_panel == 0)
	{
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".left_panel_act")->Visible(true);
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".right_panel_act")->Visible(false);

		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout")->Visible(true);
		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout")->Visible(false);
	}
	else
	{
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".left_panel_act")->Visible(false);
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".right_panel_act")->Visible(true);

		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout")->Visible(false);
		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout")->Visible(true);
	}
}

void TaiXiuLayer::OnCheckTextInputMoney(WidgetEntity * _widget)
{
	s64 current_input_money = atoll(static_cast<TextFieldWidget*>(_widget)->GetText().GetString());
	s64 current_money = current_stage_taixiu_panel == 0 ? GameController::myInfo->money : GameController::myInfo->ruby;
	if (current_money >= 1000)
	{
		if (current_input_money > current_money)
		{
			int mul_money = current_money % 1000;
			current_money = current_money - mul_money;
			static_cast<TextFieldWidget*>(_widget)->ClearText();
			static_cast<TextFieldWidget*>(_widget)->SetText(Utils::formatNUmber_dot(current_money));
		}
		else
		{
			int mul_money = current_input_money % 1000;
			current_input_money = current_input_money - mul_money;
			static_cast<TextFieldWidget*>(_widget)->ClearText();
			static_cast<TextFieldWidget*>(_widget)->SetText(Utils::formatNUmber_dot(current_input_money));
		}
	}
	else
	{
		static_cast<TextFieldWidget*>(_widget)->ClearText();
		static_cast<TextFieldWidget*>(_widget)->SetText(Utils::formatNUmber_dot(current_money));
	}
}

void TaiXiuLayer::OnShowPanelBetTaixiu(bool show, CallFunc * call_back)
{
	if (current_stage_bet_taixiu == 0 || current_stage_bet_taixiu == -1)
	{
		if (show)
		{
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_tai_btn")->Visible(true);
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_xiu_btn")->Visible(false);
			p_layout_taixiu_bet->SetActionCommandWithName("ON_SHOW_TAI", call_back);
		}
		else
		{
			p_layout_taixiu_bet->SetActionCommandWithName("ON_HIDE_TAI", call_back);
		}
	}
	else if (current_stage_bet_taixiu == 1 || current_stage_bet_taixiu == -1)
	{
		if (show)
		{
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_tai_btn")->Visible(false);
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_xiu_btn")->Visible(true);
			p_layout_taixiu_bet->SetActionCommandWithName("ON_SHOW_XIU", call_back);
		}
		else
		{
			p_layout_taixiu_bet->SetActionCommandWithName("ON_HIDE_XIU", call_back);
		}
	}

	if (show)
	{
		SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
		s64 current_money = GetCurrentMoney();
		if (current_money <= 1000)
		{
			slider->SetPart(0);
		}
		else
		{
			int number_part = current_money / 1000;
			slider->SetPart(number_part);
		}
	}
}

void TaiXiuLayer::SetInputMoneyTextBet(s64 money)
{
	if (money > 1000)
	{
		int mul = money % 1000;
		money = money - mul;
	}

	if (current_stage_bet_taixiu == 0)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(Utils::formatNUmber_dot(money));
	}
	else if (current_stage_bet_taixiu == 1)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(Utils::formatNUmber_dot(money));
	}
}

WidgetEntity * TaiXiuLayer::GetSliderBet()
{
	WidgetEntity * wi = nullptr;
	if (current_stage_taixiu_panel == 0)
	{
		wi = (p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout.slider_bet_win"));
	}
	else
	{
		wi = (p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout.slider_bet_ruby"));
	}
	return wi;
}

s64 TaiXiuLayer::GetCurrentMoney()
{
	s64 current_money = current_stage_taixiu_panel == 0 ? GameController::myInfo->money : GameController::myInfo->ruby;
	return current_money;
}

void TaiXiuLayer::BetMoney(int stage)
{
	s64 current_money = 0;
	if (stage == 0)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(3);
		auto str = static_cast<TextFieldWidget*>(wi)->GetText();
		current_money = Utils::GetNumberFromFormat(str.GetString());
	}
	else if (stage == 1)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(3);
		auto str = static_cast<TextFieldWidget*>(wi)->GetText();
		current_money = Utils::GetNumberFromFormat(str.GetString());
	}
	else
	{
		PASSERT2(false, "wrong here");
	}

	GlobalService::taiXiuBet(GameController::myInfo->name, current_money, current_stage_bet_taixiu);
}

void TaiXiuLayer::SetNumberBetResult(s64 money_total, int state)
{
	WidgetEntity * wi = nullptr;
	if (state == 0)
	{
		wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(1);
	}
	else if (state == 1)
	{
		wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(1);
	}
	else
	{
		PASSERT2(false, "wrong here");
	}
	s64 current_number = Utils::GetNumberFromFormat(static_cast<TextWidget*>(wi)->GetText().GetString());
	static_cast<TextWidget*>(wi)->TextRunEffect(
		money_total, current_number, 1.f,
		[](s64 money)->std::string
	{
		return Utils::formatNUmber_dot(abs(money));
	},
		CallFunc::create([wi, money_total]()
	{
		static_cast<TextWidget*>(wi)->SetText(Utils::formatNUmber_dot(money_total), true);
	}));

}

void TaiXiuLayer::OnReceiveTransferMoney(s64 bonus_money, RKString info, int type)
{
	WidgetEntity * w_text = nullptr;
	if (bonus_money >= 0)
	{
		w_text = p_text_effect_bonus_money->GetWidgetChildByName(".bonus_iwin_add");
		p_text_effect_bonus_money->GetWidgetChildByName(".bonus_iwin_div")->Visible(false);
	}
	else
	{
		w_text = p_text_effect_bonus_money->GetWidgetChildByName(".bonus_iwin_div");
		p_text_effect_bonus_money->GetWidgetChildByName(".bonus_iwin_add")->Visible(false);
	}
	w_text->Visible(true);
	static_cast<TextWidget*>(w_text)->TextRunEffect(
		bonus_money, 0, 1.f, [bonus_money](s64 money)->std::string {
		auto text = Utils::formatNUmber_dot(abs(money));
		if (bonus_money < 0)
		{
			text = "-" + text;
		}
		else
		{
			text = "+" + text;
		}
		return text;
	});
	p_text_effect_bonus_money->SetActionCommandWithName("ACTION");
}

void TaiXiuLayer::OnShowResultTaiXiu(int result, bool show)
{
	auto tai_effect = p_main_taixiu_layout->GetWidgetChildByName(".tai_panel_bg")->GetWidgetChildAtIdx(1);
	auto xiu_effect = p_main_taixiu_layout->GetWidgetChildByName(".xiu_panel_bg")->GetWidgetChildAtIdx(1);
	if (show)
	{
		if (result == 0) //tai
		{
			tai_effect->Visible(true);
			tai_effect->SetActionCommandWithName("ACTION");
			xiu_effect->Visible(false);
		}
		else
		{
			xiu_effect->Visible(true);
			xiu_effect->SetActionCommandWithName("ACTION");
			tai_effect->Visible(false);
		}
	}
	else
	{
		xiu_effect->ForceFinishAction();
		tai_effect->ForceFinishAction();

		xiu_effect->Visible(false);
		tai_effect->Visible(false);
	}
}

void TaiXiuLayer::OnParseTaiXiuLeaderboard(WidgetEntity* _widget, iwinmesage::TaiXiuUserOrder * user, int idx)
{
	if (!user || !_widget)
		return;

	if (idx % 2 == 0)
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(true);
		_widget->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
		_widget->GetWidgetChildAtIdx(1)->Visible(true);
	}

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".stt_id"))->SetText(std::to_string(user->getOrder()) + ".", true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(user->getUsername(), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNUmber_dot(user->getWin()), true);
}

void TaiXiuLayer::OnParseTaiXiuHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserHistory * his, int idx)
{
	if (!his || !_widget)
		return;

	if (idx % 2 == 0)
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(true);
		_widget->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
		_widget->GetWidgetChildAtIdx(1)->Visible(true);
	}

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".phien_id"))->SetText(std::to_string(his->getMatchId()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".time_id"))->SetText(std::to_string(his->getTime()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".bet_way_id"))->SetText(std::to_string(his->getBetChoice()), true);

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".result_id"))->SetText(his->getResult(), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".bet_id"))->SetText(std::to_string(his->getBetWin()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".return_id"))->SetText(std::to_string(his->getReturnWin()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".receive_id"))->SetText(std::to_string(his->getReceivedWin()), true);

}

void TaiXiuLayer::OnShowResultCirlceWithTimer(bool show, double long total_time, double long current_time)
{
	if (show)
	{
		p_taixiu_circle_cd_layout->Visible(false);
		p_taixiu_circle_result_layout->Visible(true);
		static_cast<TimeProcessWidget*>(p_taixiu_circle_layout->GetWidgetChildByName(".circle_time_process"))->SetTimer(total_time, current_time);
	}
	else
	{
		p_taixiu_circle_cd_layout->Visible(true);
		p_taixiu_circle_result_layout->Visible(false);
		static_cast<TimeDigitalWidget*>(p_taixiu_circle_cd_layout->GetWidgetChildByName(".circle_time"))->SetTimer(total_time, current_time);
	}
}

void TaiXiuLayer::OnShowResultCircle()
{
	if (!p_taixiu_circle_small_result_layout->Visible())
	{
		p_taixiu_circle_small_result_layout->SetActionCommandWithName("ON_SHOW");
	}
	if (p_result_taixiu == 0) // tai
	{
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(true);
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(false);
	}
	else
	{
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(false);
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(true);
	}
	this->OnShowResultTaiXiu(p_result_taixiu, true);
}

int TaiXiuLayer::GetStateTaiXiu()
{
	if (p_layout_tai_xiu_panel->Visible())
	{
		if (current_stage_taixiu_panel == 0)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}
	return -1;
}