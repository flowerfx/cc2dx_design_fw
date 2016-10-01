#ifndef __CHAT_BOARD_H__
#define __CHAT_BOARD_H__

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


class ChatBoard 
{
private:
	Utility::UI_Widget::ScrollViewWidget* _scroll_view;
	Utility::UI_Widget::WidgetEntity*	_item;
	std::vector<Utility::UI_Widget::WidgetEntity*> _list_items;
	Layout*								_container;
	int									_num_chat;
	Vec2								_size_chat;
public:
	ChatBoard();
	virtual ~ChatBoard();

	void init(Utility::UI_Widget::ScrollViewWidget* list_view, Utility::UI_Widget::WidgetEntity* item);
	void addChat(std::string user_name, std::string message);
	void reset();

};

#endif // __IWIN_LIST_VIEW_H__