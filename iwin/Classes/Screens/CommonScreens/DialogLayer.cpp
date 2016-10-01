#include "DialogLayer.h"

#include "UI/AdvanceUI/wText.h"

DialogLayer::DialogLayer()
{
	p_dialog_layout = nullptr;
	p_Function0 = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;

	p_nameBtnProcess = "";
	p_current_dialog = TYPE_DIALOG::DIALOG_NONE;

	p_panel_1_btn = nullptr;
	p_panel_2_btn = nullptr;
	p_panel_0_btn = nullptr;
	p_panel_fb_btn = nullptr;

	p_time_show_button = -1;
}

DialogLayer::~DialogLayer()
{

}

int DialogLayer::Update(float dt)
{
	if (p_dialog_layout->GetParentWidget()->Visible())
	{
		if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON || p_current_dialog == TYPE_DIALOG::DIALOG_ZERO_BUTTON_WAIT)
		{
			if (p_time_show_button > -1.f)
			{
				p_time_show_button -= dt;
				if (p_time_show_button <= 0.f)
				{
					if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON)
					{
						p_panel_1_btn->Visible(true);
					}
					else
					{
						OnHideDialog();
					}
					p_time_show_button = -1.f;
				}
			}
		}
		return 1;
	}
	return 0;
}

bool DialogLayer::ProcessWidgetDialog(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && p_dialog_layout->GetParentWidget()->Visible())
	{
		p_nameBtnProcess = name;
		OnHideDialog();
		return true;
	}
	return false;
}

void DialogLayer::OnShowDialog(RKString str_title,
	RKString str_message,
	TYPE_DIALOG type,
	const char * strCB,
	const std::function<void(const char *, const char*)> & func,
	const std::function<void(const char *, const char*)> & func1,
	const std::function<void(const char *, const char*)> & func2,
	RKString title_btn_0,
	RKString title_btn_1
)
{

	p_current_dialog = type;
	p_time_show_button = -1.f;

	static_cast<TextWidget*>(p_text_title)->SetText(str_title);
	static_cast<TextWidget*>(p_text_content)->SetText(str_message);

	if (p_current_dialog == TYPE_DIALOG::DIALOG_ONE_BUTTON)
	{
		p_panel_1_btn->Visible(true);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_TWO_BUTTON)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(true);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_ZERO_BUTTON || p_current_dialog == DIALOG_ZERO_BUTTON_WAIT)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(true);
		p_panel_fb_btn->Visible(false);
		if (p_current_dialog == DIALOG_ZERO_BUTTON_WAIT)
		{
			p_time_show_button = 2.f;
		}
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_FACEBOOK)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(true);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
		p_time_show_button = 3.0f;
	}


	p_Function0 = func;
	p_Function1 = func1;
	p_Function2 = func2;

	if (title_btn_0 != "")
	{
		static_cast<TextWidget*>(p_panel_1_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_1_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
	}

	if (title_btn_1 != "")
	{
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1, true);
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1, true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
	}

	AligneSizePanel();

	if (!p_dialog_layout->GetParentWidget()->Visible())
	{
		_common_screen->SetInteractiveInsteadOf(false, p_dialog_layout);

		cocos2d::Size size_target = p_dialog_layout->GetResource()->getContentSize();

		xml::BasicDec xml_value;
		xml_value.InsertDataVector2("SIZE", size_target * 1.2f);
		p_dialog_layout->SetHookCommand("ON_SHOW", "act0", 0, 0, xml_value, "SIZE");
		xml::BasicDec xml_value1;
		xml_value1.InsertDataVector2("SIZE", size_target);
		p_dialog_layout->SetHookCommand("ON_SHOW", "act0", 0, 1, xml_value1, "SIZE");

		p_dialog_layout->GetParentWidget()->Visible(true);
		p_dialog_layout->SetActionCommandWithName("ON_SHOW");

		p_dialog_layout->ClearHookCommand("ON_SHOW", "act0", 0, 0);
		p_dialog_layout->ClearHookCommand("ON_SHOW", "act0", 0, 1);
	}


	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true, 1);
	static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false , false);

}

void DialogLayer::OnProcessBtnFunc()
{
	if (p_current_dialog == DIALOG_ONE_BUTTON ||
		((p_current_dialog == DIALOG_WAITING_BUTTON || p_current_dialog == DIALOG_ZERO_BUTTON_WAIT) && p_nameBtnProcess == "") ||
		p_current_dialog == DIALOG_ZERO_BUTTON ||
		p_nameBtnProcess == "ok_2_btn" ||
		p_nameBtnProcess == "ok_fb_btn"
		)
	{
		if (p_Function0 != nullptr)
		{
			p_Function0("", p_nameBtnProcess.GetString());
		}
	}
	else if (p_nameBtnProcess == "cancel_fb_btn" || p_nameBtnProcess == "cancel_2_btn")
	{
		if (p_Function1 != nullptr)
		{
			p_Function1("", p_nameBtnProcess.GetString());
		}
	}

	p_Function0 = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;
	p_nameBtnProcess = "";


	_common_screen->SetInteractiveInsteadOf(true, p_dialog_layout);
	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false, 1);
	static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();
}

void DialogLayer::OnHideDialog()
{
	if (p_dialog_layout->GetParentWidget()->Visible())
	{
		p_dialog_layout->ForceFinishAction();
		p_dialog_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			this->p_dialog_layout->ForceFinishAction();
			this->p_dialog_layout->GetParentWidget()->Visible(false);
			this->OnProcessBtnFunc();
		}));
	}
}

