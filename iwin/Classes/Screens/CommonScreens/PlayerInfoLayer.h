#ifndef __PLAYER_INFO_LAYER_H__
#define __PLAYER_INFO_LAYER_H__

#include "Screens/ScreenManager.h"

using namespace Utility;
using namespace Utility::UI_Widget;
using namespace iwinmesage;
class Achievement;
class PlayerInfoLayer
{
public:
	PlayerInfoLayer();
	virtual ~PlayerInfoLayer();
private:
	BaseScreen * _common_screen;
	/*
	use : show player info
	*/
	WidgetEntity *	p_layout_player_info;
	WidgetEntity *	p_tab_info_player;
	WidgetEntity *	p_list_detail_achieve;
	WidgetEntity *	p_tab_achieve_player;

	std::vector<Achievement*> p_list_achieve;
protected:
	/*
	use : show player info
	*/
	void OnParseAchievementIntoGame(WidgetEntity* widget, Achievement * achie, int idx);
public:
	/*
	use : show player info
	*/
	void InitInfoPlayer(BaseScreen * screen);
	bool IsPlayerInfoVisible();
	bool ProcessWidgetShowPlayerInfo(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void OnShowPlayerInfo(Player * player);
	void OnSetUserPlayer(User * user);
	void OnReceiveAchievement(int id, void * list_achie);

};

#endif //__PLAYER_INFO_LAYER_H__

