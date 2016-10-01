#ifndef __INVITE_LAYER_H__
#define __INVITE_LAYER_H__

#include "Screens/ScreenManager.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class InviteLayer
{
public:
	InviteLayer();
	virtual ~InviteLayer();
private:
	BaseScreen * _common_screen;
	/*
	use : invite panel here
	*/
	int					_gameId;
	int					_roomId;
	int					_boardId;
	RKString			_password;


	WidgetEntity *     p_panel_invite;
	WidgetEntity *     p_check_deny_invite_layout;
	WidgetEntity *     p_text_message_invite;
	WidgetEntity *     avatar_user_invite_layout;

protected:
	void OnHideInvite(CallFunc * call_back);
public:
	bool ProcessWidgetInvitePanel(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar);
	void OnInitInviteBanner(BaseScreen * screen);
};

#endif //__INVITE_LAYER_H__

