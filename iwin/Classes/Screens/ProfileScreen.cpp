#include "ProfileScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wPageView.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListViewTable.h"

#include "MainScreen.h"

#include "Social/SocialManager.h"
#include "Network/Global/Globalservice.h"
#include "Models/Achievement.h"
#include "Models/Item.h"

#include "Network/JsonObject/Avatar/AvatarCat.h"
#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"
#include "RKThread_Code/RKThreadWrapper.h"
#include "Screens/GamePlayMgr.h"
#include "Screens/GameScreens/GameScreens.h"
using namespace iwinmesage;
char * list_sx[3] = { "male" , "female" ,"unknown" };

ProfileScreen::ProfileScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_page_view_profile = nullptr;
	p_page_profile_left = nullptr;
	p_page_profile_name = nullptr;	
	p_page_trans_history = nullptr;
	p_page_view_achieve = nullptr;
	p_previous_page_idx = 0;

	p_list_choose_sx = nullptr;
	p_list_choose_day = nullptr;
	p_list_choose_month = nullptr;
	p_list_choose_year = nullptr;

	p_thread_check_time = nullptr;

	p_panel_avatar = nullptr;


	p_user_dirty = false;
	p_is_create_list_choose = false;
	p_list_achieve.clear();

	p_cur_day = 1;
	p_cur_month = 1;
	p_cur_year = 1917;

	p_layout_edit_profile = nullptr;

	p_shop_avatar_layout = nullptr;
	p_shop_avatar_list_cat = nullptr;
	p_shop_avatar_list = nullptr;
	p_confirm_buy_avatar_layout = nullptr;

	p_layout_send_gift = nullptr;
	p_textfield_gift_account = nullptr;
	p_textfield_gift_number = nullptr;
	p_avatar_list = nullptr;
	p_current_avatar_idx_buy = 0;
	p_stage_avatar_cat_list = 0;
	p_list_avatar_download.clear();

	p_top_panel = nullptr;
}

ProfileScreen::~ProfileScreen()
{
	p_top_panel = nullptr;
	p_page_view_profile = nullptr;
	p_page_profile_left = nullptr;
	p_page_profile_name = nullptr;
	p_page_trans_history = nullptr;
	p_page_view_achieve = nullptr;

	p_previous_page_idx = -1;

	p_list_choose_sx = nullptr;
	p_list_choose_day = nullptr;
	p_list_choose_month = nullptr;
	p_list_choose_year = nullptr;

	if (p_thread_check_time)
	{
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}

	p_panel_avatar = nullptr;


	p_user_dirty = false;
	p_status_dirty = false;
	p_is_create_list_choose = false;
	for (size_t i = 0; i < p_list_achieve.size(); i++)
	{
		delete p_list_achieve.at(i);
	}
	p_list_achieve.clear();

	p_layout_send_gift = nullptr;
	p_textfield_gift_account = nullptr;
	p_textfield_gift_number = nullptr;
}

int ProfileScreen::Init()
{
	RKString _menu = "profile_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	
	CallInitComponent(true);

	return 1;
}

int	ProfileScreen::InitComponent()
{
	p_top_panel = GetWidgetChildByName("profile_screen.top_panel");
	p_page_view_profile = GetWidgetChildByName("profile_screen.main_profile_layout");

	p_page_profile_left = p_page_view_profile->GetWidgetChildByName(".left_layout");
	p_page_profile_name = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_profile_name");
	p_page_view_achieve = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_profile_achieve.list_detail_achieve");
	p_page_trans_history = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_trans_history");

	p_list_choose_sx = p_page_profile_name->GetWidgetChildByName(".title_input_sx.list_choose_sx");
	p_list_choose_day = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_day");
	p_list_choose_month = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_month");
	p_list_choose_year = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_year");

	p_panel_avatar = p_page_profile_left->GetWidgetChildByName(".panel_avatar");
	p_layout_edit_profile = GetWidgetChildByName("profile_screen.layout_edit_profile");


	p_shop_avatar_layout = GetWidgetChildByName("profile_screen.shop_avatar_layout.shop_layout");
	p_shop_avatar_list_cat = p_shop_avatar_layout->GetWidgetChildByName(".list_shop_view");
	p_shop_avatar_list = p_shop_avatar_layout->GetWidgetChildByName(".list_avatar_view");
	p_confirm_buy_avatar_layout = p_shop_avatar_layout->GetParentWidget()->GetWidgetChildByName(".confirm_buy_avatar_layout._layout");
	//init table_view
	static_cast<ListViewTableWidget*>(p_shop_avatar_list)->InitTable();
	static_cast<ListViewTableWidget*>(p_page_view_achieve)->InitTable();

	p_layout_send_gift = GetWidgetChildByName("profile_screen.gift_win_layout.gift_win");
	p_textfield_gift_account = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_account.panel_input.send_account_input");
	p_textfield_gift_number = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_number.panel_input.number_of_win_input");

	return 1;
}

int ProfileScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_thread_check_time && !p_thread_check_time->IsThreadRunning())
	{
		auto day = p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_day.text_input");
		auto mon = p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
		auto year = p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
		//parse data after recorrect time
		UpdateBirthDay_ToServer(p_cur_day, p_cur_month, p_cur_year);
		//
		static_cast<TextWidget*>(day)->SetText(std::to_string(p_cur_day), true);
		static_cast<TextWidget*>(mon)->SetText(std::to_string(p_cur_month), true);
		static_cast<TextWidget*>(year)->SetText(std::to_string(p_cur_year), true);

		/*auto number_wi = p_list_choose_day->GetNumberChildWidget();
		for (int i = 0; i < number_wi; i++)
		{
			p_list_choose_day->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(2)->Visible(false);
		}
		p_list_choose_day->GetWidgetChildAtIdx(p_cur_day - 1)->GetWidgetChildAtIdx(2)->Visible(true);
		static_cast<ListViewWidget*>(p_list_choose_day)->JumpToItemIdx(p_cur_day - 1);*/

		/*number_wi = p_list_choose_month->GetNumberChildWidget();
		for (int i = 0; i < number_wi; i++)
		{
			p_list_choose_month->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(2)->Visible(false);
		}
		p_list_choose_month->GetWidgetChildAtIdx(p_cur_month - 1)->GetWidgetChildAtIdx(2)->Visible(true);
		static_cast<ListViewWidget*>(p_list_choose_month)->JumpToItemIdx(p_cur_month - 1);*/

		//after thread check finish delete it away;
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}


	if (p_receive_data_achievement && static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetGameList())
	{
		p_receive_data_achievement = false;
		RunThreadParseLB();
	}

	return 1;
}

void ProfileScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);

}
void ProfileScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void ProfileScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	if (p_list_choose_sx->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_sx,
			[&](void) {
			OnShowOrHideListChoose(p_list_choose_sx, false);
		});
	}
	else if (p_list_choose_day->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_day,
			[&](void) {
			OnShowOrHideListChoose(p_list_choose_day, false);
		});
	}
	else if (p_list_choose_month->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_month,
			[&](void) {
			OnShowOrHideListChoose(p_list_choose_month, false);
		});
	}
	else if (p_list_choose_year->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_year,
			[&](void) {
			OnShowOrHideListChoose(p_list_choose_year, false);
		});
	}
	else if (p_layout_edit_profile->Visible())
	{
		CheckTouchOutSideWidget(p, p_layout_edit_profile,
			[this](void)
		{
			this->p_layout_edit_profile->SetActionCommandWithName("ON_HIDE");
			this->SetInteractiveInsteadOf(true, this->p_layout_edit_profile);
		});
	}
	else if (p_shop_avatar_layout->GetParentWidget()->Visible())
	{
		CheckTouchOutSideWidget(p, p_shop_avatar_layout->GetParentWidget(),
			[this](void)
		{
			if (p_shop_avatar_list_cat->Visible())
			{
				this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
				SetInteractiveInsteadOf(true, this->p_shop_avatar_layout->GetParentWidget());
			}
			else
			{
				p_shop_avatar_list_cat->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() 
				{
					this->p_shop_avatar_list_cat->ForceFinishAction();
					this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
					SetInteractiveInsteadOf(true, this->p_shop_avatar_layout->GetParentWidget());
				}));
				p_shop_avatar_list->SetActionCommandWithName("ON_HIDE");
			}
		});
	}
}

void ProfileScreen::OnShowIdxBtnTopPanel(int idx)
{
	p_top_panel->GetWidgetChildByName(".bg_info_on")->Visible(false);
	p_top_panel->GetWidgetChildByName(".bg_achieve_on")->Visible(false);
	p_top_panel->GetWidgetChildByName(".bg_history_on")->Visible(false);

	if (idx == 0)
	{
		p_top_panel->GetWidgetChildByName(".bg_info_on")->Visible(true);
	}
	else if (idx == 1)
	{
		p_top_panel->GetWidgetChildByName(".bg_achieve_on")->Visible(true);
	}
	else if (idx == 2)
	{
		p_top_panel->GetWidgetChildByName(".bg_history_on")->Visible(true);
	}
}

void ProfileScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_info" && p_previous_page_idx != 0)
		{

			p_page_profile_name->SetActionCommandWithName("MOVE_MIDDLE", CallFunc::create(
			[this]() {
				this->p_page_profile_name->ForceFinishAction();
				this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(true);
			}));
			if (p_previous_page_idx == 1)
			{
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_RIGHT");
			}
			else
			{
				p_page_trans_history->SetActionCommandWithName("MOVE_RIGHT");
			}
			p_previous_page_idx = 0;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}
		else if (name == "btn_achieve" && p_previous_page_idx != 1)
		{
			this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(false);		
			if (p_previous_page_idx == 0)
			{
				p_page_profile_name->SetActionCommandWithName("MOVE_LEFT");
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_MIDDLE_FROM_RIGHT");
			}
			else
			{
				p_page_trans_history->SetActionCommandWithName("MOVE_RIGHT");
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_MIDDLE_FROM_LEFT");
			}

			p_previous_page_idx = 1;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}
		else if (name == "btn_history" && p_previous_page_idx != 2)
		{
			this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(false);
			p_page_trans_history->SetActionCommandWithName("MOVE_MIDDLE");
			if (p_previous_page_idx == 0)
			{
				p_page_profile_name->SetActionCommandWithName("MOVE_LEFT");
			}
			else
			{
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_LEFT");
			}
			p_previous_page_idx = 2;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}

		else if (name == "btn_back")
		{
			ScrMgr->SwitchToMenu(p_menu_come_from, MENU_LAYER::PROFILE_SCREEN);
		}
		else if (name == "btn_input_sx")
		{
			OnShowOrHideListChoose(p_list_choose_sx, true);
		}
		else if (name == "btn_input_day")
		{
			OnShowOrHideListChoose(p_list_choose_day, true);
		}
		else if (name == "btn_input_month")
		{
			OnShowOrHideListChoose(p_list_choose_month, true);
		}
		else if (name == "btn_input_year")
		{
			OnShowOrHideListChoose(p_list_choose_year, true);
		}
		else if (name.Contains("sx"))
		{
			RKString idx_sx = name.Split("_").GetAt(1);
			int idx = atoi(idx_sx.GetString());
			WidgetEntity * p_list = nullptr;
			p_list = p_list_choose_sx;
			idx_sx = list_sx[idx];

			UpdateSx_ToServer(idx);

			auto wi = p_list->GetParentWidget()->GetWidgetChildByName(".btn_input_sx.text_input");
			static_cast<TextWidget*>(wi)->SetText(idx_sx);

			auto number_wi = p_list->GetNumberChildWidget();
			for (size_t i = 0; i < number_wi; i++)
			{
				p_list->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(2)->Visible(false);
			}
			p_list->GetWidgetChildAtIdx(idx)->GetWidgetChildAtIdx(2)->Visible(true);
			static_cast<ListViewWidget*>(p_list)->JumpToItemIdx(idx);
			OnShowOrHideListChoose(p_list, false);
		}
		else if (name == "clear_text_input_1")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_1");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "clear_text_input_phone")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_phone");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "clear_text_input_address")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_address");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "clear_text_input_cmnd")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_cmnd");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "clear_text_input_status")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_status");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "clear_text_input_email")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_email");
			static_cast<TextFieldWidget*>(wi)->ClearText();
		}
		else if (name == "confirm_email")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input.text_input_email");
			RKString current_email = static_cast<TextFieldWidget*>(wi)->GetText();
			if (current_email != "")
			{
				GlobalService::requestVerifyEmail(current_email.GetString());
			}
			else
			{
				ScrMgr->OnShowDialog("error", L"email không được rỗng", DIALOG_ONE_BUTTON);
			}
		}
		else if (name == "confirm_phone")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input.text_input_phone");
			RKString current_phone = static_cast<TextFieldWidget*>(wi)->GetText();
			if (current_phone != "")
			{
				GlobalService::requestVerifyPhone(current_phone.GetString());
			}	
			else
			{
				ScrMgr->OnShowDialog("error", L"số phone không được rỗng", DIALOG_ONE_BUTTON);
			}
		}
		else if (name == "send_iwin_other")
		{
			p_layout_send_gift->GetParentWidget()->Visible(true);
			p_layout_send_gift->SetActionCommandWithName("ON_SHOW", CallFunc::create(
				[this]()
			{
				this->p_layout_send_gift->ForceFinishAction();
				this->SetInteractiveInsteadOf(false, this->p_layout_send_gift);
			}));
		}

		else if (name == "btn_profile")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_SHOW");
			this->SetInteractiveInsteadOf(false, p_layout_edit_profile);
		}
		else if (name == "change_pass_btn")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_edit_profile->ForceFinishAction();

				auto scr_common = GetCommonScr;
				scr_common->SetScreenComeFrom(this);
				//scr_common->SetUser(this->p_user);
				scr_common->OnShowChangePassLayout();
			}));
		}
		else if (name == "take_avatar_shop_btn")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_edit_profile->ForceFinishAction();
				this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_SHOW");
				SetInteractiveInsteadOf(false, this->p_shop_avatar_layout->GetParentWidget());
				//request avatar cate from server
				GlobalService::getAvatarCategory();
			}));
		}
		else if (name == "btn_back_shop_avatar")
		{
			if (p_shop_avatar_list_cat->Visible())
			{
				this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
				SetInteractiveInsteadOf(true, this->p_shop_avatar_layout->GetParentWidget());
			}
			else
			{
				p_shop_avatar_list_cat->SetActionCommandWithName("ON_SHOW");
				p_shop_avatar_list->SetActionCommandWithName("ON_HIDE");
			}
		}
		else if (name == "btn_buy_avatar" || name == "btn_cancel_buy_avatar")
		{
			p_confirm_buy_avatar_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() 
			{
				this->p_confirm_buy_avatar_layout->ForceFinishAction();
				this->p_confirm_buy_avatar_layout->GetParentWidget()->Visible(false);
				//this->SetInteractiveInsteadOf(true, this->p_confirm_buy_avatar_layout);
				this->SetInteractiveInsteadOf(false, this->p_shop_avatar_layout->GetParentWidget());
			}));

			if (name == "btn_buy_avatar" && p_avatar_list)
			{
				if (p_current_avatar_idx_buy >= 0 && p_current_avatar_idx_buy < p_avatar_list->getList().size())
				{
					auto current_avatar_id = p_avatar_list->getList()[p_current_avatar_idx_buy];
					GlobalService::buyAvatar(current_avatar_id->getId(), current_avatar_id->getMoney());
				}
				else
				{
					PASSERT2(false, "problem here!");
				}
			}
		}

		else if (name == "btn_clear_number" || name == "btn_clear_account")
		{
			_widget->Visible(false);
			static_cast<TextFieldWidget*>(_widget->GetParentWidget()->GetWidgetChildAtIdx(0))->ClearText();
		}
		else if (name == "btn_close_panel_gift")
		{
			p_layout_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_layout_send_gift->ForceFinishAction();
				this->p_layout_send_gift->GetParentWidget()->Visible(false);
				this->SetInteractiveInsteadOf(true, this->p_layout_send_gift);
			}));
		}
		else if (name == "btn_gift_win")
		{

			RKString text_name = static_cast<TextFieldWidget*>(p_textfield_gift_account)->GetText();
			RKString text_number = static_cast<TextFieldWidget*>(p_textfield_gift_number)->GetText();
			u64 number_money = atol(text_number.GetString());
			if (text_name == "" || text_number == "" || number_money <= 0)
				return;

			RKString text_arg = LangMgr->GetString("do_you_agree_transfe");

			text_arg.ReplaceFirst("%s" , std::to_string(number_money));
			text_arg.ReplaceFirst("%s" , text_name);
			//int n = sprintf_s(buffer, text_arg.GetString(), str_money, str_name);
			//buffer[n] = '/0';
			ScrMgr->OnShowDialog("send_win", text_arg, DIALOG_TWO_BUTTON, "",
				[this , number_money](const char * str, const char * name_btn)
			{
				if (number_money < 10000 || number_money > 100000000)
				{
					ScrMgr->OnShowDialog("notice", L"số Win phải từ 10.000 đến 100.000.000", DIALOG_ONE_BUTTON);
				}
				else
				{

					this->p_layout_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create(
						[this, number_money]()
					{
						this->p_layout_send_gift->ForceFinishAction();
						this->p_layout_send_gift->GetParentWidget()->Visible(false);
						this->SetInteractiveInsteadOf(true, this->p_layout_send_gift);

						RKString text_name = static_cast<TextWidget*>(p_textfield_gift_account)->GetText();
						GlobalService::transferMoney(text_name.GetString(), number_money);
					}));
				}

			});
		}

	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "text_input_1_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_1")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_1")->Visible(false);
			}
		}
		else if (name == "text_input_1_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_1")->Visible(false);
			UpdatePlayerName_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
		}
		else if (name == "text_input_1_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_1")->Visible(true);
		}
		else if (name == "text_input_phone_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_phone")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_phone")->Visible(false);
			}
		}
		else if (name == "text_input_phone_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_phone")->Visible(false);
			UpdatePhoneNumber_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
		}
		else if (name == "text_input_phone_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_phone")->Visible(true);
		}
		else if (name == "text_input_address_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_address")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_address")->Visible(false);
			}
		}
		else if (name == "text_input_address_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_address")->Visible(false);
			UpdateAddress_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
		}
		else if (name == "text_input_address_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_address")->Visible(true);
		}
		else if (name == "text_input_cmnd_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd")->Visible(false);
			}
		}
		else if (name == "text_input_cmnd_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd")->Visible(false);
			UpdateCMNDNumber_ToServer(atoi(static_cast<TextFieldWidget*>(_widget)->GetText().GetString()));
		}
		else if (name == "text_input_cmnd_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd")->Visible(true);
		}
		else if (name == "text_input_status_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_status")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_status")->Visible(false);
			}
		}
		else if (name == "text_input_status_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_status")->Visible(false);
			UpdateStatus_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
		}
		else if (name == "text_input_status_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_status")->Visible(true);
		}
		else if (name == "text_input_email_ATTACH_IME")
		{
			RKString str = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (str.Length() > 1)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_email")->Visible(true);
			}
			else
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_email")->Visible(false);
			}
		}
		else if (name == "text_input_email_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_email")->Visible(false);
			UpdateEmail_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
		}
		else if (name == "text_input_email_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_email")->Visible(true);
		}

		else if (name == "send_account_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else if (name == "send_account_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
		}
		else if (name == "send_account_input_INSERT_TEXT")
		{

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
		}
		else if (name == "number_of_win_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else if (name == "number_of_win_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
		}
		else if (name == "number_of_win_input_INSERT_TEXT")
		{

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
		}


	}
	
}

void ProfileScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(PROFILE_SCREEN);

	if (p_thread_check_time)
	{
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}

	p_menu_show_next = MENU_NONE;

	if (p_user_dirty)
	{
		if (GetUser)
		{
			GlobalService::UpdateProfile(GetUser);
		}
		p_user_dirty = false;
	}
	if (p_status_dirty)
	{
		if (GetUser)
		{
			GlobalService::UpdateStatus(GetUser->status);
		}
		p_status_dirty = false;
	}

	if(ScrMgr->GetGamePlayMgr() && ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen() != nullptr)
	{
		ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen()->setVisible(true);
	}
}

void ProfileScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void ProfileScreen::OnBeginFadeIn()
{
	CreateListDate();
}

void ProfileScreen::CreateListDate()
{
	if (p_is_create_list_choose)
		return;

	p_is_create_list_choose = true;
	//gener
	for (int i = 0; i < 3; i++)
	{
		auto widget_ = static_cast<ListViewWidget*>(p_list_choose_sx)->CreateWidgetRefer(i);
		widget_->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, widget_->GetWidgetChildAtIdx(1));
		widget_->GetWidgetChildAtIdx(1)->SetName("sx_" + std::to_string(i));
		static_cast<TextWidget*>(widget_->GetWidgetChildByName(".title"))->SetText(RKString(list_sx[i]));
	}
	//day
	{
		int current_day = 1;
		auto _list_choose_day = static_cast<ListViewTableWidget*>(p_list_choose_day);
		_list_choose_day->addEventListener(
			[this, current_day](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_day), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_day == this->p_cur_day)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_day), true);
				this->OnShowOrHideListChoose(this->p_list_choose_day, false);
				this->p_cur_day = current_idx_to + current_day;
				this->RunThreadCheckDateTime();

			}
		});
		_list_choose_day->InitTable();

	}
	//month
	{
		int current_month = 1;
		auto _list_choose_month = static_cast<ListViewTableWidget*>(p_list_choose_month);
		_list_choose_month->addEventListener(
			[this, current_month](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_month), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_month == this->p_cur_month)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_month), true);
				this->OnShowOrHideListChoose(this->p_list_choose_month, false);
				this->p_cur_month = current_idx_to + current_month;
				this->RunThreadCheckDateTime();

			}
		});
		_list_choose_month->InitTable();
	}
	//year
	{
		int current_year = 1917;
		auto _list_choose_year = static_cast<ListViewTableWidget*>(p_list_choose_year);
		_list_choose_year->addEventListener(
			[this , current_year](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_year), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_year == this->p_cur_year)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_year), true);
				this->OnShowOrHideListChoose(this->p_list_choose_year, false);
				this->p_cur_year = current_idx_to + current_year;
				//this->RunThreadCheckDateTime();

			}
		});
		_list_choose_year->InitTable();
	}
}

