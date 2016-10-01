#ifndef __BUTTON_WIDGET_H__
#define __BUTTON_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class ButtonWidget : public WidgetEntity
		{
		private:
			bool p_is_only_text;
			Color3B p_color_text;
			int		p_opacity_text;
			RKString p_title_text;
		public:
			ButtonWidget();
			virtual ~ButtonWidget();

			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInteractive(bool value)										override;
			virtual void ResizeAndPosChildWidget(WidgetEntity * child)					override;
			virtual void SetColor(Color3B p)											override;
			virtual void SetSize(Vec2 s)												override;
			virtual void ReloadText()													override;

			void SetTitle(RKString title);
			void SetCallBack(const ui::Widget::ccWidgetTouchCallback& callback);
		};

	}
}

#endif //__BUTTON_ENTITY_H__

