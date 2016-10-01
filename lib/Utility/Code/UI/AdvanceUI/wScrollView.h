#ifndef __SCROLL_WIDGET_H__
#define __SCROLL_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class ScrollViewWidget : public WidgetEntity
		{
		private:
			

		public:
			ScrollViewWidget();
			virtual ~ScrollViewWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetSizeValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void SetInteractive(bool value)										override;
		};

	}
}

#endif //__SCROLL_WIDGET_H__