void ProfileScreen::ParseUserUI()
{
	User * p_user = GetUser;
	if (p_user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}

	SetAddress(p_user->address);
	SetAccountName(p_user->nickName);
	SetPlayerName(p_user->fullname);
	SetSx(p_user->gender);
	SetPhoneNumber(p_user->phoneNo);
	SetBirthDay(p_user->dayOfBirth, p_user->monthOfBirth, p_user->yearOfBirth);
	SetCMNDNumber(p_user->idnumber);
	SetLevel(GameController::myInfo->getLevel());
	SetHourPlayed(p_user->playingTime);
	SetWinNumber(p_user->money);
	SetEmail(p_user->email);

	long money = p_user->money;
	long ruby = p_user->ruby;

	auto w_money = p_page_profile_left->GetWidgetChildByName(".add_iwin_panel");
	auto w_ruby = p_page_profile_left->GetWidgetChildByName(".add_ruby_panel");

	static_cast<TextWidget*>(w_money->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(money));
	static_cast<TextWidget*>(w_ruby->GetWidgetChildAtIdx(1))->SetText(Utils::formatNUmber_dot(ruby));


	p_page_profile_name->GetWidgetChildByName(".title_8.confirm_email")->Visible(!p_user->isEmailValidate);
	p_page_profile_name->GetWidgetChildByName(".title_8.txt_confirm")->Visible(p_user->isEmailValidate);
	p_page_profile_name->GetWidgetChildByName(".title_8.panel_input.text_input_email")->SetDisable(p_user->isEmailValidate);


	p_page_profile_name->GetWidgetChildByName(".title_5.confirm_phone")->Visible(!p_user->isPhoneValidate);
	p_page_profile_name->GetWidgetChildByName(".title_5.txt_confirm")->Visible(p_user->isPhoneValidate);
	p_page_profile_name->GetWidgetChildByName(".title_5.panel_input.text_input_phone")->SetDisable(p_user->isPhoneValidate);

	GameMgr->HandleAvatarDownload(
		p_user->avatarID, p_user->nickName + "_profile_scr", [this](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(p_panel_avatar, data, str, tag);
	});
}

void ProfileScreen::onMyAvatars(std::vector<Item*> items)
{
}

void ProfileScreen::onMyItems(std::vector<Item*> items, int cateId, int totalPage, ubyte gender) 
{
}

void ProfileScreen::onEventItems(std::vector<Item*> items) 
{
	//showLoadingCircle(false, userInfoScroll);
}

void ProfileScreen::onChangeMyAvatar()
{
	User * p_user = GetUser;
	if (p_user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}

	p_user->avatarID = GameController::myInfo->avatar;
	ScrMgr->OnShowDialog("notice", "Bạn đã đổi avatar thành công.", TYPE_DIALOG::DIALOG_ONE_BUTTON);
	if (p_previous_page_idx == 0)
	{
		GlobalService::getMyAvatars();
	}
}

void ProfileScreen::onSetAchievement(void * data)
{
	//p_page_view_achieve->ClearChild();
	for (size_t i = 0; i < p_list_achieve.size(); i++)
	{
		delete p_list_achieve.at(i);
	}
	p_list_achieve.clear();

	std::vector<Achievement*> p_achie = *(std::vector<Achievement*>*)data;
	p_list_achieve = p_achie;
	//
	PASSERT2(p_list_achieve.size() > 0 , "list achievement is zero!");
	if (p_list_achieve.size() < 0)
	{
		return;
	}
	//
	p_receive_data_achievement = true;

}

void ProfileScreen::OnShowOrHideListChoose(WidgetEntity * list, bool Show)
{
	if (Show && !list->Visible())
	{
		list->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this , list]()
		{
			this->SetInteractiveInsteadOf(false, list);
			list->ForceFinishAction();
			list->GetParentWidget()->SetZOrder(100);
		}));
	}
	else if (!Show && list->Visible())
	{
		list->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this, list]() {
			this->GetRoot()->SetInteractive(true);
			list->ForceFinishAction();
			list->GetParentWidget()->SetZOrder(0);
		}));
	}
}

void ProfileScreen::SetStatusPlayer(RKString status)
{
	auto wi = p_page_profile_left->GetWidgetChildByName(".player_status.text_input_status");
	static_cast<TextFieldWidget*>(wi)->SetText(status);
}


void ProfileScreen::SetAccountName(RKString name)
{
	auto wi = p_page_profile_left->GetWidgetChildByName(".title_name_player");
	auto wi_1 = p_page_profile_name->GetWidgetChildByName(".title_1.title_detail");

	static_cast<TextWidget*>(wi)->SetText(name);
	static_cast<TextWidget*>(wi_1)->SetText(name);
}

