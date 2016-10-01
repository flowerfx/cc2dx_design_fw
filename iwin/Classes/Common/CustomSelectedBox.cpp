#include "CustomSelectedBox.h"
#include "Common/IwinListView.h"
#include "UI/AdvanceUI/WidgetEntity.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wButton.h"
using namespace Utility::UI_Widget;


CustomSelectedBox::CustomSelectedBox()
{
	_list_view = nullptr;
}

CustomSelectedBox::~CustomSelectedBox()
{
	CC_SAFE_DELETE(_list_view);
}

void CustomSelectedBox::init(WidgetEntity* entity, std::function<void(int, WidgetEntity*, int)> update_func)
{
	_list_view = new IwinListView();
	_list_view->init(static_cast<ListViewWidget*>(entity->GetWidgetChildByName(".list_select")), entity->GetWidgetChildByName(".select_item"), CC_CALLBACK_3(CustomSelectedBox::updateItemListView, this));
	static_cast<Button*>(entity->GetWidgetChildByName(".btn_select")->GetResource())->setZoomScale(0.0f);
	entity->GetWidgetChildByName(".select_item")->SetVisible(true);
	entity->GetWidgetChildByName(".select_item.btn_select_item")->SetVisible(false);
	entity->GetWidgetChildByName(".select_item.img_normal")->SetVisible(false);
	static_cast<Button*>(entity->GetWidgetChildByName(".select_item.btn_select_item")->GetResource())->setZoomScale(.0f);
	_update_item_func = update_func;
}

void CustomSelectedBox::updateItemListView(int eventType, WidgetEntity* entity, int idx)
{
	if (eventType == 0)
	{
		entity->GetWidgetChildByName(".btn_select_item")->SetVisible(true);
		entity->GetWidgetChildByName(".img_normal")->SetVisible(true);
		entity->GetWidgetChildByName(".btn_select_item")->GetResource()->setUserData(this);
	}
	_update_item_func(eventType, entity, idx);
}

void CustomSelectedBox::showListView(bool visible)
{
	_list_view->setVisible(visible);
}

void CustomSelectedBox::reload(int count)
{
	_list_view->reload(count);
}

void CustomSelectedBox::setSelectedID(int idx)
{
	_selected_idx = idx;
	_list_view->handleItemList([idx](WidgetEntity* entity) {
		int cur_idx = entity->GetWidgetChildByName(".btn_select_item")->GetResource()->getTag();
		entity->GetWidgetChildByName(".img_selected")->SetVisible(cur_idx == idx);
	});
}

int CustomSelectedBox::getSelectedID()
{
	return _selected_idx;
}