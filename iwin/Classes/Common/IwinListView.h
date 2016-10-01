#ifndef __IWIN_LIST_VIEW_H__
#define __IWIN_LIST_VIEW_H__

#include "UI/AdvanceUI/WidgetEntity.h"
#include <functional>

namespace Utility {
	namespace UI_Widget {
		class ListViewWidget;
		class WidgetEntity;
	}
}

class IwinListView
{

private:
	Utility::UI_Widget::ListViewWidget* _list_view;
	Utility::UI_Widget::WidgetEntity*	_item;
	std::function<void(int, Utility::UI_Widget::WidgetEntity*, int)> _func_update_item;
	std::vector<Utility::UI_Widget::WidgetEntity*> _list_items;
public:
	IwinListView();
	virtual ~IwinListView();



	void init(Utility::UI_Widget::ListViewWidget* list_view, Utility::UI_Widget::WidgetEntity* item, std::function<void(int, Utility::UI_Widget::WidgetEntity*, int)> _func_update_item);
	void reload(int num_item);
	void handleItemList(std::function<void(Utility::UI_Widget::WidgetEntity*)> handle);
	void setVisible(bool visible);
};

#endif // __IWIN_LIST_VIEW_H__