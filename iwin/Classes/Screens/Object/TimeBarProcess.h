#ifndef __TIME_BAR_PROCESS__H__
#define __TIME_BAR_PROCESS__H__
#include "Screens/ScreenManager.h"

namespace Utility
{
	namespace UI_Widget
	{
		class WidgetEntity;
	}
}
class TimeBarProcess
{
private:
	Utility::UI_Widget::WidgetEntity* p_panel_time_bar;
	Utility::UI_Widget::WidgetEntity* p_time_bar;
	Utility::UI_Widget::WidgetEntity* p_percent_label;

	float _interval;
	float _remain;
	float _curent_time;

	std::function<std::string(int, int)> _custom_label;
	void setPercent(int percent);
public:
	TimeBarProcess(Utility::UI_Widget::WidgetEntity* p_panel);
	~TimeBarProcess();

	void update(float t);
	void setCustomLabel(std::function<std::string(int, int)> custom_label);
	void onStartime(float interval, int remain);
	void onStop(bool isHide = true);

	void setVisible(bool isShow);
	bool getVisible();
	
};
#endif //__TIME_BAR_PROCESS__H__

