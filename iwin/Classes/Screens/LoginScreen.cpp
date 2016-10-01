#include "LoginScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"

#include "UI/AdvanceUI/wListView.h"
#include "Features/FacebookMgr.h"
#include "Features/IapMgr.h"
#include "Network/Core/ServerMgr.h"
#include "Platform/WindowsPhone/WPPlatform.h"
#include "Platform/Platform.h"

#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"
#include "Network/Global/Globalservice.h"
LoginScreen::LoginScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_panel_choose_server = nullptr;
	p_panel_check_name_1 = nullptr;
	p_btn_login = nullptr;
	p_btn_create_user = nullptr;
	p_panel_check_name_input = nullptr;
	p_panel_check_repass_input = nullptr;
	p_already_auto_login = false;
	p_id_captcha = "";
	p_panel_captcha = nullptr;
	p_password = "";
	p_stage_choose_method_getpass = 0;

	p_panel_first_choose_login = nullptr;
}

LoginScreen::~LoginScreen()
{
	p_panel_choose_server = nullptr;
	p_panel_check_name_1 = nullptr;
	p_btn_login = nullptr;
	p_btn_create_user = nullptr;
	p_panel_check_name_input = nullptr;
	p_panel_check_repass_input = nullptr;
	p_already_auto_login = false;
	p_id_captcha = "";
	p_panel_captcha = nullptr;
	p_panel_first_choose_login = nullptr;
}

int LoginScreen::Init()
{
	RKString _menu = "login_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (cur_ratio <= 1.34) //ipad ratio
	{
//		_menu = "login_screen_ipad";
	}

	InitMenuWidgetEntity(_menu);
	
	p_list_server = GetWidgetChildByName("main_login_screen.panel_choose_server.list_server");
	auto list_server = GetServerMgr()->getServerList();
	for (size_t i = 0; i < list_server.size(); i++)
	{
		auto layout_ = static_cast<ListViewWidget*>(p_list_server)->CreateWidgetRefer(i);
		static_cast<TextWidget*>(layout_->GetWidgetChildByName(".server_choose_btn.server_name"))->SetText(list_server.at(i)._name);
		if (i == 0)
		{
			layout_->GetWidgetChildByName(".stick_icon")->Visible(true);
			layout_->GetWidgetChildByName(".highlights")->Visible(true);
			static_cast<TextWidget*>(GetWidgetChildByName("main_login_screen.btn_server.name_server"))->SetText(GetServerMgr()->getCurServer()._name);
		}
	}

	CallInitComponent(true);

	return 1;
}

int	LoginScreen::InitComponent()
{
	p_panel_first_choose_login = GetWidgetChildByName("main_login_screen.first_login_layout");
	p_panel_choose_server = GetWidgetChildByName("main_login_screen.panel_choose_server");
	p_panel_register_user = GetWidgetChildByName("main_login_screen.register_layout");
	p_panel_login_layout = GetWidgetChildByName("main_login_screen.login_layout");

	p_panel_forget_pass = GetWidgetChildByName("main_login_screen.forget_pass_layout");

	p_panel_check_name_1 = p_panel_login_layout->GetWidgetChildByName(".panel_check_name");

	p_panel_check_name_input = p_panel_register_user->GetWidgetChildByName(".panel_check_name_input");
	p_panel_check_pass_input = p_panel_register_user->GetWidgetChildByName(".panel_check_pass_input");
	p_panel_check_repass_input = p_panel_register_user->GetWidgetChildByName(".panel_check_repass_input");

	p_btn_login = p_panel_login_layout->GetWidgetChildByName(".login_game_btn");
	p_btn_create_user = p_panel_register_user->GetWidgetChildByName(".create_acc_btn");

	static_cast<ui::Button*>(p_btn_create_user->GetResource())->setEnabled(false);
	p_btn_create_user->SetActionCommandWithName("SET_DISABLE");

	auto btn_agree_term = p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_2");
	Vec2 pos = btn_agree_term->GetPosition();
	Vec2 size = btn_agree_term->GetSize();

	p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_1")->GetResource()->setPositionX(pos.x - ((size.x / 2.f) + 4));
	p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_3")->GetResource()->setPositionX(pos.x + ((size.x / 2.f) + 4));

	p_panel_captcha = p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.captcha_panel");

	GameMgr->Network_GetCaptchaInfo(p_id_captcha);

	return 1;
}

int LoginScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}
    
	return 1;
}

void LoginScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void LoginScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void LoginScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	CheckTouchOutSideWidget(p, p_panel_choose_server,
		[&](void) {
		p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this]() {
			this->p_panel_choose_server->ForceFinishAction();
			this->p_panel_choose_server->Visible(false);
		}));
	});
}

void LoginScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "text_input_1_DETACH_IME" )
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_name_btn")->Visible(false);
            RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
            if(!IsValidString(string_input.GetString()))
            {
                static_cast<ui::Button*>(p_btn_login->GetResource())->setEnabled(false);
                p_btn_login->SetActionCommandWithName("SET_DISABLE");
            }
            else
            {
                static_cast<ui::Button*>(p_btn_login->GetResource())->setEnabled(true);
                p_btn_login->SetActionCommandWithName("SET_ENABLE");
            }
		}
		else if (name == "text_input_1_ATTACH_IME" )
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (!p_panel_check_name_1->Visible() && !IsValidString(string_input.GetString()))
			{
				p_panel_check_name_1->Visible(true);
			}

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_name_btn")->Visible(true);
			}
		}
		else if (name == "text_input_1_INSERT_TEXT")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (!p_panel_check_name_1->Visible() && !IsValidString(string_input.GetString()))
			{
				p_panel_check_name_1->Visible(true);
			}
			else if (p_panel_check_name_1->Visible())
			{
				p_panel_check_name_1->GetWidgetChildAtIdx(1)->Visible(true);
			}

			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_name_btn")->Visible(true);
		}
		else if (name == "text_input_1_DELETE_BACKWARD")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (p_panel_check_name_1->Visible() && IsValidString(string_input.GetString()))
			{
				p_panel_check_name_1->Visible(false);
			}
		}

		else if (name == "text_input_create_name_DETACH_IME" )
		{
			RKString string_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->GetText();

			if (string_input.Length() < 4 || string_input.Length() > 21)
			{
				p_panel_check_name_input->Visible(true);
				auto panel = p_panel_check_name_input->GetWidgetChildAtIdx(1);
				panel->Visible(true);
				panel->SetActionCommandWithName("SHOW_2");
				panel->GetWidgetChildAtIdx(0)->SetActionCommandWithName("TEXT_2");

			}
			else if (!IsValidString(string_input.GetString()))
			{
				p_panel_check_name_input->Visible(true);
				auto panel = p_panel_check_name_input->GetWidgetChildAtIdx(1);
				panel->Visible(true);
				panel->SetActionCommandWithName("SHOW_1");
				panel->GetWidgetChildAtIdx(0)->SetActionCommandWithName("TEXT_1");
			}
			else
			{
				GameMgr->Network_CheckUserNameExist(string_input);
			}
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_name_btn")->Visible(false);
			OnCheckEnableBtnCreateUser();
		}
		else if (name == "text_input_create_name_ATTACH_IME" )
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			
			if (p_panel_check_name_input->Visible())
			{
				p_panel_check_name_input->GetWidgetChildAtIdx(1)->Visible(false);
			}

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_name_btn")->Visible(true);
			}
		}
		else if (name == "text_input_create_name_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_name_btn")->Visible(true);
			p_panel_check_name_input->Visible(false);
		}

		else if (name == "text_input_create_pass_DETACH_IME" )
		{
			p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);

			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_pass_btn")->Visible(false);
			OnCheckEnableBtnCreateUser();
		}
		else if (name == "text_input_create_pass_ATTACH_IME" )
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_pass_btn")->Visible(true);
			}
		}
		else if (name == "text_input_create_pass_INSERT_TEXT")
		{
			RKString pass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString name_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->GetText();

			CheckInputPass(pass_input, name_input);
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_pass_btn")->Visible(true);
		}
		else if (name == "text_input_create_pass_DELETE_BACKWARD")
		{
			RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();
			RKString name_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->GetText();

			CheckInputPass(pass_input, name_input);
		}

		else if (name == "text_input_create_repass_DETACH_IME" )
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();

			if (repass_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(true);
			}
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_repass_btn")->Visible(false);
			OnCheckEnableBtnCreateUser();
		}
		else if (name == "text_input_create_repass_ATTACH_IME" )
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();

			if (repass_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
			}

			if (repass_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_repass_btn")->Visible(true);
			}
		}
		else if (name == "text_input_create_repass_INSERT_TEXT")
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();

			if (repass_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
			}
			else
			{
				p_panel_check_repass_input->Visible(false);
			}

			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_repass_btn")->Visible(true);
		}
		else if (name == "text_input_create_repass_DELETE_BACKWARD")
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();

			if (repass_input == pass_input)
			{
				p_panel_check_repass_input->Visible(false);
			}
		}

		else if (name == "title_1_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_pass_btn")->Visible(true);
		}
		else if (name == "title_1_DETACH_IME" )
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_pass_btn")->Visible(false);
		}
		else if (name == "title_1_ATTACH_IME" )
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_pass_btn")->Visible(true);
			}
		}
		
		else if (name == "title_captcha_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_captcha_btn")->Visible(false);
			OnCheckEnableBtnCreateUser();
		}
		else if (name == "title_captcha_ATTACH_IME")
		{
			RKString captcha = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (captcha.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_captcha_btn")->Visible(true);
			}
		}
		else if (name == "title_captcha_INSERT_TEXT")
		{			
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_captcha_btn")->Visible(true);
		}

		else if (name == "text_input_getpass_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_getpass_btn")->Visible(false);
		}
		else if (name == "text_input_getpass_ATTACH_IME")
		{
			RKString captcha = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (captcha.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_getpass_btn")->Visible(true);
			}
		}
		else if (name == "text_input_getpass_INSERT_TEXT")
		{
			_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_getpass_btn")->Visible(true);
		}

	}
	
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "server_choose_btn")
		{
			int idx_list_select = static_cast<ListViewWidget*>(p_list_server)->GetCurrentIdxSelected();
			if (idx_list_select >= 0)
			{
				onSetServer(idx_list_select);
				ScrMgr->SaveDataInt(STR_SAVE_SERVER_CHOOSE, idx_list_select);
				ScrMgr->SaveTheGame();
			}

			static_cast<ListViewWidget*>(p_list_server)->SetCurrentIdxSelected(-1);

			p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_panel_choose_server->ForceFinishAction();
				this->p_panel_choose_server->Visible(false);
				SetInteractiveInsteadOf(true, this->p_panel_choose_server);
			}));
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
				}
				));
				SetInteractiveInsteadOf(true, p_panel_choose_server);
			}
			else
			{
				p_panel_choose_server->SetActionCommandWithName("ON_SHOW");
				p_panel_choose_server->Visible(true);
				SetInteractiveInsteadOf(false, p_panel_choose_server);
			}
		}
		else if (name == "login_game_btn")
		{
			RKString username = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->GetText();
			RKString pass = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->GetText();
			if (username == "" || pass == "")
				return;


			//startWaitingDialog();
            
            ScrMgr->OnShowDialog("logging_in", "logging_in_wit", DIALOG_WAITING_BUTTON);
            
			GameMgr->Network_LoginToGame(username.GetString(), pass.GetString());
			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->ClearText();
			//GameController::myInfo->setName(username.GetString());
			//GameMgr->Network_LoginToGame("test111", "t");
            p_password = pass;
            p_username = username;
			ScrMgr->SaveDataString(STR_SAVE_NAME_LOGIN, username);
			ScrMgr->SaveDataString(STR_SAVE_PASS_LOGIN, pass);
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_IWIN);
			ScrMgr->SaveTheGame();
			//ScrMgr->SwitchToMenu(MENU_LAYER::LOADING_SCREEN, LOGIN_SCREEN);
		}
		else if (name == "create_account_btn")
		{
			p_panel_login_layout->Visible(false);
			p_panel_register_user->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
				this->p_panel_register_user->ForceFinishAction();
				ScrMgr->SetScreenIDDirty();
			}));
		}
		else if (name == "have_acc_btn")
		{
			p_panel_login_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
				this->p_panel_login_layout->ForceFinishAction();
				ScrMgr->SetScreenIDDirty();
			}));
			p_panel_register_user->Visible(false);

		}
		else if (name == "forget_pass_title")
		{
			p_panel_login_layout->Visible(false);
			p_panel_forget_pass->SetActionCommandWithName("ON_SHOW");

		}
		else if (name == "have_acc_btn_pass")
		{
			p_panel_login_layout->SetActionCommandWithName("ON_SHOW");
			p_panel_forget_pass->Visible(false);

		}
		else if (name == "fb_btn" || name == "btn_login_fb")
		{
			startWaitingDialog();

			if (!GetFacebookMgr()->isLoggedIn())
			{
				GetFacebookMgr()->login();
			}
			else
			{
				GameMgr->Network_loginFacebook(GetFacebookMgr()->getAccessToken());
			}
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_FACEBOOK);
			ScrMgr->SaveTheGame();
		}
		else if (name == "create_acc_btn")
		{
			RKString captcha =	static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->GetText();		
			ScrMgr->OnShowDialog(
				"create_account", L"Tài khoản đang được tạo", DIALOG_ZERO_BUTTON
			);
			
			GameMgr->Network_CheckCaptchaResult(p_id_captcha, captcha);
		}
		else if (name == "reget_pass_btn")
		{
			RKString username = static_cast<TextFieldWidget*>(p_panel_forget_pass->GetWidgetChildByName(".panel_name_input_pass.text_input_getpass"))->GetText();
			if (username != "")
			{
				ScrMgr->OnShowDialog("notice", "pending", DIALOG_ONE_BUTTON);
				if (p_stage_choose_method_getpass == 0)
				{
					GameMgr->Network_HandleGetForgottenPassword(0, username);
				}
				else if(p_stage_choose_method_getpass == 1)
				{
					//GameMgr->Network_getSMSInfoForgotPassword(username);
					GameMgr->Network_HandleGetForgottenPassword(1, username);
				}
			}
		}

		else if (name == "clear_text_input_name_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_1");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "clear_text_input_pass_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".title_1");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "clear_text_input_create_name_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_create_name");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "clear_text_input_create_pass_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_create_pass");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "clear_text_input_create_repass_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_create_repass");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "clear_text_input_getpass_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".text_input_getpass");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}

		else if (name == "btn_icon_warning_name_input_1")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
		}
		else if (name == "btn_icon_warning_name_input")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
		}
		else if (name == "btn_icon_warning_pass_input")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
		}
		else if (name == "btn_icon_warning_repass_input")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
		}
		else if (name == "gg_btn" || name == "btn_login_google")
		{
			GetIapMgr()->purchase("iwin.vn.full.1", "aaa");
		}
		else if (name == "btn_question")
		{
			//Platform::showWeb("https://iwin.vn");
#if defined OS_W8 || defined WP8
			//WPPlatform::WPHelper::actionShowWebView("https://iwin.vn/");
#endif
			Platform::sendSMS("Pham Do Truong", "01633222744");
			//Platform::makeCall("01633222744");
		}

		else if (name == "reload_captcha")
		{
			GameMgr->Network_GetCaptchaInfo(p_id_captcha);
		}
		else if (name == "clear_captcha_btn")
		{
			auto text_field = _widget->GetParentWidget()->GetWidgetChildByName(".title_captcha");
			static_cast<TextFieldWidget*>(text_field)->ClearText();
			_widget->Visible(false);
		}
		else if (name == "btn_select_1")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(true);
			_widget->GetParentWidget()->GetParentWidget()->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(2)->Visible(false);
			p_stage_choose_method_getpass = 0;
		}
		else if (name == "btn_select_2")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(true);
			_widget->GetParentWidget()->GetParentWidget()->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(2)->Visible(false);
			p_stage_choose_method_getpass = 1;
		}

		else if (name == "btn_login_iwin")
		{
			p_panel_first_choose_login->Visible(false);
			p_panel_login_layout->SetActionCommandWithName("ON_SHOW");
		}
		else if (name == "btn_back")
		{
			p_panel_login_layout->Visible(false);
			p_panel_first_choose_login->SetActionCommandWithName("ON_SHOW");
		}
		else if (name == "btn_custommer")
		{

		}
		else if (name == "btn_sound")
		{
			auto ico = _widget->GetWidgetChildAtIdx(0);
			if (ico->Visible() == true)
			{
				_widget->SetOpacity(255);
				ico->Visible(false);
				//enable sound
				GetSound->ResetVolume();
				GetSound->ResetVolume(false);

				ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, GetSound->GetVolume() * 100.f);
				ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, GetSound->GetVolume(false) * 100.f);
				ScrMgr->SaveTheGame();
			}
			else
			{
				_widget->SetOpacity(0);
				ico->Visible(true);
				//disable sound
				GetSound->MuteVolume();
				GetSound->MuteVolume(false);

				ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, GetSound->GetVolume() * 100.f);
				ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, GetSound->GetVolume(false) * 100.f);
				ScrMgr->SaveTheGame();
			}
		}
	}
}

void LoginScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(LOGIN_SCREEN);

	p_menu_show_next = MENU_NONE;
}

void LoginScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

void LoginScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	GetSound->PlayMusicEffect("MUSIC_MAIN");
	onAutoLogin();
}

void LoginScreen::OnBeginFadeIn()
{
	int idx_server_select = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_SERVER_CHOOSE, idx_server_select))
	{
		onSetServer(idx_server_select);
	}

	if (p_menu_come_from != LOADING_SCREEN)
	{
		ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, -1);
		ScrMgr->SaveTheGame();
	}

}

void LoginScreen::CheckInputPass(RKString str_pass, RKString str_username)
{
	int result = IsValidPass(str_pass.GetString());
	bool res_1 = str_pass.Contains(str_username);
	bool res_2 = str_username.Contains(str_pass);

	if ((result != 0 || res_1 || res_2) && str_pass != "")
	{
		p_panel_check_pass_input->Visible(true);
		auto panel = p_panel_check_pass_input->GetWidgetChildAtIdx(1);
		panel->Visible(true);
		GetSound->PlaySoundEffect("SFX_BUZZ");
		if (res_1)
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::GREEN);
		}

		if (res_2)
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::GREEN);
		}


		if (result & 1)
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::GREEN);
		}

		if (result & 2)
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::GREEN);
		}

		if (result & 4)
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::GREEN);
		}
	}
	else
	{
		p_panel_check_pass_input->Visible(false);
	}

}

