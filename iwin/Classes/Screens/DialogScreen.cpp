#include "DialogScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
DialogScreen::DialogScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_Function = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;
	p_strCallback = "";

	p_nameBtnProcess = "";

	p_current_dialog = TYPE_DIALOG::DIALOG_NONE;

	p_panel_1_btn = nullptr;
	p_panel_2_btn = nullptr;
	p_panel_0_btn = nullptr;
	p_panel_fb_btn = nullptr;
	p_time_show_button = -1;
}


DialogScreen::~DialogScreen()
{
	p_Function = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;

	p_strCallback = "";

	p_panel_1_btn = nullptr;
	p_panel_2_btn = nullptr;
	p_panel_0_btn = nullptr;
	p_panel_fb_btn = nullptr;
	p_time_show_button = -1;
}

int DialogScreen::Init()
{
	RKString _menu = "dialog_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	p_panel_contain = GetWidgetChildByName("dialog_screen.panel_contain");
	p_panel_1_btn = p_panel_contain->GetWidgetChildByName(".layout_btn.panel_1_btn");
	p_panel_2_btn = p_panel_contain->GetWidgetChildByName(".layout_btn.panel_2_btn");
	p_panel_0_btn = p_panel_contain->GetWidgetChildByName(".layout_btn.panel_0_btn");
	p_panel_fb_btn = p_panel_contain->GetWidgetChildByName(".layout_btn.panel_fb_btn");

	p_text_title = p_panel_contain->GetWidgetChildByName(".title_notice");
	p_text_content = p_panel_contain->GetWidgetChildByName(".notice_content");
	return 1;
}

int DialogScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

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
					OnProcessWidgetChild("", UI_TYPE::UI_BUTTON, nullptr);
				}
				p_time_show_button = -1.f;
			}
		}
	}

	return 1;
}

void DialogScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void DialogScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void DialogScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (UI_TYPE::UI_BUTTON)
	{
		OnHide();
		p_nameBtnProcess = name;
	}
}

void DialogScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(DIALOG_SCREEN);

	if (p_current_dialog == DIALOG_ONE_BUTTON || 
		((p_current_dialog == DIALOG_WAITING_BUTTON || p_current_dialog == DIALOG_ZERO_BUTTON_WAIT) && p_nameBtnProcess == "") ||
		p_current_dialog == DIALOG_ZERO_BUTTON ||
			p_nameBtnProcess == "ok_2_btn" || 
			p_nameBtnProcess == "ok_fb_btn"	
		)
	{
		callFunction();
	}
	else if (p_nameBtnProcess == "cancel_fb_btn" || p_nameBtnProcess == "cancel_2_btn")
	{
		if (p_Function1 != nullptr)
		{
			p_Function1(p_strCallback.GetString(), p_nameBtnProcess.GetString());
		}
	}


	p_Function = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;
	p_nameBtnProcess = "";
	p_menu_show_next = MENU_NONE;

	GameMgr->ResetMessageStage();
}


void DialogScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}

//void functionCallBack
void DialogScreen::callFunction()
{
	/* do something */
	if (p_Function == nullptr)
		return;

	p_Function(p_strCallback.GetString(), p_nameBtnProcess.GetString());

	p_strCallback = "";

}


void DialogScreen::AligneSizePanel()
{
	auto _w_msg = static_cast<TextWidget*>(p_text_content);
	RKString current_string = _w_msg->GetText();
	int leng_txt = current_string.LengthUTF();
	float font_size = static_cast<Text*>(_w_msg->GetResource())->getFontSize();

	float actual_size = font_size * leng_txt;
	if (actual_size > _w_msg->GetTextAreaSize().x * 1.2f)
	{
		int delta_size = 10 * GetGameSize().width / this->GetDesignSize().x;

		float target_x = 650 * GetGameSize().width / this->GetDesignSize().x;
		float target_y = 0;

		cocos2d::Size current_size_text = _w_msg->GetResource()->getContentSize();
		Vec2 size_layout_btn = p_panel_1_btn->GetParentWidget()->GetSize();
		target_y = size_layout_btn.y + delta_size + current_size_text.height + 2 * delta_size;

		if (target_y < 200 * GetGameSize().width / this->GetDesignSize().x)
		{
			target_y = 200 * GetGameSize().width / this->GetDesignSize().x;
		}

		p_panel_contain->GetResource()->setContentSize(cocos2d::Size(target_x, target_y));
		p_panel_contain->GetWidgetChildAtIdx(0)->SetSize(Vec2(target_x, target_y));
		p_panel_contain->GetWidgetChildAtIdx(0)->SetPosition(	Vec2(target_x / 2, 0));

		p_panel_1_btn->GetParentWidget()->SetPosition(			Vec2(target_x / 2, size_layout_btn.y));
		_w_msg->SetPosition(									Vec2(target_x / 2, size_layout_btn.y + delta_size));
		static_cast<TextWidget*>(p_text_title)->SetPosition(	Vec2(target_x / 2, target_y - 2 * delta_size));
	}
}

//call popup method
void DialogScreen::OnShowDialog(RKString str_title,
	RKString str_message,
	TYPE_DIALOG type,
	const char * strCB,
	const std::function<void(const char *, const char* )> & func,
	const std::function<void(const char *, const char* )> & func1,
	const std::function<void(const char *, const char* )> & func2 ,
	RKString title_btn_0 , 
	RKString title_btn_1 
)
{
	if (ScrMgr->GetCurrentMenuUI() != DIALOG_SCREEN)
	{
		OnFadeIn();
	}
	else
	{
		for (size_t i = 0; i < p_ListStatus.size(); i++)
		{
			//if (p_ListStatus.at(i)->p_IsActive)
			{
				SetStatusAtIdx(STATUS_MENU::M_IDLE, i);
				if (i >= (size_t)(p_ListStatus.size() > 1 ? 1 : 0))
				{
					OnSetActionFadeAllResource(GetCurrentStatus(i), i);
				}
			}
		}
		this->RemoveActionFadeAllChild(0);
		this->RemoveActionFadeAllChild(1);
		this->SetLayerInteractive(true);
		p_IdxUITagAppear = 0;
		p_DeltaMoveTime = 0.0;

	}

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
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_ZERO_BUTTON || p_current_dialog==DIALOG_ZERO_BUTTON_WAIT)
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

	p_strCallback = strCB;
	p_Function = func;
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
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1 , true);
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1, true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
	}
	AligneSizePanel();
}


void DialogScreen::push_layer_to_main_scene(int order)
{
	order = 101;
	Screen::push_layer_to_main_scene(order);
}