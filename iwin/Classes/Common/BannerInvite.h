#ifndef __BANNER_INVITE_H__
#define __BANNER_INVITE_H__ 

#include "UI/AdvanceUI/WidgetEntity.h"
#include <functional>

namespace Utility {
	namespace UI_Widget {
		class ScrollViewWidget;
		class WidgetEntity;
	}
}
using namespace Utility::UI_Widget;

class BannerInvite 
{

private:
	WidgetEntity*					p_panel_invite;

	int					_gameId;
	int					_roomId;
	int					_boardId;
	std::string			_password;
	int					_level;
	int					_idInvite;
	std::string			_username;
	int					_idAvatar;

	bool				_isShowing;

public:
	BannerInvite();
	virtual ~BannerInvite();

	void init(WidgetEntity* entity);
	void setData(int gameId, int roomId, int boardId, std::string password, std::string message, int level, int idInvite, std::string username, int idAvatar);
	void show();
	void hide();
	void handleAgreeBtn();
};

#endif // __IWIN_LIST_VIEW_H__