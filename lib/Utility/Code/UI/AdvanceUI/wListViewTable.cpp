#include "wListViewTable.h"
#include "LayerEntity.h"
#include "wLayout.h"
USING_NS_CC_EXT;
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ListViewTableWidget::ListViewTableWidget()
		{
			p_typeUI = UI_TYPE::UI_LIST_VIEW_TABLE;
			p_refer_child = nullptr;
			current_idx_selected = -1;
			p_size_child = Vec2(0, 0);
			number_size_of_cell = 0;
			p_list_widget.clear();
			_event_callback = nullptr;
			p_table = nullptr;
			current_idx_to = 0;
			p_widget_on_touch = nullptr;
			_state_drag_cell = 0;
			_on_end_drag = false;
			_state_drag_first_end = EventType_DragCell::DRAG_TOP_DOWN;
			_delta_pos = Vec2(0, 0);
			_current_distance_move = _first_distance_move = Vec2(0, 0);

		}
		ListViewTableWidget::~ListViewTableWidget()
		{
			if (p_refer_child)
			{
				p_refer_child->RemoveAllChild();
				delete p_refer_child;
				p_refer_child = nullptr;
			}
			_event_callback = nullptr;
			p_xml_value = nullptr;
			p_table = nullptr;

			for (int i = 0; i < (int)p_list_widget.size(); i++)
			{
				p_list_widget[i]->ClearChild();
				delete p_list_widget[i];
			}
			p_list_widget.clear();

			p_widget_on_touch = nullptr;
		}

		void ListViewTableWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<ListViewTableWidget*>(value);
			WidgetEntity::CopyValue(value);
			p_Resource->removeAllChildrenWithCleanup(true);
			//copy refer
			p_refer_child = static_cast<LayoutWidget*>(val->p_refer_child->Clone());

			current_idx_selected = val->current_idx_selected;
			p_size_child = val->p_size_child;
			number_size_of_cell = val->number_size_of_cell;

			_event_callback = val->_event_callback;
			p_table = val->p_table;
			current_idx_to = val->current_idx_to;
			p_widget_on_touch = val->p_widget_on_touch;
			_state_drag_cell = val->_state_drag_cell;
			_on_end_drag = val->_on_end_drag;
			_state_drag_first_end = val->_state_drag_first_end;
			_delta_pos = val->_delta_pos;
			_current_distance_move = val->_current_distance_move;
			_first_distance_move = val->_first_distance_move;
		}

		WidgetEntity * ListViewTableWidget::Clone()
		{
			ListViewTableWidget * p_clone = new ListViewTableWidget();
			p_clone->CopyValue(this);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);
			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void ListViewTableWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			p_xml_value = (xml::UILayerWidgetDec*)xml_value;
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void ListViewTableWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
		
			//create layout
			Layout * panel_layout = Layout::create();
			if (xml_value->GeneralValue->GetDataInt("clipping_enable") == 1)
			{
				panel_layout->setClippingEnabled(true);
			}
			else
			{
				panel_layout->setClippingEnabled(false);
			}

			int background_type = xml_value->GeneralValue->GetDataInt("background_type");

			if (background_type >= 0 && background_type <= 2)
			{
				panel_layout->setBackGroundColorType((ui::Layout::BackGroundColorType)background_type);

				if (background_type == 1)
				{
					Color4B color_layout = xml_value->GeneralValue->GetDataColor("tint");
					panel_layout->setBackGroundColor(Color3B(color_layout.r, color_layout.g, color_layout.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					panel_layout->setBackGroundColorOpacity(opacity_layout);
				}
				else if (background_type == 2)
				{
					Color4B color_layout_from = xml_value->GeneralValue->GetDataColor("tint");
					Color4B color_layout_to = xml_value->GeneralValue->GetDataColor("tint_to");
					panel_layout->setBackGroundColor(Color3B(color_layout_from.r, color_layout_from.g, color_layout_from.b),
						Color3B(color_layout_to.r, color_layout_to.g, color_layout_to.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					panel_layout->setBackGroundColorOpacity(opacity_layout);
				}
			}
			else if (background_type == 3)
			{
				RKString name_image = xml_value->GeneralValue->GetDataChar("source");
				bool IsUseAtlast = xml_value->GeneralValue->GetDataInt("use_atlas") == 1;

				//auto t_sprite = TextureMgr->GetSpriteByName(name_image.GetString());
				//Vec2 size_srpite = t_sprite->getContentSize();
				//p_Resource->setContentSize(Size(size_srpite.x, size_srpite.y));

				panel_layout->setBackGroundImage(name_image.GetString(), IsUseAtlast ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);
				bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);

				p_isScale9Enable = IsScale9Enable;
				panel_layout->setBackGroundImageScale9Enabled(IsScale9Enable);
				if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_image.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					panel_layout->setBackGroundImageCapInsets(cap_insert);
				}

			}
			else
			{
				PWARN1("Wrong input !");
			}
		
			number_size_of_cell = xml_value->GeneralValue->GetDataInt("number_cell");
			//panel_layout->addChild(panel);
			p_Resource = panel_layout;
		}

		void ListViewTableWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			PASSERT2(xml_value->p_list_child_widget.size() == 1, "the sample list must have 1 value to refer");
			auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(0), this->GetParentLayer(), this);
			_widget_child->SetMenuParent(this->GetMenuParent());
			_widget_child->SetParentWidget(this);
			_widget_child->GetResource()->retain();
			p_refer_child = static_cast<LayoutWidget*>(_widget_child);

			//InitTable(xml_value);
		}
		void ListViewTableWidget::InitTable()
		{
			xml::UILayerWidgetDec * xml_value = p_xml_value;
			//create table
			Vec2 inner_size = xml_value->GeneralValue->GetDataVector2("inner_size");
			inner_size.x = inner_size.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
			inner_size.y = inner_size.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
			TableView* panel = TableView::create(this, cocos2d::Size(inner_size.x, inner_size.y));

			p_size_child = xml_value->GeneralValue->GetDataVector2("size_child");
			p_size_child.x = p_size_child.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
			p_size_child.y = p_size_child.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;

			panel->setBounceable(xml_value->GeneralValue->GetDataInt("not_use_bounce") == 0);

			extension::ScrollView::Direction direction = (extension::ScrollView::Direction)xml_value->GeneralValue->GetDataInt("direction");
			panel->setDirection(direction);

			if (xml_value->GeneralValue->HaveDataVector2("Percent_Scroll"))
			{
				float bounce_value = 0.f;
				Vec2 value = xml_value->GeneralValue->GetDataVector2("Percent_Scroll");
				if (direction == extension::ScrollView::Direction::HORIZONTAL)
				{
					bounce_value = value.x;
				}
				else if (direction == extension::ScrollView::Direction::VERTICAL)
				{
					bounce_value = value.y;
				}

				_first_distance_move = Vec2(value.x * inner_size.x , value.y * inner_size.y);

				panel->SetBounceBackValue(bounce_value);
			}
			else
			{
				Vec2 value = Vec2(0, 0);
				if (direction == extension::ScrollView::Direction::HORIZONTAL)
				{
					value.x = 0.35f;
				}
				else if (direction == extension::ScrollView::Direction::VERTICAL)
				{
					value.y = 0.35f;
				}
				_first_distance_move = Vec2(value.x * inner_size.x, value.y * inner_size.y);
			}

			TableView::VerticalFillOrder vertial_fill_order = (extension::TableView::VerticalFillOrder)xml_value->GeneralValue->GetDataInt("vertical_fill_order");
			panel->setVerticalFillOrder(vertial_fill_order);
			panel->setDelegate(this);

			panel->setAnchorPoint(xml_value->GeneralValue->GetDataVector2("anchor_point_list"));
			p_table = panel;
			p_Resource->addChild(panel);

			p_table->EnableTouch(p_IsInteractive);
		}

		WidgetEntity * ListViewTableWidget::CreateWidgetRefer(int idx)
		{
			LayoutWidget * _layout_widget = nullptr;
			if (!p_refer_child)
				return nullptr;

			_layout_widget = static_cast<LayoutWidget*>(p_refer_child->Clone());

			//p_listChildWidget.push_back(_layout_widget);
			_layout_widget->GetResource()->setName(_layout_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (_layout_widget->GetZOrder() == 0)
			{
				_layout_widget->SetZOrder(p_listChildWidget.size());
			}
			_layout_widget->GetResource()->setLocalZOrder(_layout_widget->GetZOrder());
			_layout_widget->SetParentLayer(this->GetParentLayer());
			_layout_widget->SetParentWidget(this);
			_layout_widget->SetMenuParent(this->GetMenuParent());

			return _layout_widget;

		}

		int ListViewTableWidget::GetCurrentIdxSelected() { return current_idx_selected; }

		void ListViewTableWidget::SetCurrentIdxSelected(int idx) { current_idx_selected = idx; }

		void ListViewTableWidget::tableCellTouchedBegin(TableView* table, TableViewCell* cell)
		{
			if (!IsInteractive())
				return;

			if (_event_callback)
			{

				//PASSERT2(p_widget_on_touch == nullptr, "at this stage the widget on track must be null");
				auto resource_ = cell->getChildByTag(123);
				if (resource_)
				{
					for (int i = 0; i < (int)p_list_widget.size(); i++)
					{
						if (resource_ == p_list_widget[i]->GetResource())
						{
							p_widget_on_touch = p_list_widget[i];
							CCLOG("found list widget at: %ld", i);
							break;
						}
					}
				}
				_event_callback(this, EventType_SetCell::ON_BEGIN_TOUCH_CELL, p_widget_on_touch);
			}
		}

		void ListViewTableWidget::tableCellTouchedMove(TableView* table, TableViewCell* cell)
		{
			if (!IsInteractive())
				return;

			if (_event_callback)
			{
				WidgetEntity * resource_refer = nullptr;
				auto resource_ = cell->getChildByTag(123);
				if (resource_)
				{
					if (!p_widget_on_touch)
					{
						PASSERT2(p_widget_on_touch != nullptr, "widget on touch is null");
						return;
					}

					if (resource_ != p_widget_on_touch->GetResource())
					{
						PASSERT2(resource_ == p_widget_on_touch->GetResource(), "the resource of cell and widget on track must be the same pointer!");
						return;
					}

					_event_callback(this, EventType_SetCell::ON_MOVE_TOUCH_CELL, p_widget_on_touch);
				}
			}
		}

		void ListViewTableWidget::tableCellTouched(TableView* table, TableViewCell* cell)
		{
			if (!IsInteractive())
				return;


			CCLOG("cell touched at index: %ld", cell->getIdx());
			current_idx_selected = cell->getIdx();

			if (_event_callback)
			{
				WidgetEntity * resource_refer = nullptr;
				auto resource_ = cell->getChildByTag(123);
				if (resource_)
				{
					if (!p_widget_on_touch)
					{
						PASSERT2(p_widget_on_touch != nullptr, "widget on touch is null");
						return;
					}
					if (resource_ != p_widget_on_touch->GetResource())
					{
						PASSERT2(resource_ == p_widget_on_touch->GetResource(), "the resource of cell and widget on track must be the same pointer!");
						return;
					}
					_event_callback(this, EventType_SetCell::ON_END_TOUCH_CELL, p_widget_on_touch);
					p_widget_on_touch = nullptr;

				}
			}
		}

		cocos2d::Size ListViewTableWidget::tableCellSizeForIndex(TableView *table, ssize_t idx)
		{

			if (p_refer_child)
			{
				return cocos2d::Size(p_refer_child->GetSize().x, p_refer_child->GetSize().y);
			}
			else
			{
				return cocos2d::Size(p_size_child.x, p_size_child.y);
			}
		}

		TableViewCell* ListViewTableWidget::tableCellAtIndex(TableView *table, ssize_t idx)
		{
			WidgetEntity * resource_refer = nullptr;
			TableViewCell *cell = table->dequeueCell();
			if (!cell) {
				cell = new (std::nothrow) cocos2d::extension::TableViewCell();
				cell->autorelease();
				resource_refer = CreateWidgetRefer(0);
				_event_callback(this, EventType_SetCell::ON_INIT_CELL, resource_refer);

				if (resource_refer)
				{
					resource_refer->GetResource()->setTag(123);
					cell->addChild(resource_refer->GetResource());
					p_list_widget.push_back(resource_refer);
				}
			}
			else
			{
				auto resource_ = cell->getChildByTag(123);
				if (!resource_)
				{
					resource_refer = CreateWidgetRefer(0);
					_event_callback(this, EventType_SetCell::ON_INIT_CELL, resource_refer);
					if (resource_refer)
					{
						resource_refer->GetResource()->setTag(123);
						cell->addChild(resource_refer->GetResource());
					}
					p_list_widget.push_back(resource_refer);
				}
				else
				{
					for (int i = 0; i < (int)p_list_widget.size(); i++)
					{
						if (resource_ == p_list_widget[i]->GetResource())
						{
							resource_refer = p_list_widget[i];
							break;
						}
					}
					
				}
			}
			if (_event_callback && resource_refer)
			{
				current_idx_to = idx;
				_event_callback(this, EventType_SetCell::ON_SET_CELL , resource_refer);
			}

			return cell;
		}

		ssize_t ListViewTableWidget::numberOfCellsInTableView(TableView *table)
		{
			return number_size_of_cell;
		}

		void ListViewTableWidget::SetNumberSizeOfCell(size_t number, bool reload_)
		{

			if (number == number_size_of_cell)
				return;

		//	RefreshListView();

			bool need_reload = number_size_of_cell == 0;
			if (reload_)
			{
				need_reload = reload_;
			}
			number_size_of_cell = number;
			if (p_table)
			{
				if (need_reload)
				{
					p_table->reloadData();
				}
				else
				{
					p_table->OnRefreshCell();
				}
			}
			else
			{
				PASSERT2(p_table != nullptr, "table is null, the bug happened!");
			}
		}

		size_t ListViewTableWidget::GetNumberSizeOfCell()
		{
			return number_size_of_cell;
		}

		void ListViewTableWidget::OnEndDragList(TableView * table , int stage)
		{
			if (_event_callback)
			{
				if (_state_drag_cell == 1 && !_on_end_drag)
				{
					_on_end_drag = true;
					EventType_SetCell event_type = EventType_SetCell::ON_END_DRAG_CELL;
					_current_distance_move = Vec2(0, 0);
					_event_callback(this, event_type, this);

				}
				
			}
		}

		void ListViewTableWidget::OnBeginDragList(cocos2d::extension::TableView * table, Vec2 pos, int stage)
		{
			if (_event_callback && _state_drag_cell < 2)
			{

				EventType_SetCell event_type = EventType_SetCell::ON_BEGIN_DRAG_CELL;
				if (_state_drag_cell == 0)
				{
					_state_drag_first_end = (EventType_DragCell)stage;
					_previous_pos = pos;		
					_state_drag_cell = 1;

					_current_distance_move = _first_distance_move ;
	
				}
				else if(_state_drag_cell == 1)
				{
					event_type = EventType_SetCell::ON_DRAG_CELL;
					_delta_pos = (_previous_pos - pos);
					//CCLOG("delta pos : [%f , %f] ", _delta_pos.x, _delta_pos.y);
					//_delta_pos = Vec2(abs(_delta_pos.x), abs(_delta_pos.y));
					_previous_pos = pos;
					if (_delta_pos.y < 0)
					{
						if (_state_drag_first_end == EventType_DragCell::DRAG_TOP_DOWN)
						{
							_state_drag_first_end = EventType_DragCell::DRAG_TOP_UP;
						}
						else if (_state_drag_first_end == EventType_DragCell::DRAG_BOT_UP)
						{
							_state_drag_first_end = EventType_DragCell::DRAG_BOT_DOWN;
						}
					}
					else
					{

					}
					_current_distance_move -= _delta_pos;				
				}
				

				_event_callback(this, event_type, this);
			}
		}
		
		void ListViewTableWidget::OnDragList(cocos2d::extension::TableView * table, Vec2 pos, int stage)
		{
			if (_event_callback)
			{
				EventType_SetCell event_type = EventType_SetCell::ON_DRAG_CELL;

				_delta_pos = (_previous_pos - pos);

				_previous_pos = pos;

				_event_callback(this, event_type, this);
			}
		}

		void ListViewTableWidget::OnEndDropList(cocos2d::extension::TableView * table, int stage)
		{
			if (_event_callback)
			{
				EventType_SetCell event_type = EventType_SetCell::ON_END_DROP_CELL;

				_current_distance_move = Vec2(0,0);

				_event_callback(this, event_type, this);
			}
		}

		void ListViewTableWidget::OnBeginDropList(cocos2d::extension::TableView * table, Vec2 pos, int stage)
		{
			if (_event_callback)
			{
				_previous_pos = pos;
				EventType_SetCell event_type = EventType_SetCell::ON_BEGIN_DROP_CELL;

				_current_distance_move = _first_distance_move;
				_state_drag_cell = 0;

				_event_callback(this, event_type, this);
				_on_end_drag = false;

			}
		}

		void ListViewTableWidget::OnDropList(cocos2d::extension::TableView * table, Vec2 pos, int stage)
		{
			if (_event_callback)
			{

				EventType_SetCell event_type = EventType_SetCell::ON_DROP_CELL;
				_delta_pos = (_previous_pos - pos);
				//_delta_pos = Vec2(abs(_delta_pos.x), abs(_delta_pos.y));
				_current_distance_move -= _delta_pos;
			
				_previous_pos = pos;

				_event_callback(this, event_type, this);
			}
		}

		void ListViewTableWidget::addEventListener(const wSetCellListViewTable& callback)
		{
			_event_callback = callback;
		}

		int ListViewTableWidget::GetCurrentIdxTo()
		{
			return current_idx_to;
		}

		void ListViewTableWidget::CycleList(bool forward)
		{
			if (forward)
			{
				auto temp = p_list_widget[0];
				for (int i = 0; i < (int)p_list_widget.size() - 1; i++)
				{
					p_list_widget[i] = p_list_widget[i + 1];
				}
				p_list_widget[p_list_widget.size() - 1] = temp;
			}
			else
			{
				auto temp = p_list_widget[p_list_widget.size() - 1];
				for (int i = p_list_widget.size() - 1; i >= 1; i--)
				{
					p_list_widget[i] = p_list_widget[i - 1];
				}
				p_list_widget[0] = temp;
			}
		}

		void ListViewTableWidget::ResetListView()
		{
			number_size_of_cell = 0;
			if (p_table)
			{
				p_table->reloadData();
			}
			//p_list_widget.clear();
			//p_table->retain();
			//this->ClearChild();
		}

		void ListViewTableWidget::JumpToItemIdx(int idx)
		{
		}

		std::vector<WidgetEntity *> ListViewTableWidget::GetListWidgetSample()
		{
			return p_list_widget;
		}

		void ListViewTableWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			if (p_table)
			{
				p_table->EnableTouch(value);
			}
			WidgetEntity::SetInteractive(value);

		}

		void ListViewTableWidget::RefreshListView()
		{
			if (p_table)
			{
				p_table->RefreshTableView();
			}
		}

		EventType_DragCell ListViewTableWidget::current_stage_drag()
		{
			return _state_drag_first_end;
		}

		Vec2  ListViewTableWidget::GetDeltaPos()
		{
			return _delta_pos;
		}

		float ListViewTableWidget::GetPercentMoveDistance()
		{
			float value_return = 0.f;
			if (_first_distance_move.x == 0 && _first_distance_move.y != 0)
			{
				value_return = (_current_distance_move.y / _first_distance_move.y) * 100.f;
			}
			else if (_first_distance_move.y == 0 && _first_distance_move.x != 0)
			{
				value_return = (_current_distance_move.x / _first_distance_move.x) * 100.f;
			}
			if (value_return < 0.f)
				value_return = 0.f;

			return value_return;
		}

		bool ListViewTableWidget::HaveEndDrag()
		{
			return _on_end_drag;
		}
	}
}