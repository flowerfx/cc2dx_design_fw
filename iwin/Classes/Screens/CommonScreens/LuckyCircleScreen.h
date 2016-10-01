#ifndef __LUCKY_CIRCLE_SCREEN_H__
#define __LUCKY_CIRCLE_SCREEN_H__
#include "Screens/ScreenManager.h"
using namespace Utility;
using namespace Utility::UI_Widget;

namespace iwinmesage
{
	class LuckyCircleResultList;
	class LuckyCircleQuestList;
}
using namespace iwinmesage;

class LuckyCircleScreen
{

private:
	BaseScreen * _common_screen;

	LuckyCircleQuestList*  p_lucky_quest_list;
	LuckyCircleResultList*  p_lucky_result;

	bool isRotation;
	int index_result;

	WidgetEntity* p_layout_main;
	WidgetEntity* p_arow_tick;
	WidgetEntity* p_list_view_result;
	WidgetEntity* p_btn_money_win;
	WidgetEntity* p_btn_money_rubby;
	
	WidgetEntity* p_popup_result;
	WidgetEntity* p_lb_money_win;
	WidgetEntity* p_lb_money_start;

	WidgetEntity*  p_img_circle;
	WidgetEntity* p_check_auto;
	WidgetEntity* p_anim_money;

	void RemoveImageFinish();
	void ShowResult();
public:

	LuckyCircleScreen();
	virtual ~LuckyCircleScreen();

	int  InitLuckyCircle(BaseScreen * screen);
	bool IsVisible();
	void OnProcessChildTouch(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);
	void SetVisible(bool isShow);
	int Update(float dt);
	void OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list);
	void OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item);
	void ActionRotation(float angle);
	void OnRotationDone();
	void UpdateMoney();

};

#endif 

