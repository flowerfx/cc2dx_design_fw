#include "wTimeDigitalWidget.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		TimeDigitalWidget::TimeDigitalWidget()
		{
			p_typeUI = UI_TYPE::UI_TIME_DIGITAL;
		
			p_time_target = 0.0f;
			p_current_time = 0.0f;
			p_already_to_zero = true;
			p_show_type = 1;
			_eventCallback = nullptr;
			MoveFromZero = false;
		}

		TimeDigitalWidget::~TimeDigitalWidget()
		{
			_eventCallback = nullptr;
		}
		
		void TimeDigitalWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TimeDigitalWidget*>(value);
			TextWidget::CopyValue(value);
			//
			p_time_target = val->p_time_target;
			p_current_time = val->p_current_time;
			MoveFromZero = val->MoveFromZero;
			p_already_to_zero = val->p_already_to_zero;
			p_show_type = val->p_show_type;
			_eventCallback = val->_eventCallback;
		}
		
		WidgetEntity * TimeDigitalWidget::Clone()
		{
			TimeDigitalWidget * p_clone = new TimeDigitalWidget();		
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->GetResource()->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void TimeDigitalWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void TimeDigitalWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			TextWidget::InitTexture(xml_value);

			MoveFromZero = (xml_value->GeneralValue->GetDataInt("move_from_zero") == 1);

		}

		bool TimeDigitalWidget::Update(float dt)
		{
			if (!p_Visible)
				return false;

			if (MoveFromZero)
			{
				p_current_time += dt;
			}
			else
			{
				p_current_time -= dt;
			}

			if (MoveFromZero ? (p_current_time >= p_time_target) : (p_current_time < 0))
			{

				if (MoveFromZero)
				{
					p_current_time = p_time_target;
				}
				else
				{
					p_current_time = 0;
				}

				if (!p_already_to_zero)
				{
					p_already_to_zero = true;

					if (_eventCallback)
					{
						_eventCallback(this, EventType_Digital::ON_TIMER_ZERO);
					}
				}
			}

			RKString time_str = "";

			int time_show = (int)p_current_time;

			int year = 0;
			int month = 0;
			int day = 0;
			int hour = 0;
			int min = 0;
			int sec = 0;

			if (p_show_type >= 5)
			{
				year = time_show / (60 * 60 * 24 * 365);
			}
			int sec_in_month  = time_show % (60 * 60 * 24 * 365);

			if (p_show_type == 4)
			{
				sec_in_month = time_show;
			}
			if (p_show_type >= 4)
			{
				month = sec_in_month / (60 * 60 * 24 * 30);
			}
			int sec_in_day = sec_in_month % (60 * 60 * 24 * 30);
			if (p_show_type == 3)
			{
				sec_in_day = time_show;
			}
			if (p_show_type >= 3)
			{
				day = sec_in_day / (60 * 60 * 24);
			}
			int sec_in_hour = sec_in_day % (60 * 60 * 24);
			if (p_show_type == 2)
			{
				sec_in_hour = time_show;
			}
			if (p_show_type >= 2)
			{
				hour = sec_in_hour / (60 * 60);
			}
			int sec_in_min = sec_in_hour % (60 * 60 );
			if (p_show_type == 1)
			{
				sec_in_min = time_show;
			}
			if (p_show_type >= 1)
			{
				min = sec_in_min / 60;
			}

			if (p_show_type == 0)
			{
				sec = time_show;
			}
			else
			{
				sec = sec_in_min % 60;
			}


			if (p_show_type >= 5)
			{
				if (year >= 10)
				{
					time_str += std::to_string(year);
				}
				else
				{
					time_str += ("0" + std::to_string(year));
				}
				time_str += ":";
			}

			if (p_show_type >= 4 )
			{
				if (month >= 10)
				{
					time_str += std::to_string(month);
				}
				else
				{
					time_str += ("0" + std::to_string(month));
				}
				time_str += ":";
			}
			if (p_show_type >= 3)
			{
				if (day >= 10)
				{
					time_str += std::to_string(day);
				}
				else
				{
					time_str += ("0" + std::to_string(day));
				}
				time_str += ":";
			}
			if (p_show_type >= 2)
			{
				if (hour >= 10)
				{
					time_str += std::to_string(hour);
				}
				else
				{
					time_str += ("0" + std::to_string(hour));
				}
				time_str += ":";
			}
			if (p_show_type >= 1)
			{
				if (min >= 10)
				{
					time_str += std::to_string(min);
				}
				else
				{
					time_str += ("0" + std::to_string(min));
				}
				time_str += ":";
			}
			if (p_show_type >= 0)
			{
				if (sec >= 10)
				{
					time_str += std::to_string(sec);
				}
				else
				{
					time_str += ("0" + std::to_string(sec));
				}
			}

			p_current_text = time_str;
			SetText();
			return true;
		}

		void TimeDigitalWidget::ResetTimer(long double time, long double current_time)
		{
			p_current_time = current_time;
			p_time_target = time;
			PASSERT2(p_time_target > 0 && p_current_time >= 0, "timer is not a valid value!");
			if (p_time_target <= 0)
			{
				p_time_target = 1;
			}
			if (p_current_time < 0)
			{
				p_current_time = p_time_target;
			}
			p_already_to_zero = false;
		}

		void TimeDigitalWidget::SetTimer(long double time, long double current_time, const wDigitalTimerCallBack& callback)
		{
			p_current_time = current_time;
			p_time_target = time;
			PASSERT2(p_time_target > 0 && p_current_time >= 0, "timer is not a valid value!");
			if (p_time_target <= 0)
			{
				p_time_target = 1;
			}
			if (p_current_time < 0)
			{
				p_current_time = 0;
			}
			Visible(true);
			p_already_to_zero = false;

			if (callback)
			{
				_eventCallback = callback;
			}
		}

		void TimeDigitalWidget::addEventListener(const wDigitalTimerCallBack& callback)
		{
			_eventCallback = callback;
		}

		void TimeDigitalWidget::StopTimeProcess()
		{
			p_already_to_zero = true;
			p_current_time = 0;
			Visible(false);
		}

	}
}