#ifndef __TIME_PROCESS_WIDGET_H__
#define __TIME_PROCESS_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		enum class EventType_Process
		{
			ON_TIMER_ZERO
		};
		class TimeProcessWidget : public WidgetEntity
		{
		private:
			cocos2d::ProgressTimer * p_ObjectTimer;
			float p_timer;
			float p_current_timer;
			bool  p_already_to_zero;
			
			float previous_percent;
			bool  MoveFromZero;

			xml::UILayerWidgetDec * p_xml_value;

			cocos2d::ProgressTimer * CreateTimer(xml::UILayerWidgetDec * xml_value);
		public:

			typedef std::function<void(Ref*, EventType_Process)> wProgressTimerCallBack;


			virtual cocos2d::Node * GetResource()									override;
			virtual bool SetNewResource(cocos2d::Node * resource)					override;
			
			void  SetPercent(float per);
			TimeProcessWidget();
			virtual ~TimeProcessWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual bool Update(float dt)												override;

			void SetTimer(float time , float current_time , const wProgressTimerCallBack& callback = nullptr);
			void addEventListener(const wProgressTimerCallBack& callback);
			void StopTimeProcess();

			float GetCurrentTimer();
		private:
			wProgressTimerCallBack  _eventCallback;
		};

	}
}

#endif //__TIME_PROCESS_WIDGET_H__