bool LoginScreen::IsValidString(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		int number_anscii = (unsigned char)str[i];

		if (!
			((number_anscii >= 48 && number_anscii <= 57)
			||
			(number_anscii >= 64 && number_anscii <= 90)
			||
			(number_anscii >= 97 && number_anscii <= 122))
			)
		{
			return false;
		}
	}
	return true;
}

int  LoginScreen::IsValidPass(std::string str)
{
	int result = 0;
	bool have_number = false;
	bool have_char = false;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (have_number && have_char)
			break;
		int number_anscii = (unsigned char)str[i];

		if ((number_anscii >= 48 && number_anscii <= 57))
		{
			have_number = true;
		}

		if((number_anscii >= 65 && number_anscii <= 90)
			||
			(number_anscii >= 97 && number_anscii <= 122))
		{
			have_char = true;
		}
	}
	if (!have_number)
	{
		result |= 1;
	}
	if (!have_char)
	{
		result |= 2;
	}
	if (str.length() < 6)
	{
		result |= 4;
	}
	return result;
}

bool LoginScreen::IsValidInputUser()
{
	RKString pass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();
	RKString repass_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_repass_input.text_input_create_repass"))->GetText();
	RKString name_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->GetText();
	RKString captcha = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->GetText();

	bool register_name_input_correct = !p_panel_check_name_input->Visible();
	bool register_pass_input_correct = !p_panel_check_pass_input->Visible();
	bool register_repass_input_correct = !p_panel_check_repass_input->Visible();

	if (register_name_input_correct 
		&& register_pass_input_correct 
		&& register_repass_input_correct 
		&& (pass_input == repass_input) 
		&& name_input != "" 
		&& pass_input != ""
		&& captcha != "")
		return true;

	return false;
}

