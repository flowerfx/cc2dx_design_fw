#ifndef __RADIO_BTN_WIDGET_H__
#define __RADIO_BTN_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class RadioBtnWidget : public WidgetEntity
		{
		private:

			RKString	p_on_state;
			RKString	p_off_state;
			float		p_distance;
			int			p_direction;
			float		p_scale;
		protected:
			void create_btn_radio();
		public:
			RadioBtnWidget();
			virtual ~RadioBtnWidget();

			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInteractive(bool value)										override;
			virtual void SetSize(Vec2 size)												override;

			void AddRadioBtn();
			void RemoveRadioBtn();
			void SetNumberRadioBtn(int number);

			void MoveToNextBtn();
			void MovePreviousBtn();
			void MoveToBtnIdx(int idx);
		};

	}
}

#endif //__RADIO_BTN_WIDGET_H__

