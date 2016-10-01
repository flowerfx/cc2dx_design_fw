#ifndef __SLIDER_WIDGET_H__
#define __SLIDER_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class SliderWidget : public WidgetEntity
		{
		private:

		public:
			SliderWidget();
			virtual ~SliderWidget();

			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInteractive(bool value)										override;
			virtual void SetSize(Vec2 s)												override;
			virtual Vec2 GetSize()														override;

			float GetPercent();
			void SetPercent(float percent);

			int GetPart();
			void SetPart(int value);
		};

	}
}

#endif //__SLIDER_WIDGET_H__

