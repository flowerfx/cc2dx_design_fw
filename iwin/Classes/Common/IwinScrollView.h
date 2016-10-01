#ifndef __IWIN_SCROLL_VIEW_H__
#define __IWIN_SCROLL_VIEW_H__

#include "UI/AdvanceUI/WidgetEntity.h"
#include <functional>

namespace Utility {
	namespace UI_Widget {
		class ScrollViewWidget;
		class WidgetEntity;
	}
}
namespace cocos2d {
	namespace ui {
		class Layout;
	}
}


class IwinScrollView 
{

private:
	Utility::UI_Widget::ScrollViewWidget* _scroll_view;
	Utility::UI_Widget::WidgetEntity*	_item;
	std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> _func_update_item;
	std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> _func_init_item;
	std::vector<Utility::UI_Widget::WidgetEntity*> _list_items;
	Layout*								_container;
	int _max_item;
public:
	IwinScrollView();
	virtual ~IwinScrollView();

	void init(Utility::UI_Widget::ScrollViewWidget* list_view, Utility::UI_Widget::WidgetEntity* item, 
		std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_update_item,
		std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_init_item);
	void reload(int num_item);
	void setMaxItem(int max_item);

};

#endif // __IWIN_LIST_VIEW_H__