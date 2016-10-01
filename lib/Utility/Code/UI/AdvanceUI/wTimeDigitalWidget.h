#ifndef __TIME_DIGITAL_WIDGET_H__
#define __TIME_DIGITAL_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
#include "wText.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		enum class EventType_Digital
		{
			ON_TIMER_ZERO
		};
		class TimeDigitalWidget : public TextWidget
		{
		private:
			long double p_time_target;
			long double p_current_time;
			bool MoveFromZero;
			bool p_already_to_zero;
			int p_show_type; //0 is sec, 1 is minute , 2 is hour, 3 is day , 4 is month, 5 is year
		public:
			enum EventType
			{
				ON_TIMER_ZERO
			};

			typedef std::function<void(Ref*, EventType_Digital)> wDigitalTimerCallBack;

			TimeDigitalWidget();
			virtual ~TimeDigitalWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)		override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)						override;
			virtual WidgetEntity * Clone()													override;
			virtual void CopyValue(WidgetEntity * value)									override;
			virtual bool Update(float dt)													override;
			//current in sec
			void SetTimer(long double time , long double current_time , const wDigitalTimerCallBack& callback = nullptr);
			//current in sec
			void ResetTimer(long double time, long double current_time);

			void addEventListener(const wDigitalTimerCallBack& callback);
			void StopTimeProcess();
		private:
			wDigitalTimerCallBack  _eventCallback;
		};

	}
}

#endif //__TIME_DIGITAL_WIDGET_H__

