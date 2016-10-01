#include "ChangePassLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"

ChangePassLayer::ChangePassLayer()
{
	_common_screen = nullptr;
}

ChangePassLayer::~ChangePassLayer()
{

}

void ChangePassLayer::InitChangePass(BaseScreen * screen)
{
	_common_screen = screen;

	p_layout_change_pass = _common_screen->GetWidgetChildByName("common_screen.change_pass_layout.change_pass");
	p_panel_check_pass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_new_pass.panel_check_pass_input");
	p_panel_check_repass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_confirm_new_pass.panel_check_repass_input");

	p_textfield_pass_input = p_panel_check_pass_input->GetParentWidget()->GetWidgetChildByName(".panel_input.new_pass_input");
	p_textfield_repass_input = p_panel_check_repass_input->GetParentWidget()->GetWidgetChildByName(".panel_input.confirm_new_pass_input");
	p_textfield_oldpass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_current_pass.panel_input.old_pass_input");

	p_btn_changepass = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.btn_change_pass");
}

bool ChangePassLayer::ProcessWidgetChangePass(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_clear_old_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();

			return true;
		}
		else if (name == "btn_clear_confirm_new_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();
			return true;
		}
		else if (name == "btn_clear_new_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();
			return true;
		}
		else if (name == "btn_icon_warning_repass_input")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			widget->Visible(!widget->Visible());
			return true;
		}
		else if (name == "btn_icon_warning_pass_input")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			widget->Visible(!widget->Visible());
			return true;
		}
		else if (name == "btn_close_change_pass")
		{
			p_layout_change_pass->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_change_pass->ForceFinishAction();
				this->p_layout_change_pass->GetParentWidget()->Visible(false);

				static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
				static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();
			}));
			return true;
		}
		else if (name == "btn_change_pass")
		{
			p_layout_change_pass->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_change_pass->ForceFinishAction();
				this->p_layout_change_pass->GetParentWidget()->Visible(false);
				static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
				static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();
				//send request change pass to server
				GlobalService::changePassword(
					static_cast<TextFieldWidget*>(p_textfield_oldpass_input)->GetText().GetString(),
					static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText().GetString());
			}));
			return true;
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "new_pass_input_DETACH_IME")
		{
			p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
			OnCheckEnableBtnChangePass();
			return true;
		}
		else if (name == "new_pass_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);
			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
			return true;
		}
		else if (name == "new_pass_input_INSERT_TEXT")
		{
			RKString pass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			CheckInputPass(pass_input, GetUser->nickName);
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			return true;
		}
		else if (name == "new_pass_input_DELETE_BACKWARD")
		{
			RKString pass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			CheckInputPass(pass_input, GetUser->nickName);
			return true;
		}

		else if (name == "old_pass_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
			OnCheckEnableBtnChangePass();
			return true;
		}
		else if (name == "old_pass_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
			return true;
		}
		else if (name == "old_pass_input_INSERT_TEXT")
		{
			//RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			return true;
		}
		else if (name == "old_pass_input_DELETE_BACKWARD")
		{
			//RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			return true;
		}

		else if (name == "confirm_new_pass_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);

			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

			if (repass_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(true);
			}
			OnCheckEnableBtnChangePass();

			return true;
		}
		else if (name == "confirm_new_pass_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

			if (string_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
			}

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
			return true;
		}
		else if (name == "confirm_new_pass_input_INSERT_TEXT")
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

			if (repass_input != pass_input)
			{
				p_panel_check_repass_input->Visible(true);
				p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
			}
			else
			{
				p_panel_check_repass_input->Visible(false);
			}

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			return true;
		}
		else if (name == "confirm_new_pass_input_DELETE_BACKWARD")
		{
			RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
			RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

			if (repass_input == pass_input)
			{
				p_panel_check_repass_input->Visible(false);
			}
			return true;
		}
	}
	return false;
}

void ChangePassLayer::OnShowChangePassLayout()
{
	p_layout_change_pass->GetParentWidget()->Visible(true);
	p_layout_change_pass->SetActionCommandWithName("ON_SHOW");


	OnCheckEnableBtnChangePass();

	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);
	static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
}

int  ChangePassLayer::IsValidPass(std::string str)
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

		if ((number_anscii >= 65 && number_anscii <= 90)
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

void ChangePassLayer::CheckInputPass(RKString str_pass, RKString str_username)
{
	int result = IsValidPass(str_pass.GetString());
	bool res_1 = str_pass.Contains(str_username);
	bool res_2 = str_username.Contains(str_pass);

	if ((result != 0 || res_1 || res_2) && str_pass != "")
	{
		p_panel_check_pass_input->Visible(true);
		auto panel = p_panel_check_pass_input->GetWidgetChildAtIdx(1);
		panel->Visible(true);

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

bool ChangePassLayer::IsValidInputPass()
{
	RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();
	RKString repass_input = static_cast<TextFieldWidget*>(p_textfield_repass_input)->GetText();
	RKString old_pass_input = static_cast<TextFieldWidget*>(p_textfield_oldpass_input)->GetText();

	bool register_pass_input_correct = !p_panel_check_pass_input->Visible();
	bool register_repass_input_correct = !p_panel_check_repass_input->Visible();

	if (register_pass_input_correct
		&& register_repass_input_correct
		&& (pass_input == repass_input)
		&& old_pass_input != ""
		&& pass_input != "")
		return true;

	return false;
}

void ChangePassLayer::OnCheckEnableBtnChangePass()
{
	if (IsValidInputPass())
	{
		static_cast<ui::Button*>(p_btn_changepass->GetResource())->setEnabled(true);
		p_btn_changepass->SetActionCommandWithName("SET_ENABLE");
	}
	else
	{
		static_cast<ui::Button*>(p_btn_changepass->GetResource())->setEnabled(false);
		p_btn_changepass->SetActionCommandWithName("SET_DISABLE");
	}
}