void ProfileScreen::SetPlayerName(RKString name)
{
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_2.panel_input.text_input_1");
	static_cast<TextFieldWidget*>(wi)->SetText(name);
}

void ProfileScreen::SetSx(int id)
{
	if (id >= 3 || id < 0)
		id = 0;
	RKString sx_type = list_sx[id];
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_input_sx.btn_input_sx.text_input");
	static_cast<TextWidget*>(wi)->SetText(sx_type);
}

void ProfileScreen::SetPhoneNumber(RKString number)
{
	if (number == "")
		return;
	RKString number_phone = (number);
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_5.panel_input.text_input_phone");
	static_cast<TextFieldWidget*>(wi)->SetText(number_phone);
}

void ProfileScreen::SetBirthDay(int day, int month, int year)
{
	auto wday = p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_day.text_input");
	auto wmon = p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
	auto wyear = p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
	static_cast<TextWidget*>(wday)->SetText(std::to_string(day), true);
	static_cast<TextWidget*>(wmon)->SetText(std::to_string(month), true);
	static_cast<TextWidget*>(wyear)->SetText(std::to_string(year), true);

	p_cur_day = day;
	p_cur_month = month;
	p_cur_year = year;

	RunThreadCheckDateTime();
}

void ProfileScreen::SetAddress(RKString address)
{
	if (address == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_6.panel_input.text_input_address");
	static_cast<TextFieldWidget*>(wi)->SetText(address);
}

void ProfileScreen::SetCMNDNumber(RKString number)
{
	RKString number_CMND = (number);
	if (number == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_7.panel_input.text_input_cmnd");
	static_cast<TextFieldWidget*>(wi)->SetText(number_CMND);
}

void ProfileScreen::SetEmail(RKString email)
{
	if (email == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_8.panel_input.text_input_email");
	static_cast<TextFieldWidget*>(wi)->SetText(email);
}

void ProfileScreen::SetLevel(int level)
{
	RKString number_ = std::to_string(level);
	auto wi = p_page_profile_left->GetWidgetChildByName(".panel_text.title_level_detail");
	static_cast<TextWidget*>(wi)->SetText(number_, true);
}

void ProfileScreen::SetWinNumber(int number_win)
{
	RKString number_ = std::to_string(number_win);
	auto wi = p_page_profile_left->GetWidgetChildByName(".add_iwin_panel");
	static_cast<TextWidget*>(wi->GetWidgetChildAtIdx(1))->SetText(number_, true);
}

void ProfileScreen::SetRubyNumber(int number_ruby)
{
	RKString number_ = std::to_string(number_ruby);
	auto wi = p_page_profile_left->GetWidgetChildByName(".add_ruby_panel");
	static_cast<TextWidget*>(wi->GetWidgetChildAtIdx(1))->SetText(number_, true);
}


void ProfileScreen::SetHourPlayed(RKString hour)
{
	if (hour == "")
		return;
	RKString number_ = (hour);
	auto wi = p_page_profile_left->GetWidgetChildByName(".panel_text.title_time_play_detail");
	static_cast<TextWidget*>(wi)->SetText(number_, true);
}

bool ProfileScreen::CheckValidateDay(int &day, int &month, int &year, bool recorrect )
{
	if (year < 1917 )
	{
		if (recorrect) { year = 1917; }
		else { PASSERT2(year > 1917 , "wrong year"); }
		return CheckValidateDay(day, month, year, recorrect);
	}

	bool is_nam_nhuan = (year % 4 == 0 && year % 100 == 0);
	if (month > 12 || month < 1)
	{
		if (recorrect) { month = 1; } else { PASSERT2(month <= 12 && month >= 1, "wrong month"); }
		return CheckValidateDay(day,month,year , recorrect);
	}

	if (month == 2)
	{
		if (is_nam_nhuan)
		{
			if (day > 29)
			{
				if (recorrect){ day = 29; } else { PASSERT2(day < 29, "wrong day"); }
				return false;
			}
		}
		else
		{
			if (day > 28)
			{
				if (recorrect) { day = 28; }else { PASSERT2(day < 28, "wrong day"); }
				return CheckValidateDay(day, month, year, recorrect);
			}
		}
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		if (day > 30)
		{
			if (recorrect) { day = 30; }else { PASSERT2(day < 30, "wrong day"); }
			return CheckValidateDay(day, month, year, recorrect);
		}
	}
	
	if (day > 31 || day < 1)
	{
		if (recorrect) { day = 1; }else { PASSERT2(day <= 31 && day >= 1, "wrong day"); }
		return CheckValidateDay(day, month, year, recorrect);
	}

	return true;
}

void ProfileScreen::RunThreadParseLB()
{
	auto _list_view_achieve = static_cast<ListViewTableWidget*>(p_page_view_achieve);

	_list_view_achieve->addEventListener(
		[this, _list_view_achieve](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{

			int current_idx_to = _list_view_achieve->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_list_achieve.size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->p_list_achieve.size() - 1;
			}

			OnParseAchievementIntoGame(_widget, this->p_list_achieve[current_idx_to], current_idx_to);
		}
	});

	_list_view_achieve->SetNumberSizeOfCell(p_list_achieve.size());
}

void ProfileScreen::RunThreadCheckDateTime()
{
	if (p_thread_check_time && p_thread_check_time->IsThreadRunning())
	{
		//avoid spam thread
		return;
	}

	//delete current theard if its not run anymore
	if (p_thread_check_time)
	{
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}

	//create thread
	p_thread_check_time = new ThreadWrapper();
	p_thread_check_time->CreateThreadWrapper("thread_check_date_time",
		([](void * data) -> uint32
	{
		ProfileScreen * scr = (ProfileScreen*)data;
		scr->p_thread_check_time->OnCheckUpdateThreadWrapper
		([scr]() 
		{
			scr->OnRecorrectDateTime();
		});
		return 1;
	}),
	(void*)this);
}

void ProfileScreen::OnRecorrectDateTime()
{
	bool res = CheckValidateDay(p_cur_day, p_cur_month, p_cur_year, true);
}

void ProfileScreen::OnParseAchievementIntoGame(WidgetEntity* widget , Achievement * achie , int idx)
{
	PASSERT2(achie != nullptr, "incorrect data");
	//PASSERT2(idx < p_page_view_achieve->GetNumberChildWidget() && idx >= 0, "idx out of list achievement!");

	PASSERT2(widget != nullptr, "widget is nullptr !");

	auto detail = ScrMgr->GetGameDetailByType((GameType)(achie->gameId));

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title"))->SetText(detail.name);
	auto panel_layout = widget->GetWidgetChildByName(".layout_");
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".panel_exp_score.title"))->SetText(std::to_string(achie->exp), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level_number"))->SetText(std::to_string(achie->eloNumber), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level"))->SetText(achie->elo, true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_win_number"))->SetText(std::to_string(achie->win), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_lose_number"))->SetText(std::to_string(achie->lose), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_draw_number"))->SetText(std::to_string(achie->draw), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_leave_number"))->SetText(std::to_string(achie->givenUp), true);
}

