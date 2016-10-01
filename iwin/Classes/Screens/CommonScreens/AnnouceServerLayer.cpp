#include "AnnouceServerLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#include "Screens/MainScreen.h"
AnnouceServerLayer::AnnouceServerLayer()
{
	p_annouce_server_layout = nullptr;
	p_annouce_is_running = false;
	p_have_annouce = false;
}

AnnouceServerLayer::~AnnouceServerLayer()
{
	p_annouce_server_layout = nullptr;
	p_announce_text = "";
}

void AnnouceServerLayer::InitAnnouceServer(BaseScreen * screen)
{
	_common_screen = screen;
	p_annouce_server_layout = _common_screen->GetWidgetChildByName("common_screen.layout_anoun_server");
}

int AnnouceServerLayer::Update(float dt)
{
	if (p_annouce_is_running)
	{
		if (ScrMgr->GetCurrentMenuUI() == MAIN_SCREEN)
		{
			p_annouce_server_layout->Visible(false);
		}
		else
		{
			p_annouce_server_layout->Visible(true);
		}
	}

	if (p_have_annouce)
	{
		p_have_annouce = false;
		p_annouce_server_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_annouce_server_layout->ForceFinishAction();
			auto w_text = this->p_annouce_server_layout->GetWidgetChildAtIdx(1);
			static_cast<TextWidget*>(w_text)->SetText(this->p_announce_text, true);
			w_text->SetActionCommandWithName("ON_SHOW");
			this->p_annouce_is_running = true;
		}));
		return 1;
	}

	if (p_annouce_server_layout->Visible())
	{
		auto w_text = p_annouce_server_layout->GetWidgetChildAtIdx(1);
		if (w_text->GetPosition_BottomRight().x < 0)
		{
			p_annouce_is_running = false;
			w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
				w_text->ForceFinishAction();
				this->p_annouce_server_layout->SetActionCommandWithName("ON_HIDE");
			}));
		}

		return 1;
	}

	return 1;
}

bool AnnouceServerLayer::ProcessWidgetAnnounce(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "close_annouce_panel")
		{
			if (p_annouce_is_running && p_annouce_server_layout->Visible())
			{
				p_annouce_is_running = false;
				auto w_text = p_annouce_server_layout->GetWidgetChildAtIdx(1);
				w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
					w_text->ForceFinishAction();
					this->p_annouce_server_layout->SetActionCommandWithName("ON_HIDE");
				}));
				static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->forceStopAnnouce(true);
			}
			return true;
		}
	}

	return false;
}

void AnnouceServerLayer::OnReceiveAnnouncementFromServer(RKString text)
{
	p_announce_text = text;
	p_have_annouce = true;
}

void AnnouceServerLayer::forceStopAnnouce(bool value)
{
	p_annouce_is_running = !value;
	p_annouce_server_layout->Visible(!value);
}