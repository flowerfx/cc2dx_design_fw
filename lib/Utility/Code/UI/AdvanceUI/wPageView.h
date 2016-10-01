#ifndef __PAGE_WIDGET_H__
#define __PAGE_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class LayoutWidget;
		class PageViewWidget : public WidgetEntity
		{
		private:
			LayoutWidget * p_refer_child;
		public:
			PageViewWidget();
			virtual ~PageViewWidget();

			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetSizeValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void SetInteractive(bool value)										override;

			WidgetEntity * CreateWidgetReferAtIdx(int idx);
			void PushBackItem(WidgetEntity * item);
			int GetCurrentPageIdx();
			void JumpToPageIdx(int idx);
			void ScrollToPageIdx(int idx);
		};

	}
}

#endif //__SCROLL_WIDGET_H__

