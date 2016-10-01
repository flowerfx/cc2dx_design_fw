
#include "cocos2d.h"
#include "TimeBarProcess.h"
#include "UI/AdvanceUI/WidgetEntity.h"
#include "UI/AdvanceUI/wLoadingBar.h"
#include "UI/AdvanceUI/wText.h"

TimeBarProcess::TimeBarProcess(WidgetEntity* p_panel)
{
	this->p_panel_time_bar = p_panel;
	p_time_bar = p_panel->GetWidgetChildByName(".progress_bar_value");
	p_percent_label = p_panel->GetWidgetChildByName(".title_percent_value");
	_curent_time = 0;
	_interval = 0;
	_custom_label = nullptr;
}

TimeBarProcess::~TimeBarProcess()
{
	p_panel_time_bar = nullptr;
	p_time_bar = nullptr;
	p_percent_label = nullptr;

	_custom_label = nullptr;
}
void TimeBarProcess::update(float t)
{
	if (!p_panel_time_bar->Visible())
		return;
	_curent_time -= t;
	if (_curent_time <= 0 || _interval <0)// het thoi gian
	{
		onStop(false);
		return;
	}
	if (p_time_bar) {
		float percent = _curent_time / _interval *100;
		if (percent < 0 || percent>100)
		{
			percent = 0;
		}
		setPercent(percent);
		
	}
}

void TimeBarProcess::onStartime(float interval, int remain)
{
	_curent_time = _interval = interval;
	float percent = 100;
	if (remain != 0)
	{
		percent = remain / interval * 100;
	}

	setPercent(_curent_time / _interval * 100);
	
}

void TimeBarProcess::onStop(bool isHide)
{
	_curent_time = 0;
	setVisible(!isHide);
}
void TimeBarProcess::setVisible(bool isShow)
{
	if (p_panel_time_bar)
		this->p_panel_time_bar->Visible(isShow);
}
bool TimeBarProcess::getVisible()
{
	return p_panel_time_bar->Visible();
}

void TimeBarProcess::setPercent(int percent)
{
	if (!p_time_bar) return;
	static_cast<LoadingBarWidget*>(p_time_bar)->SetPercent(percent);
	if (!_custom_label)
	{
		static_cast<TextWidget*>(p_percent_label)->SetText(
			std::to_string((int)(_interval * percent / 100)) + "/" + std::to_string((int)_interval)
		);
	}
	else
	{
		static_cast<TextWidget*>(p_percent_label)->SetText(_custom_label(_curent_time, _interval));
	}
	float value = _curent_time / _interval;
	p_time_bar->SetColor(Color3B(255 * (1 - value), 255 * value, 0));
}

void TimeBarProcess::setCustomLabel(std::function<std::string(int, int)> custom_label)
{
	_custom_label = custom_label;
}
