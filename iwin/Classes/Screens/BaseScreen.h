#ifndef __MENU_ENTITY_WRAPPER_H__
#define __MENU_ENTITY_WRAPPER_H__

#include "UI/AdvanceUI/Screen.h"
#include "Common/Common.h"
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
namespace RKUtils
{
	class ThreadWrapper;
}
enum TYPE_DIALOG
{
	DIALOG_NONE = 0,
	DIALOG_ONE_BUTTON,
	DIALOG_TWO_BUTTON,
	DIALOG_ZERO_BUTTON,
	DIALOG_FACEBOOK,
	DIALOG_WAITING_BUTTON,
	DIALOG_ZERO_BUTTON_WAIT,
	DIALOG_COUNT
};

enum MENU_LAYER
{
	MENU_NONE = 0,
	LOADING_SCREEN,
	DIALOG_SCREEN,
	LOGIN_SCREEN,
	MAIN_SCREEN,
	PROFILE_SCREEN,
	LOBBY_SCREEN,
	LEADERBOARD_SCREEN,
	EVENT_SCREEN,
	FRIEND_SCREEN,
	IAP_SCREEN,
	HUD_SCREEN,
	NEW_LOBBY_SCREEN,
	CHANGE_AWARD_SCREEN,
	COMMON_SCREEN,
	MENU_COUNT
};

class BaseScreen : public Screen
{
protected:

	MENU_LAYER p_menu_come_from;
    
    MENU_LAYER p_menu_show_next;

	int		p_current_idx_add_ ;
	float	p_time_add_ ;
	float   p_origin_time_add_;
	int		p_size_object_need_to_add_;

	bool	p_have_info_change;

	ThreadWrapper *		  p_thread_init_component;
	bool   p_init_component_complete;
public:
	BaseScreen();
	virtual ~BaseScreen();

	virtual  int	Init();
	virtual  int	InitComponent();
	virtual void	CallInitComponent(bool use_thread);
	virtual  int	Update(float dt) override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags);
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags);

	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget) = 0;
	virtual void OnDeactiveCurrentMenu();
	virtual void OnBeginFadeIn();
	virtual void OnFadeInComplete();

	virtual void ParseUserUI();
	virtual void onMyInfoChanged();
	virtual void ParseImgToPanel(WidgetEntity * panel, void * data, void * url , int id );
	virtual void ParseSpriteToPanel(WidgetEntity * panel, Sprite * sp, int z_order);
	virtual void OnReceiveFriendList();

	void SetMenuComFrom(MENU_LAYER menu){ p_menu_come_from = menu; }
	MENU_LAYER GetMenuComeFrom() { return p_menu_come_from; }
    
    void SetMenuShowNext(MENU_LAYER menu) {p_menu_show_next = menu; }
    MENU_LAYER GetMenuShowNext() { return p_menu_show_next; }

	void OnCallBackProcess(RKString str);
	virtual void OnProcess(RKString str);

	RKString NormalValue(s64 value);
	RKString NorValue(s64 value);

	virtual void CheckTouchOutSideWidget(cocos2d::Point p, WidgetEntity * panel, const std::function<void(void)> & func);
	virtual void SetInteractiveInsteadOf(bool value, WidgetEntity * exp);
protected:
	virtual void ParseList();
	virtual void LoadListAsTime(float dt);

	virtual void OnHandleInfoChange();
};

#endif //__MENU_ENTITY_WRAPPER_H__

