#ifndef __LAYOUT_WIDGET_H__
#define __LAYOUT_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class LayoutWidget : public WidgetEntity
		{
		private:
			cocos2d::EventListenerFocus* _eventListener;
			bool p_isfocusEnable;
			bool p_isVisibleRect;
		protected:

			void CreateLayout();

		public:
			LayoutWidget();
			virtual ~LayoutWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value) final {} ;
			virtual void SetInitPos(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetPosValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;

			void createFocusEvent(std::function<void(ui::Widget*, ui::Widget*)> func_call_back);

			bool IsFocusEnable() { return p_isfocusEnable; }

			virtual bool Update(float dt)												override;

			bool IsVisibleRect();
		};

	}
}

#endif //__LAYOUT_WIDGET_H__

