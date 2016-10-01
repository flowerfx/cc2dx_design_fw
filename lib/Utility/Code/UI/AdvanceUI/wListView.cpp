#include "wListView.h"
#include "LayerEntity.h"
#include "wLayout.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ListViewWidget::ListViewWidget()
		{
			p_typeUI = UI_TYPE::UI_LISTVIEW;
			p_refer_child.clear();
			current_idx_selected = -1;
			p_use_pool = true;

		}
		ListViewWidget::~ListViewWidget()
		{
			for (int i = 0; i < (int)p_refer_child.size(); i++)
			{
				delete p_refer_child[i];
				p_refer_child[i] = nullptr;
			}
			p_refer_child.clear();

			for (auto entity : p_pool_child)
			{
				entity->GetResource()->release();
			}
			p_pool_child.clear();
		}

		void ListViewWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<ListViewWidget*>(value);
			WidgetEntity::CopyValue(value);
			current_idx_selected = val->current_idx_selected;
			p_use_pool = val->p_use_pool;
			p_Resource->removeAllChildrenWithCleanup(true);
			for (auto c : val->p_refer_child)
			{
				p_refer_child.push_back(static_cast<LayoutWidget*>(c->Clone()));
			}
		}

		WidgetEntity * ListViewWidget::Clone()
		{
			ListViewWidget * p_clone = new ListViewWidget();
			p_clone->CopyValue(this);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void ListViewWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void ListViewWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{

			ListView* panel = ListView::create();

			panel->setScrollBarWidth(1);
			panel->setScrollBarPositionFromCorner(Vec2(0, 2));
			panel->setScrollBarColor(Color3B::WHITE);
			panel->setScrollBarEnabled(xml_value->GeneralValue->GetDataInt("scrollbar_enable") == 1);

			Vec2 inner_size = xml_value->GeneralValue->GetDataVector2("inner_size");

			inner_size.x = inner_size.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
			inner_size.y = inner_size.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
			panel->setInnerContainerSize(cocos2d::Size(inner_size.x, inner_size.y));
			panel->setBounceEnabled(xml_value->GeneralValue->GetDataInt("not_use_bounce") == 0);
			if(xml_value->GeneralValue->HaveDataVector2("Percent_Scroll"))
			{ 
				panel->SetPercentPosScrollBounce(xml_value->GeneralValue->GetDataVector2("Percent_Scroll"));
			}
			else
			{
				panel->SetPercentPosScrollBounce(Vec2(0.15f, 0.15f));
			}
			p_use_pool = xml_value->GeneralValue->GetDataInt("not_use_pool") == 0;
			//chien todo: 
			p_use_pool = false;
			ui::ScrollView::Direction direction = (ui::ScrollView::Direction)xml_value->GeneralValue->GetDataInt("direction");
			panel->setDirection(direction);

			//// set all items layout gravity
			panel->setGravity(ListView::Gravity::CENTER_VERTICAL);
			//
			//// set items margin
			//panel->setItemsMargin(3.0f);

			int background_type = xml_value->GeneralValue->GetDataInt("background_type");

			if (background_type >= 0 && background_type <= 2)
			{
				panel->setBackGroundColorType((ui::Layout::BackGroundColorType)background_type);

				if (background_type == 1)
				{
					Color4B color_layout = xml_value->GeneralValue->GetDataColor("tint");
					panel->setBackGroundColor(Color3B(color_layout.r, color_layout.g, color_layout.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					panel->setBackGroundColorOpacity(opacity_layout);
				}
				else if (background_type == 2)
				{
					Color4B color_layout_from = xml_value->GeneralValue->GetDataColor("tint");
					Color4B color_layout_to = xml_value->GeneralValue->GetDataColor("tint_to");
					panel->setBackGroundColor(Color3B(color_layout_from.r, color_layout_from.g, color_layout_from.b),
						Color3B(color_layout_to.r, color_layout_to.g, color_layout_to.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					panel->setBackGroundColorOpacity(opacity_layout);
				}
			}
			else if (background_type == 3)
			{
				RKString name_image = xml_value->GeneralValue->GetDataChar("source");
				bool IsUseAtlast = xml_value->GeneralValue->GetDataInt("use_atlas") == 1;

				//auto t_sprite = TextureMgr->GetSpriteByName(name_image.GetString());
				//Vec2 size_srpite = t_sprite->getContentSize();
				//p_Resource->setContentSize(Size(size_srpite.x, size_srpite.y));

				panel->setBackGroundImage(name_image.GetString(), IsUseAtlast ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);
				
				bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);
				p_isScale9Enable = IsScale9Enable;
				panel->setBackGroundImageScale9Enabled(IsScale9Enable);
				if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_image.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					panel->setBackGroundImageCapInsets(cap_insert);
				}
			}
			else
			{
				PWARN1("Wrong input !");
			}
			if (xml_value->GeneralValue->GetDataInt("clipping_enable") == 1)
			{
				panel->setClippingEnabled(true);
			}
			else
			{
				panel->setClippingEnabled(false);
			}

			bool as_page_view =( xml_value->GeneralValue->GetDataInt("as_page_view") == 1);
			if (as_page_view)
			{
				int number_move = xml_value->GeneralValue->GetDataInt("number_move");
				panel->SetAsPageView(as_page_view);
				panel->SetNumberItemMove(number_move);
			}

			int touch_item = xml_value->GeneralValue->GetDataInt("touch_item");
			if (touch_item > 0)
			{
				panel->setPropagateTouchEvents(true);
			}

			//panel->setInnerContainerPosition(panel->getSize());
			p_Resource = panel;
		}

		void ListViewWidget::SetSizeValueXML(xml::UILayerWidgetDec * xml_value)
		{
			ActionProcess::SetSizeValueXML(xml_value);

			static_cast<ListView*>(p_Resource)->jumpToTop();
		}

		void ListViewWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			//PASSERT2(xml_value->p_list_child_widget.size() == 1, "the sample list must have 1 value to refer");
			for (int i = 0; i < (int)(xml_value->p_list_child_widget.size()); i++)
			{
				auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(i), this->GetParentLayer(), this);
				_widget_child->SetMenuParent(this->GetMenuParent());
				_widget_child->SetParentWidget(this);

				if (_widget_child->getUIWidgetType() != UI_TYPE::UI_LAYOUT
					||
					xml_value->p_list_child_widget.at(i)->GeneralValue->GetDataInt("not_default_layout") == 1
					)
				{
					int idx = static_cast<ListView*>(p_Resource)->getChildrenCount();
					static_cast<ListView*>(p_Resource)->pushBackCustomItem(static_cast<Widget*>(_widget_child->GetResource()));

					_widget_child->SetName(_widget_child->GetName() /*+ std::to_string(idx)*/);
					p_listChildWidget.push_back(_widget_child);
					_widget_child->GetResource()->setName((_widget_child->GetName() /*+ std::to_string(idx)*/).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
					_widget_child->GetResource()->setLocalZOrder(p_listChildWidget.size());
					_widget_child->SetParentLayer(this->GetParentLayer());
				}
				else
				{
					p_refer_child.push_back(static_cast<LayoutWidget *>(_widget_child));
					p_refer_child.at(p_refer_child.size() -1 )->GetResource()->retain();
				}
			}
			//static_cast<ListView*>(p_Resource)->setItemModel(static_cast<Widget*>(_widget_child->GetResource()));
			//InsertChildWidget(_widget_child);
		}

		void ListViewWidget::InsertChildWidget(WidgetEntity* child_widget)
		{
			//p_listChildWidget.push_back(child_widget);
			//child_widget->GetResource()->setName(child_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			//child_widget->GetResource()->setZOrder(p_listChildWidget.size());
			//child_widget->SetParentLayer(this->GetParentLayer());
			//GetResource()->addChild(child_widget->GetResource());
		}

		LayoutWidget* ListViewWidget::CreateLayoutWidget()
		{
			LayoutWidget* rs = nullptr;
			for (auto entity : p_pool_child)
			{
				if (entity->GetResource()->getParent() == nullptr)
				{
					rs = entity;
				}
			}
			if (rs == nullptr)
			{
				if (p_refer_child.size() <= 0)
				{
					PASSERT2(p_refer_child.size() > 0, "size of p_refer_child is zero");
					return nullptr;
				}
				rs = static_cast<LayoutWidget*>(p_refer_child[0]->Clone());
				rs->GetResource()->retain();
				rs->retain();
				p_pool_child.push_back(rs);
			}

			return rs;
		}

		WidgetEntity * ListViewWidget::CreateWidgetReferNoPushBack(int refer_idx)
		{
			auto list_view = static_cast<ListView*>(p_Resource);
			if (refer_idx >= (int)p_refer_child.size())
			{
				PASSERT2(refer_idx < (int)p_refer_child.size(), "idx_refer is out of range!");
				refer_idx = 0;
				if (p_refer_child.size() <= 0)
				{
					PASSERT2(false, "size of p_refer_child is zero");
					return nullptr;
				}
			}
			LayoutWidget * _layout_widget = nullptr;
			if (p_use_pool)
			{
				_layout_widget = CreateLayoutWidget();
			}
			else
			{
				_layout_widget = static_cast<LayoutWidget*>(p_refer_child[refer_idx]->Clone());
			}
			//_layout_widget->SetName(_layout_widget->GetName() + std::to_string(idx));
			//_layout_widget->GetResource()->setName((_layout_widget->GetName() + std::to_string(idx)).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic

			return _layout_widget;
		}

		WidgetEntity * ListViewWidget::CreateWidgetRefer(int idx , int idx_refer)
		{
			auto list_view = static_cast<ListView*>(p_Resource);
			if (idx_refer >= (int)p_refer_child.size() )
			{
				PASSERT2(idx_refer < (int)p_refer_child.size(), "idx_refer is out of range!");
				idx_refer = 0;
				if (p_refer_child.size() <= 0)
				{
					PASSERT2(false, "size of p_refer_child is zero");
					return nullptr;
				}
			}
			LayoutWidget * _layout_widget = nullptr;
			if(p_use_pool)
			{
				_layout_widget = CreateLayoutWidget();
			}
			else
			{
				_layout_widget = static_cast<LayoutWidget*>(p_refer_child[idx_refer]->Clone());
			}

			list_view->pushBackCustomItem(static_cast<Widget*>(_layout_widget->GetResource()));

			_layout_widget->SetName(_layout_widget->GetName() + std::to_string(idx));
			p_listChildWidget.push_back(_layout_widget);
			_layout_widget->GetResource()->setName((_layout_widget->GetName() + std::to_string(idx)).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (_layout_widget->GetZOrder() == 0)
			{
				_layout_widget->SetZOrder(p_listChildWidget.size());
			}
			_layout_widget->GetResource()->setLocalZOrder(_layout_widget->GetZOrder());
			_layout_widget->SetParentLayer(this->GetParentLayer());
            _layout_widget->SetMenuParent(this->GetMenuParent());
			_layout_widget->SetParentWidget(this);

			return _layout_widget;

		}

		void ListViewWidget::ResetListView()
		{
			if (!p_use_pool)
			{
				GetResource()->removeAllChildren();
			}
			else
			{
				auto list_child = this->GetResource()->getChildren();
				for (int i = 0; i < list_child.size(); i++)
				{
					this->GetResource()->removeChild(list_child.at(i));
				}
				list_child.clear();
			}
			for (auto entity : p_pool_child)
			{
				entity->SetParentWidget(nullptr);
			}
			//p_listChildWidget.clear();
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDeleteResource(false);
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();
		}

		void ListViewWidget::PushBackItem(WidgetEntity * _layout_widget)
		{
			auto list_view = static_cast<ListView*>(p_Resource);

			list_view->pushBackCustomItem(static_cast<Widget*>(_layout_widget->GetResource()));

			p_listChildWidget.push_back(_layout_widget);
			_layout_widget->GetResource()->setName(_layout_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (_layout_widget->GetZOrder() == 0)
			{
				_layout_widget->SetZOrder(p_listChildWidget.size());
			}
			_layout_widget->GetResource()->setLocalZOrder(_layout_widget->GetZOrder());
			_layout_widget->SetParentLayer(this->GetParentLayer());
			_layout_widget->SetParentWidget(this);
			_layout_widget->SetMenuParent(this->GetMenuParent());
		}

		void ListViewWidget::JumpToItemIdx(int idx)
		{
			static_cast<ListView*>(p_Resource)->jumpToItem(idx, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
		}

		void ListViewWidget::ForceVisibleAllChildLayout(RKString path, bool visible)
		{
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				auto widget_layout = p_listChildWidget.at(i);
				if (widget_layout->getUIWidgetType() == UI_TYPE::UI_LAYOUT)
				{
					auto child_as_path = widget_layout->GetWidgetChildByName(path);
					if (child_as_path)
					{
						child_as_path->Visible(visible);
					}
				}
			}
		}

		void ListViewWidget::ClearChild(int count)
		{
			if (count != 0)
			{
				//static_cast<Widget*>(this->GetResource())->removeFromParentAndCleanup(true);
				count++;
			}
			if (!p_use_pool)
			{
				this->GetResource()->removeAllChildren();
			}
			else
			{
				auto list_child = this->GetResource()->getChildren();
				for (int i = 0; i < list_child.size(); i++)
				{
					this->GetResource()->removeChild(list_child.at(i));
				}
				list_child.clear();
			}
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDeleteResource(false);
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();
		}

		int ListViewWidget::GetCurrentIdxSelected() { return current_idx_selected; }

		void ListViewWidget::SetCurrentIdxSelected(int idx) { current_idx_selected = idx; }

		void ListViewWidget::SetNumberEachSlide(int number)
		{
			auto list_view = static_cast<ListView*>(GetResource());
			if (list_view->BehaviousAsPage())
			{
				list_view->SetNumberItemMove(number);
			}
		}

		void ListViewWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<ListView*>(p_Resource)->setTouchEnabled(value);
		}
	}
}