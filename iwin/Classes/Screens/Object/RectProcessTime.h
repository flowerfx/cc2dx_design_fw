#ifndef __RECT_PROCESS_TIME__H__
#define __RECT_PROCESS_TIME__H__
#include "Screens/ScreenManager.h"

namespace Utility
{
	namespace UI_Widget
	{
		class WidgetEntity;
	}
}


class RectProcessTime
{
private:
	Utility::UI_Widget::WidgetEntity* _time_bar;
	Utility::UI_Widget::WidgetEntity* _particle;

	float _interval;
	float _curent_t;
public:
	RectProcessTime(Utility::UI_Widget::WidgetEntity* panel);
	~RectProcessTime();
	
	void update(float t);
	void startTime(float interval, std::function<void()> call_back =nullptr);
	void stop();
	bool isShowTimeAction();


};
#endif //__RECT_PROCESS_TIME__H__

