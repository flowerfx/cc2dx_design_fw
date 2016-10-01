#include "ConfirmCodeLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"

ConfirmCodeLayer::ConfirmCodeLayer()
{
	p_confirm_code_panel = nullptr;
}

ConfirmCodeLayer::~ConfirmCodeLayer()
{
	p_confirm_code_panel = nullptr;
}

void ConfirmCodeLayer::InitConfirmCode(BaseScreen * screen)
{
	_common_screen = screen;
	p_confirm_code_panel = _common_screen->GetWidgetChildByName("common_screen.confirm_layout.confirm_code");
}

bool ConfirmCodeLayer::ProcessWidgetConfirmBox(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_confirm_box" || name == "btn_cancel_confirm" || name == "btn_confirm")
		{
			p_confirm_code_panel->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_confirm_code_panel->ForceFinishAction();
				this->p_confirm_code_panel->GetParentWidget()->Visible(false);

				static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
				static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();

			}));

			if (name == "btn_confirm")
			{
				auto w_text = _widget->GetParentWidget()->GetWidgetChildByName(".layput_current_code.code_confirm_input");
				RKString confrim_code_txt = static_cast<TextFieldWidget*>(w_text)->GetText();
				GlobalService::sendConfirmSMSCode(confrim_code_txt.GetString());
			}

			return true;
		}
		else if (name == "btn_clear_code_confirm")
		{
			auto w_text = _widget->GetParentWidget()->GetWidgetChildByName(".code_confirm_input");
			static_cast<TextFieldWidget*>(w_text)->ClearText();
			_widget->Visible(false);

			return true;
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{

		if (name == "code_confirm_input_DETACH_IME")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(false);
			return true;
		}
		else if (name == "code_confirm_input_ATTACH_IME")
		{
			RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();

			if (string_input.Length() > 0)
			{
				_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			}
			return true;
		}
		else if (name == "code_confirm_input_INSERT_TEXT")
		{

			_widget->GetParentWidget()->GetWidgetChildAtIdx(1)->Visible(true);
			return true;
		}

	}
	return false;
}

void ConfirmCodeLayer::OnShowConfirmCode()
{
	p_confirm_code_panel->GetParentWidget()->Visible(true);
	p_confirm_code_panel->SetActionCommandWithName("ON_SHOW");

	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);
	static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
}