void DialogLayer::InitDialogLayout(BaseScreen * screen)
{
	_common_screen = screen;
	p_dialog_layout = _common_screen->GetWidgetChildByName("common_screen.dialog_screen.panel_contain");
	p_panel_1_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_1_btn");
	p_panel_2_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_2_btn");
	p_panel_0_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_0_btn");
	p_panel_fb_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_fb_btn");

	p_text_title = p_dialog_layout->GetWidgetChildByName(".layout_.title_notice");
	p_text_content = p_dialog_layout->GetWidgetChildByName(".layout_.notice_content");

}

void DialogLayer::AligneSizePanel()
{
	auto _w_msg = static_cast<TextWidget*>(p_text_content);
	RKString current_string = _w_msg->GetText();
	int leng_txt = current_string.LengthUTF();
	float font_size = static_cast<TextWidget*>(_w_msg)->GetFontSize();
	Vec2 delta_scale = Vec2(GetGameSize().width / _common_screen->GetDesignSize().x, GetGameSize().height / _common_screen->GetDesignSize().y);
	WidgetEntity * hlbg = p_dialog_layout->GetWidgetChildByName(".layout_.hl_panel_");
	float actual_size = font_size * leng_txt;
	if (actual_size > _w_msg->GetTextAreaSize().x * 1.2f)
	{
		int delta_size = 10 * delta_scale.x;
		float target_x = 650 * delta_scale.x;
		float target_y = 0;

		cocos2d::Size current_size_text = _w_msg->GetResource()->getContentSize();
		Vec2 size_layout_btn = p_panel_1_btn->GetParentWidget()->GetSize();
		target_y = size_layout_btn.y + delta_size + current_size_text.height + 2 * delta_size;

		if (target_y < 200.f * delta_scale.x)
		{
			target_y = 200.f * delta_scale.x;
		}
		else if (target_y > 400.f * delta_scale.x)
		{
			p_text_content->Visible(false);
			WidgetEntity * scroll_text = p_dialog_layout->GetWidgetChildByName(".layout_.scroll_content");
			scroll_text->Visible(true);
			WidgetEntity * text_contain = scroll_text->GetWidgetChildAtIdx(0);
			static_cast<TextWidget*>(text_contain)->SetText(current_string, true);
			scroll_text->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
			static_cast<ScrollView*>(scroll_text->GetResource())->setInnerContainerSize(cocos2d::Size(target_x, current_size_text.height));
			text_contain->SetPosition(Vec2(target_x / 2, current_size_text.height));
			target_y = 400.f * delta_scale.x;
		}

		p_dialog_layout->GetResource()->setContentSize(cocos2d::Size(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetSize(Vec2(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetPosition(Vec2(target_x / 2, 0));

		auto widget_panel_btn = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(5);
		widget_panel_btn->SetPosition(Vec2(target_x / 2, size_layout_btn.y));
		_w_msg->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
        _w_msg->SetTextAreaSize(Vec2(target_x * 3.f / 4.f , 0));
		p_text_title->SetPosition(Vec2(target_x / 2, target_y - (1.5f * delta_size)));
		hlbg->SetSize(Vec2(target_x / 1.25f, target_y / 1.25f));
		hlbg->SetPosition(Vec2(target_x / 2, 0));
	}
	else
	{
		int delta_size = 10 * delta_scale.x;
		float target_x = 500 * delta_scale.x;
		float target_y = 0;

		cocos2d::Size current_size_text = _w_msg->GetResource()->getContentSize();
		Vec2 size_layout_btn = p_panel_1_btn->GetParentWidget()->GetSize();
		target_y = size_layout_btn.y + delta_size + current_size_text.height + 2 * delta_size;

		if (target_y < 200.f * delta_scale.x)
		{
			target_y = 200.f * delta_scale.x;
		}
		else if (target_y > 400.f * delta_scale.x)
		{
			p_text_content->Visible(false);
			WidgetEntity * scroll_text = p_dialog_layout->GetWidgetChildByName(".layout_.scroll_content");
			scroll_text->Visible(true);
			WidgetEntity * text_contain = scroll_text->GetWidgetChildAtIdx(0);
			static_cast<TextWidget*>(text_contain)->SetText(current_string, true);
			scroll_text->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
			static_cast<ScrollView*>(scroll_text->GetResource())->setInnerContainerSize(cocos2d::Size(target_x, current_size_text.height));
			text_contain->SetPosition(Vec2(target_x / 2, current_size_text.height));
			target_y = 400.f * delta_scale.x;
		}

		p_dialog_layout->GetResource()->setContentSize(cocos2d::Size(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetSize(Vec2(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetPosition(Vec2(target_x / 2, 0));
		auto widget_panel_btn = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(5);
		widget_panel_btn->SetPosition(Vec2(target_x / 2, size_layout_btn.y));
		_w_msg->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
        _w_msg->SetTextAreaSize(Vec2(target_x * 3.f / 4.f , 0));
		p_text_title->SetPosition(Vec2(target_x / 2, target_y - (1.5f * delta_size)));
		hlbg->SetSize(Vec2(target_x / 1.25f, target_y / 1.25f));
		hlbg->SetPosition(Vec2(target_x / 2, 0));
	}
}