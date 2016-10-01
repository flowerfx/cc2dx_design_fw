#ifndef __CUSTOM_SELECTED_BOX_H__
#define __CUSTOM_SELECTED_BOX_H__
#include "cocos2d.h"
#include <functional>

namespace Utility {
	namespace UI_Widget {
		class WidgetEntity;
	}
};
class IwinListView;

class CustomSelectedBox
{
private:
	IwinListView* _list_view;

	std::function<void(int, Utility::UI_Widget::WidgetEntity*, int)> _update_item_func;
	int _selected_idx;

public:
	CustomSelectedBox();
	virtual ~CustomSelectedBox();
	void init(Utility::UI_Widget::WidgetEntity* entity, std::function<void(int eventType, Utility::UI_Widget::WidgetEntity*, int)> update_func);

	void updateItemListView(int eventType, Utility::UI_Widget::WidgetEntity* entity, int idx);
	void showListView(bool visible);
	void reload(int count);
	void setSelectedID(int idx);
	int getSelectedID();
};

#endif // __CUSTOM_SELECTED_BOX_H