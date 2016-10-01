#ifndef __PANEL_CLIPING_WIDGET_H__
#define __PANEL_CLIPING_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
#include "2d/CCClippingNode.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class PanelClippingWidget : public WidgetEntity
		{
		private:
			ClippingNode * p_resource;

			xml::UILayerWidgetDec * p_xml_dec;
		public:
			virtual cocos2d::Node * GetResource()						override;
			virtual bool SetNewResource(cocos2d::Node * resource)		override;
			
			PanelClippingWidget();
			virtual ~PanelClippingWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual void CloneChild(WidgetEntity * p_clone)								override;
			virtual bool Update(float dt)												override;
		};

	}
}

#endif //__PANEL_WIDGET_H__

