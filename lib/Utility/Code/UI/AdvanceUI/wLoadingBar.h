#ifndef __LOADING_BAR_WIDGET_H__
#define __LOADING_BAR_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class LoadingBarWidget : public WidgetEntity
		{
		private:


		public:
			LoadingBarWidget();
			virtual ~LoadingBarWidget();

			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInteractive(bool value)										override;

			float GetPercent();
			void SetPercent(float percent);
		};

	}
}

#endif //__LOADING_BAR_WIDGET_H__

