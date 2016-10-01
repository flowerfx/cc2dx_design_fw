#include "ChatBoard.h"
#include "cocos2d.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/LayerEntity.h"
#include "UI/AdvanceUI/wScrollView.h"
#include "UI/AdvanceUI/wText.h"

using namespace Utility;
using namespace Utility::UI_Widget;
using namespace cocos2d::ui;

ChatBoard::ChatBoard()
{
	_size_chat = Vec2(0, 0);
}

ChatBoard::~ChatBoard()
{
	for (auto it : _list_items)
	{
		delete it;
	}
	_list_items.clear();
	_container->removeAllChildrenWithCleanup(true);
	_scroll_view->GetResource()->removeAllChildrenWithCleanup(true);
	_scroll_view = nullptr;
	_item = nullptr;
}


void ChatBoard::init(Utility::UI_Widget::ScrollViewWidget* scroll_view, Utility::UI_Widget::WidgetEntity* item)
{
	_scroll_view = scroll_view;
	_item = item;
	_container = Layout::create();
	_container->setCascadeOpacityEnabled(true);
	scroll_view->GetResource()->addChild(_container);
	_num_chat = 0;
}

void ChatBoard::addChat(std::string user_name, std::string msg)
{
	std::string message = StringUtils::format("[#ff5722]%s :[#ffffff]%s", user_name.c_str(), msg.c_str());
	_num_chat++;
	if (_list_items.size() < _num_chat)
	{
		WidgetEntity* entity = _item->Clone();
		entity->Visible(true);
		entity->SetParentWidget(nullptr);
		entity->GetResource()->removeFromParent();
		_list_items.push_back(entity);
		_container->addChild(entity->GetResource());
	}
	WidgetEntity * text_item = _list_items[_num_chat - 1];
	static_cast<TextWidget*>(text_item)->SetText(message, true);
	text_item->SetPosition(Vec2(5, _size_chat.y));
	Vec2 size_text = text_item->GetResource()->getContentSize();
	int number_line = static_cast<TextWidget*>(text_item)->GetNumberOfLine();
	float size_y_each_line = size_text.y / number_line;
	float font_size_text = static_cast<TextWidget*>(text_item)->GetFontSize();
	float delta = abs(size_y_each_line - font_size_text);
	size_text.y = (font_size_text * number_line) + delta;
	_size_chat.y += size_text.y;

	cocos2d::Size container_size = cocos2d::Size(size_text.x, _size_chat.y);
	_container->setSize(container_size);

	static_cast<ScrollView*>(_scroll_view->GetResource())->setInnerContainerSize(container_size);
	static_cast<ScrollView*>(_scroll_view->GetResource())->scrollToTop(0.5,false);

}

void ChatBoard::reset()
{
	for (auto it : _list_items)
	{
		delete it;
	}
	_list_items.clear();
	_container->removeAllChildrenWithCleanup(true);
	_num_chat = 0;
	_size_chat = Vec2(0, 0);
}