void ProfileScreen::UpdatePlayerName_ToServer(RKString name)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->fullname = name.GetString();
	p_user_dirty = true;
}

void ProfileScreen::UpdateSx_ToServer(int id)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->gender = id;
	p_user_dirty = true;
}

void ProfileScreen::UpdatePhoneNumber_ToServer(RKString number)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->phoneNo = number.GetString();
	p_user_dirty = true;
}

void ProfileScreen::UpdateBirthDay_ToServer(int day, int month, int year)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->monthOfBirth = month;
	GetUser->dayOfBirth = day;
	GetUser->yearOfBirth = year;
	p_user_dirty = true;
}

void ProfileScreen::UpdateAddress_ToServer(RKString address)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->address = address.GetString();
	p_user_dirty = true;
}

void ProfileScreen::UpdateCMNDNumber_ToServer(int number)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->idnumber = std::to_string(number);
	p_user_dirty = true;
}

void ProfileScreen::UpdateStatus_ToServer(RKString status)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->status = status.GetString();
	p_user_dirty = true;
}

void ProfileScreen::UpdateEmail_ToServer(RKString email)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->email = email.GetString();
	p_status_dirty = true;
}

void ProfileScreen::OnParseInfoAvaToLayout(WidgetEntity * widget, iwinmesage::Avatar * info, bool request_avatar)
{
	PASSERT2(widget != nullptr, "widget is null");
	PASSERT2(info != nullptr, "info is null");

	int ID = info->getId();
	int cateID = info->getCateId();
	bool isBestSeller = info->getBestSeller();
	int day_exp = info->getExpireDay();
	auto cash_cost = info->getMoney();
	RKString name = info->getName();
	int have_bought = info->getBought();

	auto btn = widget; // ->GetWidgetChildAtIdx(0);

	static_cast<TextWidget*>(btn->GetWidgetChildByName(".title_"))->SetText(info->getName(), true);
	static_cast<TextWidget*>(btn->GetWidgetChildByName(".title_money"))->SetText( Utils::formatNUmber_dot(cash_cost), true);

	auto panel_icon = btn->GetWidgetChildByName(".icon_bg");
	panel_icon->GetWidgetChildByName(".ico_hot")->SetVisible(isBestSeller);
	RKString day = LangMgr->GetString("days");
	RKString title_day = Utils::formatNUmber_dot(day_exp) + " " + day;

	if (have_bought == 0)
	{
		auto w_title = panel_icon->GetWidgetChildByName(".bg_yeallow.title_");
		static_cast<TextWidget*>(w_title)->SetText(title_day, true);
		w_title->Visible(true);
		panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought")->Visible(false);
	}
	else
	{
		panel_icon->GetWidgetChildByName(".bg_yeallow.title_")->Visible(false);
		panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought")->Visible(true);
		auto w_title = panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought.title_1");
		static_cast<TextWidget*>(w_title)->SetText(title_day, true);
	}

	name = name + std::to_string(ID);

	if (!request_avatar)
	{
		if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(name.GetString()) != p_list_avatar_download.end())
		{
			this->ParseSpriteToPanel(panel_icon, p_list_avatar_download.at(name.GetString()), 100);
		}
	}
	else
	{
		panel_icon->retain();
		GameMgr->HandleURLImgDownload(info->getImg(), name,
			[this, panel_icon](void * data, void * str, int tag)
		{
			this->ParseImgToPanel(panel_icon, data, str, 1);
			panel_icon->release();
		});
	}

}