void LoginScreen::OnCheckEnableBtnCreateUser()
{
	if (IsValidInputUser())
	{
		static_cast<ui::Button*>(p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->GetResource())->setEnabled(true);
		p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->SetActionCommandWithName("SET_ENABLE");
	}
	else
	{
		static_cast<ui::Button*>(p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->GetResource())->setEnabled(false);
		p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->SetActionCommandWithName("SET_DISABLE");
		GetSound->PlaySoundEffect("SFX_BUZZ");
	}
}


void LoginScreen::startWaitingDialog()
{
	ScrMgr->OnShowDialog(
		"logging_in", "logging_in_wit",
		DIALOG_WAITING_BUTTON
	//	[&](const char * call_back, const char* btn_name)
	//{
	//	if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::LOGIN_SCREEN
	//		//handle case reconnect
	//		&& ScrMgr->GetMenuUI(LOGIN_SCREEN)->GetMenuShowNext() != HUD_SCREEN
	//		)
	//	{
	//		ScrMgr->SwitchToMenu(MENU_LAYER::MAIN_SCREEN, LOGIN_SCREEN);
	//	}
	//	ScrMgr->SetStateLoading(100);
	//}
	);
}

void LoginScreen::onSmsSyntaxInfo(s16 subId, std::string content, std::string address, std::string confirmMsg)
{
}

