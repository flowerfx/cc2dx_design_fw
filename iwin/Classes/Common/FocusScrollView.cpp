#include "FocusScrollView.h"

float calculateAutoScrollTimeByInitialSpeed_(float initialSpeed)
{
	// Calculate the time from the initial speed according to quintic polynomial.
	float time = sqrtf(sqrtf(initialSpeed / 5));
	return time;
}

FocusScrollView* FocusScrollView::create()
{
	FocusScrollView* rs = new FocusScrollView();
	rs->init();
	rs->autorelease();
	return rs;
}

bool FocusScrollView::init()
{
	ScrollView::init();
	setScrollBarEnabled(false);
	addEventListener(CC_CALLBACK_2(FocusScrollView::updateScrolling, this));
	_layout_container = Layout::create();
	addChild(_layout_container);
	_cur_focus_idx = -1;
	_func_change_focus = nullptr;
	_func_update_item = nullptr;
	return true;
}


void FocusScrollView::startAttenuatingAutoScroll(const Vec2& deltaMove, const Vec2& initialVelocity)
{
	float time = calculateAutoScrollTimeByInitialSpeed_(initialVelocity.length());
	Vec2 adjustedDeltaMove = flattenVectorByDirection(deltaMove);
	Vec2 final_pos = _innerContainer->getPosition() + adjustedDeltaMove;
	if (getDirection() == ScrollView::Direction::HORIZONTAL)
	{
		if (final_pos.x > _list_node_pos.front().x || final_pos.x < _list_node_pos.back().x)
		{
			if (final_pos.x > _list_node_pos.front().x)
			{
				focus(0);
			}
			else
			{
				focus(_list_node_pos.size() - 1);
			}
			startAutoScroll(adjustedDeltaMove, time, true);

			return;
		}

	}
	else
	{

	}

	for (int i = _list_node_pos.size() - 1; i >= 1; i--)
	{
		if (getDirection() == ScrollView::Direction::HORIZONTAL)
		{
			if (final_pos.x < _list_node_pos[i].x)
			{
				if (abs(_list_node_pos[i].x - final_pos.x) > abs(_list_node_pos[i - 1].x - final_pos.x))
				{
					adjustedDeltaMove.x = _list_node_pos[i - 1].x - _innerContainer->getPosition().x;
				}
				else
				{
					adjustedDeltaMove.x = _list_node_pos[i].x - _innerContainer->getPosition().x;
				}
				focus(i);
				break;
			}
		}
		else
		{
			// not implementing yet
		}
	}
	startAutoScroll(adjustedDeltaMove, time, true);
}

void FocusScrollView::setItemEntity(WidgetEntity* item)
{
	_item = item;
}

void FocusScrollView::reload(int num_item)
{
	_cur_focus_idx = -1;
	_num_items = num_item;
	if (_list_items.size() < (size_t)num_item)
	{
		int num_more = num_item - _list_items.size();
		WidgetEntity* entity;
		for (int i = 0; i < num_more; i++)
		{
			entity = _item->Clone();
			_list_items.push_back(entity);
			_layout_container->addChild(entity->GetResource());
			_func_init_item(this, entity, _list_items.size() - 1);
		}
		_max_items = num_item;
	}

	for (auto item : _list_items)
	{
		item->SetVisible(false);
	}

	Vec2 size = _item->GetSize();
	cocos2d::Size view_size = getSize();
	_list_node_pos.clear();
	for (int i = 0; i < num_item; i++)
	{
		_list_items.at(i)->SetPosition(Vec2(view_size.width / 2 + size.x*i, view_size.height / 2));
		_list_node_pos.push_back(Vec2( -size.x * i, view_size.height / 2));
		_list_items.at(i)->SetVisible(true);
		_list_items.at(i)->GetResource()->setTag(i);
		_func_change_focus(this, _list_items.at(i), false, i);
	}
	_layout_container->setVisible(true);
	cocos2d::Size container_size = cocos2d::Size(size.x * num_item - size.x + view_size.width, size.y);
	_layout_container->setSize(container_size);
	setInnerContainerSize(container_size);

	for (int i = 0; i < num_item; i++)
	{
		_func_update_item(this, _list_items.at(i), i);
	}
	focus(0);
}

void FocusScrollView::setUpdateItemFunc(std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_update_item)
{
	_func_update_item = func_update_item;
}

void FocusScrollView::setInitItemFunc(std::function<void(FocusScrollView*, Utility::UI_Widget::WidgetEntity*, int)> func_init_item)
{
	_func_init_item = func_init_item;
}


void FocusScrollView::setChangeFocusFunc(std::function<void(FocusScrollView*, WidgetEntity*, bool, int)> func_change_focus)
{
	_func_change_focus = func_change_focus;
}

void FocusScrollView::updateScrolling(Ref* ref, ScrollView::EventType evType)
{
	int half_size_view = getSize().width / 2;
	int center = abs(getInnerContainerPosition().x) + half_size_view;
	for (int i = 0; i < _num_items; i++)
	{
		Vec2 pos = _list_items[i]->GetPosition();
		int offset = abs(center - pos.x);
		if (offset > half_size_view)
		{
			_list_items[i]->GetResource()->setScale(.5f);
		}
		else
		{
			float percent = 1.0f - (offset * 1.0f / half_size_view);
			_list_items[i]->GetResource()->setScale(0.5f + (0.5f * percent));
		}
	}
}

int FocusScrollView::getItemFocus()
{
	return _cur_focus_idx;
}

void FocusScrollView::focus(int item_idx)
{
	if (item_idx == _cur_focus_idx) return;
	for (int i = 0; i < _num_items; i++)
	{
		_func_change_focus(this, _list_items.at(i), false, i);
	}
	_cur_focus_idx = item_idx;
	_func_change_focus(this, _list_items.at(item_idx), true, item_idx);
	scrollTo(item_idx);
}

void FocusScrollView::scrollTo(int item_idx)
{
	int container_width = this->getInnerContainerSize().width;// -getSize().width;
	Vec2 item_size = _item->GetSize();
	cocos2d::Size view_size = getSize();
	int offsetx = _list_items.at(item_idx)->GetPosition().x - view_size.width / 2;
    startAutoScrollToDestination(Vec2(-offsetx, _innerContainer->getPosition().y), 0.5f, true);

}