void ProfileScreen::OnParseInfoAvaCatToLayout(WidgetEntity * widget, iwinmesage::AvatarCat * info)
{
	PASSERT2(widget != nullptr, "widget is null");
	PASSERT2(info != nullptr, "info is null");

	int id = info->getId();
	auto btn = widget->GetWidgetChildAtIdx(0);
	static_cast<ButtonWidget*>(btn)->SetCallBack([this,id](Ref * ref, ui::Widget::TouchEventType event) {
		if (event == ui::Widget::TouchEventType::ENDED)
		{
			GlobalService::getAvatarOfCategory(id);

			p_shop_avatar_list_cat->SetActionCommandWithName("ON_HIDE");
			p_shop_avatar_list->SetActionCommandWithName("ON_SHOW");
		}
	});

	static_cast<TextWidget*>(btn->GetWidgetChildByName(".top_bg.title_"))->SetText(info->getName(), true);
	auto panel_icon = btn->GetWidgetChildByName(".icon_bg");

	panel_icon->retain();
	GameMgr->HandleURLImgDownload(
		info->getImg(), info->getName(), [this , panel_icon](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(panel_icon, data, str,tag);
		panel_icon->release();
	});
}

void ProfileScreen::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	if (value == nullptr)
		return;

	if (p_shop_avatar_list_cat->GetNumberChildWidget() > 0)
	{
		//remove old ava cat
		for (size_t i = 0; i < p_shop_avatar_list_cat->GetNumberChildWidget(); i++)
		{
			auto w = p_shop_avatar_list_cat->GetWidgetChildAtIdx(i)->GetWidgetChildByName(".btn_shop.icon_bg");
			auto child = w->GetResource()->getChildByTag(99);
			if (child)
			{
				w->GetResource()->removeChildByTag(99);
			}
		}
		p_shop_avatar_list_cat->ClearChild();
	}

	size_t number_avatar_cat = value->getList().size();
	
	for (size_t i = 0; i < number_avatar_cat; i++)
	{
		auto widget = static_cast<ListViewWidget*>(p_shop_avatar_list_cat)->CreateWidgetRefer(i);
		OnParseInfoAvaCatToLayout(widget, &value->getList()[i]);
	}

	delete value;
}

void ProfileScreen::onReceiveAvatarList(void * data)
{
	if (p_avatar_list)
	{
		delete p_avatar_list;
		p_avatar_list = nullptr;
		//chien todo : it support 4billion times, and afterthat will be crash , hope it's not happend :)
		p_stage_avatar_cat_list++;
	}

	p_avatar_list = (AvatarList *)data;
	if (p_avatar_list == nullptr)
		return;
	
	AvatarList * value = p_avatar_list;
	//if (p_shop_avatar_list->GetNumberChildWidget() > 0)
	{
		//remove old ava
		static_cast<ListViewTableWidget*>(p_shop_avatar_list)->ResetListView();
		CleanUpListAvatar();
	}

	size_t number_avatar = value->getList().size();
	auto _p_shop_avatar_list = static_cast<ListViewTableWidget*>(p_shop_avatar_list);

	_p_shop_avatar_list->addEventListener(
		[this, _p_shop_avatar_list,value](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = _p_shop_avatar_list->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= value->getList().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = value->getList().size() - 1;
			}
			OnParseInfoAvaToLayout(_widget->GetWidgetChildAtIdx(0), value->getList()[current_idx_to]);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			p_current_avatar_idx_buy = _p_shop_avatar_list->GetCurrentIdxSelected();
			this->p_confirm_buy_avatar_layout->GetParentWidget()->Visible(true);
			this->p_confirm_buy_avatar_layout->SetActionCommandWithName("ON_SHOW");
			this->SetInteractiveInsteadOf(false, this->p_confirm_buy_avatar_layout);

			OnParseInfoAvaToLayout(this->p_confirm_buy_avatar_layout->GetWidgetChildByName(".bg_avatar"), value->getList()[p_current_avatar_idx_buy] , true);
		}
	});

	_p_shop_avatar_list->SetNumberSizeOfCell(number_avatar);
	int current_stage_list = p_stage_avatar_cat_list;
	//download the avatar into the list
	for (size_t i = 0; i < number_avatar; i++)
	{
		auto idx_player = value->getList()[i];
		{
			GameMgr->HandleURLImgDownload(idx_player->getImg(), idx_player->getName(),
				[this, idx_player, i , current_stage_list](void * data, void * str, int tag)
			{
				Image * img = (Image*)data;
				if (current_stage_list != this->p_stage_avatar_cat_list)
				{
					PINFO("PROFILE SCREEN : avatar download and idx_player is delete , safe to continue!");
					CC_SAFE_DELETE(img);
				}
				else
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
					this->p_list_avatar_download.insert(idx_player->getName() + std::to_string(idx_player->getId()), sprite_img);

					auto widget_lb = static_cast<ListViewTableWidget*>(this->p_shop_avatar_list);
					if ((size_t)i < widget_lb->GetListWidgetSample().size())
					{
						auto widget = widget_lb->GetListWidgetSample()[i];
						if (widget)
						{
							auto panel_avatar = widget->GetWidgetChildAtIdx(0)->GetWidgetChildByName(".icon_bg");
							this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
						}
					}
				}
			});
		}


	}

	//delete value;
}

void ProfileScreen::CleanUpListAvatar()
{
	p_list_avatar_download.clear();
}

void ProfileScreen::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

void ProfileScreen::ShowLayoutState(int state)
{
	if (state == 0)
	{
		OnProcessWidgetChild("btn_info", UI_Widget::UI_TYPE::UI_BUTTON, nullptr);
	}
	else if (state == 1)
	{
		OnProcessWidgetChild("btn_achieve", UI_Widget::UI_TYPE::UI_BUTTON, nullptr);
	}
}

bool ProfileScreen::IsShopAvatarVisible()
{
	return p_shop_avatar_layout->GetParentWidget()->Visible();
}