void LoginScreen::onCallBackCheckUserName(RKString msg)
{
	if (msg == "")
		return;

	p_panel_check_name_input->Visible(true);
	auto panel = p_panel_check_name_input->GetWidgetChildAtIdx(1);
	panel->Visible(true);
	panel->SetActionCommandWithName("SHOW_3");
	static_cast<TextWidget*>(panel->GetWidgetChildAtIdx(0))->SetText(msg);

	OnCheckEnableBtnCreateUser();
}

void LoginScreen::onAutoLogin()
{
	if (p_already_auto_login)
		return;
	p_already_auto_login = true;

	int typelogin = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, typelogin))
	{
		switch ((LOGIN_TYPE)typelogin)
		{
		case LOGIN_NONE:
		{
			RKString user_name = "";
			ScrMgr->GetDataSaveString(STR_SAVE_NAME_LOGIN, user_name);
			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->SetText(user_name);

			break;
		}
		case LOGIN_IWIN:
		{
			RKString user_name = "";
			ScrMgr->GetDataSaveString(STR_SAVE_NAME_LOGIN, user_name);
			RKString pass_word = "";
			ScrMgr->GetDataSaveString(STR_SAVE_PASS_LOGIN, pass_word);

			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->SetText(user_name);
			//static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->SetText(pass_word);

			startWaitingDialog();
			GameMgr->Network_LoginToGame(user_name.GetString(), pass_word.GetString());
			break;
		}
		case LOGIN_FACEBOOK:
		{
			startWaitingDialog();

			if (!GetFacebookMgr()->isLoggedIn())
			{
				GetFacebookMgr()->login();
			}
			else
			{
				GameMgr->Network_loginFacebook(GetFacebookMgr()->getAccessToken());
			}
			break;
		}
		default:
			break;
		}
	}
}

