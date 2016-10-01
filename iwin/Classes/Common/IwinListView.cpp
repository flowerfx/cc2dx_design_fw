#include "IwinListView.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/wListView.h"

using namespace Utility::UI_Widget;


IwinListView::IwinListView()
{

}

IwinListView::~IwinListView()
{
	for (size_t i = 0; i < _list_items.size(); i++)
	{
		_list_items[i]->GetResource()->release();
	}
}


void IwinListView::init(Utility::UI_Widget::ListViewWidget* list_view, Utility::UI_Widget::WidgetEntity* item, std::function<void(int, Utility::UI_Widget::WidgetEntity*, int)> func_update_item)
{
	_list_view = list_view;
	_item = item;
	_item->SetVisible(false);
	_func_update_item = func_update_item;
}

void IwinListView::reload(int num_item)
{
	if (_list_items.size() < (size_t)num_item)
	{
		int num_more = num_item - _list_items.size();
		for (int i = 0; i < num_more; i++)
		{
			_list_items.push_back((WidgetEntity*)(_item->Clone()));
			_list_items[i]->GetResource()->retain();
			_func_update_item(0, _list_items[i], i);
		}
	}

	for (auto item : _list_items)
	{
		item->GetResource()->removeFromParent();

	}
	_list_view->GetResource()->removeAllChildren();
	for (auto i = 0; i < num_item; i++)
	{
		_list_view->PushBackItem(_list_items.at(i));
		_func_update_item(1, _list_items.at(i), i);
		_list_items.at(i)->SetVisible(true);
	}
}

void IwinListView::handleItemList(std::function<void(Utility::UI_Widget::WidgetEntity*)> handle)
{
	for (size_t i = 0; i < _list_items.size(); i++)
	{
		handle(_list_items.at(i));
	}
}

void IwinListView::setVisible(bool visible)
{
	_list_view->SetVisible(visible);
}