#include "InviteLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"

InviteLayer::InviteLayer()
{
	p_panel_invite = nullptr;
	p_check_deny_invite_layout = nullptr;
	p_text_message_invite = nullptr;
	avatar_user_invite_layout = nullptr;
}

InviteLayer::~InviteLayer()
{

}

bool InviteLayer::ProcessWidgetInvitePanel(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_agree_invite")
		{
			p_panel_invite->GetParentWidget()->Visible(false);
			static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
			static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();

			GlobalService::joinBoard(_roomId, _boardId, _password.GetString());
			return true;
		}
		else if (name == "btn_deny_invite")
		{
			OnHideInvite(CallFunc::create([this]() {
				this->p_panel_invite->ForceFinishAction();
				this->p_panel_invite->GetParentWidget()->Visible(false);
			}));
			return true;
		}
		else if (name == "btn_close_invite")
		{
			OnHideInvite(CallFunc::create([this]() {
				this->p_panel_invite->ForceFinishAction();
				this->p_panel_invite->GetParentWidget()->Visible(false);
			}));
			return true;
		}
		else if (name == "btn_check_invite_friend")
		{
			if (p_check_deny_invite_layout->GetWidgetChildByName(".on_check")->Visible())
			{
				GlobalService::sendStaticInviviteToPlayGame(false);
				p_check_deny_invite_layout->GetWidgetChildByName(".on_check")->Visible(false);
				p_check_deny_invite_layout->GetWidgetChildByName(".off_check")->Visible(true);
			}
			else
			{
				GlobalService::sendStaticInviviteToPlayGame(true);
				p_check_deny_invite_layout->GetWidgetChildByName(".on_check")->Visible(true);
				p_check_deny_invite_layout->GetWidgetChildByName(".off_check")->Visible(false);
			}
			return true;
		}
	}

	return false;
}

void InviteLayer::OnInitInviteBanner(BaseScreen * screen)
{
	_common_screen = screen;
	_gameId = 0;
	_roomId = 0;
	_boardId = 0;
	_password = "";

	p_panel_invite = _common_screen->GetWidgetChildByName("common_screen.banner_invite_layout.layout_invite");
	p_check_deny_invite_layout = p_panel_invite->GetWidgetChildByName(".check_deny_layout");
	p_text_message_invite = p_panel_invite->GetWidgetChildByName(".title_contain");
	avatar_user_invite_layout = p_panel_invite->GetWidgetChildByName(".panel_avatar");
}

void InviteLayer::OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar)
{
	_gameId = gameid;
	_roomId = roomid;
	_boardId = boardid;
	_password = pass;

	static_cast<TextWidget*>(p_text_message_invite)->SetText(message);

	GameMgr->HandleAvatarDownload(
		idavatar, "avatar_user_invite", [this](void * data, void * str, int tag)
	{
		_common_screen->ParseImgToPanel(this->avatar_user_invite_layout, data, str, tag);
	});

	p_panel_invite->GetParentWidget()->Visible(true);
	p_panel_invite->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
		this->p_panel_invite->ForceFinishAction();
		if (p_panel_invite->GetPosition().y != 0)
		{
			auto cur_pos = p_panel_invite->GetPosition();
			p_panel_invite->SetPosition(Vec2(cur_pos.x, 0));
		}
	}));

	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(true);
	static_cast<CommonScreen*>(_common_screen)->OnSetScreenComeFrom(false);
}

void InviteLayer::OnHideInvite(CallFunc * call_back)
{
	p_panel_invite->SetActionCommandWithName("ON_HIDE", call_back);

	static_cast<CommonScreen*>(_common_screen)->SetHaveLayoutInteractive(false);
	static_cast<CommonScreen*>(_common_screen)->OnScreenComeFromEnable();
}