void LoginScreen::onSetServer(int idx_server_select)
{
	if ((size_t)idx_server_select >= p_list_server->GetNumberChildWidget())
	{
		idx_server_select = 0;
	}

	auto current_layout = p_list_server->GetWidgetChildAtIdx(idx_server_select);
	current_layout->GetWidgetChildByName(".stick_icon")->Visible(true);
	current_layout->GetWidgetChildByName(".highlights")->Visible(true);

	int number_child_in_list = static_cast<ListView*>(p_list_server->GetResource())->getChildrenCount();
	for (int i = 0; i < number_child_in_list; i++)
	{
		if (i == idx_server_select)
		{
			continue;
		}
		auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
		current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
		current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);
	}
	GetServerMgr()->selectServer(idx_server_select);
	RKString name_server = GetServerMgr()->getCurServer()._name;
	static_cast<TextWidget*>(GetWidgetChildByName("main_login_screen.btn_server.name_server"))->SetText(name_server);
}

void LoginScreen::SetIDCaptcha(RKString id)
{
	p_id_captcha = id;
}

void LoginScreen::onReceiveCaptcha(iwinmesage::CaptchaInfo info)
{
	p_id_captcha = info.getId();
	auto panel = p_panel_captcha;
	GameMgr->HandleURLImgDownload(info.getImage(), p_id_captcha, [this, panel](void * data,void * str,int tag) {

		this->ParseImgToPanel(panel, data, str, tag);
	});
}

void LoginScreen::onReceiveCaptchaResult(iwinmesage::CaptchaResult info)
{
	if (info.getCode() == iwinmesage::CaptchaResult::CaptchaCheckType::SUCCESS)
	{
		RKString username = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->GetText();
		RKString pass =		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->GetText();
		p_password = pass;
		startWaitingDialog();
		CCLOG("Register now");
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_name_input.text_input_create_name"))->ClearText();
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_pass_input.text_input_create_pass"))->ClearText();
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_repass_input.text_input_create_repass"))->ClearText();
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->ClearText();
		//GameMgr->Network_RegisterNewAccount(username.GetString(), pass.GetString());
		
		p_panel_register_user->Visible(false);
		p_panel_login_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this, username, pass]() {
			this->p_panel_login_layout->ForceFinishAction();
			this->startWaitingDialog();
			static_cast<TextFieldWidget*>(this->p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->SetText(username);
			ScrMgr->SaveDataString(STR_SAVE_NAME_LOGIN, username);
			ScrMgr->SaveDataString(STR_SAVE_PASS_LOGIN, pass);
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_IWIN);
			ScrMgr->SaveTheGame();

			GlobalService::requestRegister(username.GetString(), pass.GetString());

		}));
	}
	else
	{
		ScrMgr->OnShowDialog(
			"error", info.getDesc(), DIALOG_ONE_BUTTON
		);
		GameMgr->Network_GetCaptchaInfo(p_id_captcha);
	}
}

RKString LoginScreen::GetPassword()
{
	RKString temp = p_password;
	//p_password = "";
	return temp;
}

 RKString LoginScreen::GetUsername()
{
    return p_username;
}

int LoginScreen::GetStateLoginMenu()
{
	if (p_panel_login_layout->Visible())
	{
		return 0;
	}
	else
	{
		if (p_panel_register_user->Visible())
		{
			return 1;
		}
	}
	return 2;
}

