#ifndef __CHECK_BOX_WIDGET_H__
#define __CHECK_BOX_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class CheckBoxWidget : public WidgetEntity
		{
		private:


		public:
			CheckBoxWidget();
			virtual ~CheckBoxWidget();
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInteractive(bool value)										override;
			virtual void ResizeAndPosChildWidget(WidgetEntity * child)					override;

			void OnSetSelected(bool selected);
		};

	}
}

#endif //__CHECK_BOX_WIDGET_H__

