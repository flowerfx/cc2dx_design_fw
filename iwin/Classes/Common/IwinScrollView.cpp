#include "IwinScrollView.h"
#include "cocos2d.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/LayerEntity.h"
#include "UI/AdvanceUI/wScrollView.h"

using namespace Utility;
using namespace Utility::UI_Widget;
using namespace cocos2d::ui;

IwinScrollView::IwinScrollView()
{

}

IwinScrollView::~IwinScrollView()
{
	_container->release();
}


void IwinScrollView::init(Utility::UI_Widget::ScrollViewWidget* scroll_view, Utility::UI_Widget::WidgetEntity* item,
	std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_update_item,
	std::function<void(IwinScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_init_item)
{
	_scroll_view = scroll_view;
	_item = item;
	_item->SetVisible(false);
	_func_update_item = func_update_item;
	_func_init_item = func_init_item;
	_container = Layout::create();
	_container->retain();
	_max_item = 1;
}

void IwinScrollView::reload(int num_item)
{
	_container->removeFromParent();
	if (_list_items.size() < (size_t)num_item)
	{
		int num_more = num_item - _list_items.size();
		WidgetEntity* entity;
		for (int i = 0; i < num_more; i++)
		{
			entity = _item->Clone();
			_list_items.push_back(entity);
			_container->addChild(entity->GetResource());
			if (_func_init_item) _func_init_item(this, entity, i);
		}
	}

	for (auto item : _list_items)
	{
		item->SetVisible(false);
	}

	Vec2 size = _item->GetSize();
	Vec2 view_size = _scroll_view->GetSize();
	int padding = 0;
	ScrollView::Direction dir = static_cast<ScrollView*>(_scroll_view->GetResource())->getDirection();
	if (dir == ScrollView::Direction::HORIZONTAL)
	{
		padding = (view_size.y - size.y * _max_item) / _max_item;
	}
	else
	{
		padding = (view_size.x - size.x * _max_item) / _max_item;
	}
	int round_value = floor(num_item / _max_item) + ((num_item % _max_item == 0) ? 0 : 1);
	int min_value;
	if (dir == ScrollView::Direction::HORIZONTAL)
	{
		min_value = view_size.x / (size.x + padding) + 1;
	}
	else
	{
		min_value = view_size.y / (size.y + padding) + 1;
	}

	round_value = MAX(round_value, min_value);

	for (int i = 0; i < num_item; i++)
	{
		WidgetEntity* entity = _list_items[i];
		entity->SetVisible(true);
		if (dir == ScrollView::Direction::HORIZONTAL)
		{
			entity->SetPosition(Vec2(padding / 2 + (size.x + padding) * (round_value - floor(i / _max_item) - 1), padding / 2 + (size.y + padding) * (i % _max_item)));
		}
		else
		{
			entity->SetPosition(Vec2(padding / 2 + (size.x + padding) * (i % _max_item), padding / 2 + (size.y + padding) * (round_value - floor(i / _max_item) - 1 )));
		}
		_func_update_item(this, entity, i);
	}
    cocos2d::Size container_size;
;
	if (dir == ScrollView::Direction::HORIZONTAL)
	{
        container_size = cocos2d::Size((size.x + padding) * round_value, _max_item * (size.y + padding));
	}
	else
	{
        container_size = cocos2d::Size((size.x + padding) * _max_item, round_value * (size.y + padding));
	}
	_container->setSize(container_size);
	static_cast<ScrollView*>(_scroll_view->GetResource())->setInnerContainerSize(container_size);
	_scroll_view->GetResource()->addChild(_container);
}

void IwinScrollView::setMaxItem(int max_item)
{
	_max_item = max_item;
}

