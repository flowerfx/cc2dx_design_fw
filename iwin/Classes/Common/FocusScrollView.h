#ifndef __FOCUS_SCROLLVIEW_H__
#define __FOCUS_SCROLLVIEW_H__

#include "cocos2d.h"
#include "UI/UIScrollView.h"
#include "UI/AdvanceUI/WidgetEntity.h"

namespace Utility {
	namespace UI_Widget {
		class WidgetEntity;
	}
}

using namespace Utility::UI_Widget;
USING_NS_CC;

class FocusScrollView : public ui::ScrollView
{
public:
	static FocusScrollView* create();

protected:

	virtual void startAttenuatingAutoScroll(const Vec2& deltaMove, const Vec2& initialVelocity);

public:
	virtual bool init();
	void reload(int num);
	void setUpdateItemFunc(std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_update_item);
	void setInitItemFunc(std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_init_item);
	void setChangeFocusFunc(std::function<void(FocusScrollView*, WidgetEntity*, bool, int)> func_change_focus);
	void setItemEntity(WidgetEntity* item);
	int getItemFocus();
	void focus(int item_idx);
	
private:
	void scrollTo(int item_idx);
	void updateScrolling(Ref* ref, ScrollView::EventType evType);

	std::vector<Vec2>		_list_node_pos;
	WidgetEntity*			_item;
	Layout*					_layout_container;
	std::vector<WidgetEntity*>		_list_items;
	std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> _func_update_item;
	std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> _func_init_item;
	std::function<void(FocusScrollView*, WidgetEntity*, bool, int)>	_func_change_focus;
	int						_max_items;
	int						_num_items;

	int						_cur_focus_idx;
};

#endif