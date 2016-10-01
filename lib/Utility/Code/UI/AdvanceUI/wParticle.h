#ifndef __PARTICLE_WIDGET_H__
#define __PARTICLE_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class ParticleWidget : public WidgetEntity
		{
		private:
			ParticleSystemQuad* _particle;
			RKString		  _path_plist;
		public:
			ParticleWidget();
			virtual ~ParticleWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual cocos2d::Node * GetResource()										override;
			virtual bool SetNewResource(cocos2d::Node * resource)						override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual void CloneChild(WidgetEntity * p_clone)								override;

			virtual void SetSize(Vec2 s)												override;
			virtual Vec2 GetSize()														override;

		};

	}
}
#endif //__PANEL_WIDGET_H__

