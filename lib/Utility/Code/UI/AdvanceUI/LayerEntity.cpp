#include "LayerEntity.h"

#include "wButton.h"
#include "wLayout.h"
#include "wText.h"
#include "wPanel.h"
#include "wListView.h"
#include "wScrollView.h"
#include "wPageView.h"
#include "wTextField.h"
#include "wCheckBox.h"
#include "wSlider.h"
#include "wEditBox.h"
#include "wPanelClipping.h"
#include "wLoadingBar.h"
#include "wRadioBtn.h"
#include "wTimeProcessWidget.h"
#include "wListViewTable.h"
#include "wTimeDigitalWidget.h"
#include "wAnimationWidget.h"
#include "wParticle.h"

#include "../../SoundManager.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		LayerEntity::LayerEntity()
		{
			p_listWidgetChild.clear();
			p_listLayerChild.clear();

			//setTouchEnabled(true);

			p_current_idx_appear = -1;
			p_number_child_have_action = 0;

			p_typeUI = UI_TYPE::UI_LAYER;

			RegisterUIWidget();

			p_isRootLayer = false;
		}

		LayerEntity::~LayerEntity()
		{
			ReleaseChildWidget();

			removeAllChildren();

			p_current_idx_appear = -1;
			p_number_child_have_action = 0;

		}

		void LayerEntity::ReleaseChildWidget()
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				delete p_listWidgetChild.at(i);
			}
			p_listWidgetChild.clear();

			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->removeAllChildren();
				p_listLayerChild.at(i)->ReleaseChildWidget();
			}
			p_listLayerChild.clear();
		}

		void LayerEntity::AddWidgetWithName(Widget * element, RKString name)
		{
			if (element)
			{
				element->setName(name.GetString());
				//if (p_z_order == 0)
				//{
				//	p_z_order = p_listWidgetChild.size();
				//}
				element->setLocalZOrder(p_listWidgetChild.size());
				this->addChild(element, p_z_order);
			}

		}


		cocos2d::Node * LayerEntity::GetResource()
		{
			return this;
		}

		bool LayerEntity::SetNewResource(cocos2d::Node * resource)
		{
			return false;
		}

		Widget * LayerEntity::GetWidgetElementByName(RKString name)
		{
			return static_cast<Widget*>(getChildByName(name.GetString()));
		}

		WidgetEntity * LayerEntity::GetWidgetEntityByName(RKString name)
		{
			for (unsigned int i = 0; i < p_listWidgetChild.size(); i++)
			{
				if (p_listWidgetChild.at(i)->GetName() == name)
				{
					return p_listWidgetChild.at(i);
				}
			}
			return 0;
		}

		void LayerEntity::SetIdxProcessGame(int idx) {
			p_current_idx_appear = idx;
		}

		std::vector<LayerEntity* > LayerEntity::GetListLayerChild() {
			return p_listLayerChild;
		}

		void LayerEntity::HaveChildFinishAction()
		{
			p_number_child_have_action--;

			if (p_number_child_have_action < 0)
			{
				p_number_child_have_action = 0;
			}
		}

		void LayerEntity::AddLayerEntity(LayerEntity * layer)
		{
			if (layer)
			{
				p_listLayerChild.push_back(layer);

				layer->setName(layer->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
				if (layer->p_z_order == 0)
				{
					layer->p_z_order = p_listWidgetChild.size();
				}
				layer->setLocalZOrder(layer->p_z_order);
				layer->SetParentLayer(this);
				this->addChild(layer, layer->p_z_order);
			}
		}

		bool LayerEntity::IsRootLayer()
		{
			return p_isRootLayer;
		}

		void LayerEntity::SetRootLayer(bool value) {
			p_isRootLayer = value;
		}

		void LayerEntity::AddWidgetEntity(WidgetEntity * layer)
		{
			if (layer)
			{
				//layer->SetParentLayer(this);
				p_listWidgetChild.push_back(layer);
				AddWidgetWithName((Widget*)layer->GetResource(), layer->GetName());
			}
		}

		bool LayerEntity::RemoveWigetEnityByName(RKString name)
		{
			if (name != "")
			{
				int idx_layer = -1;
				for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
				{
					if (p_listWidgetChild.at(i)->GetName() == name)
					{
						idx_layer = i;
						break;
					}
				}
				if (idx_layer == -1)
					return false;
				this->removeChild((Widget*)p_listWidgetChild.at(idx_layer));
				p_listWidgetChild.at(idx_layer)->autorelease();
				p_listWidgetChild.erase(p_listWidgetChild.begin() + idx_layer);
				return true;
			}
			return false;
		}

		bool LayerEntity::RemoveWidgetEntity(WidgetEntity * layer)
		{
			if (layer)
			{
				int idx_layer = -1;
				for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
				{
					if (p_listWidgetChild.at(i) == layer)
					{
						idx_layer = i;
						break;
					}
				}
				if (idx_layer == -1)
					return false;
				p_listWidgetChild.at(idx_layer)->autorelease();
				p_listWidgetChild.erase(p_listWidgetChild.begin() + idx_layer);
				this->removeChild((Widget*)layer->GetResource());
				layer = nullptr;
				return true;
			}
			return false;
		}

		void LayerEntity::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (p_Visible)
			{
				Layer::draw(renderer, transform, flags);
			}
		}

		void LayerEntity::visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (p_Visible)
			{
				Layer::visit(renderer, transform, flags);
			}
		}

		bool LayerEntity::Update(float dt)
		{
			if (!p_Visible)
				return false;

			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				p_listWidgetChild.at(i)->Update(dt);
			}

			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->Update(dt);
			}

			return true;
		}

		void LayerEntity::AddEventType(xml::TYPE_UI_WIDGET type, WidgetEntity * wEntity, xml::UILayerWidgetDec* _widget)
		{
			switch (type)
			{
			case xml::TYPE_UI_WIDGET::WIDGET_BUTTON:
			{
				((Button*)(wEntity->GetResource()))->addTouchEventListener(CC_CALLBACK_2(LayerEntity::OnTouchEvent, this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW:
			{
				((ListView*)(wEntity->GetResource()))->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(LayerEntity::OnSelectListItem, this, wEntity));
				if (_widget && _widget->GeneralValue->GetDataInt("scroll_event") == 1)
				{
					((ListView*)(wEntity->GetResource()))->addEventListener((ui::ScrollView::ccScrollViewCallback)CC_CALLBACK_2(LayerEntity::OnScrollWidgetEvent, this, wEntity));
				}
				if (_widget && _widget->GeneralValue->GetDataInt("as_page_view") == 1)
				{
					((ListView*)(wEntity->GetResource()))->addEventMovePageListender((ui::ListView::ccListViewMovePageCallback)CC_CALLBACK_2(LayerEntity::OnMovePageEvent, this, wEntity));
				}
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_TEXT_FIELD:
			{
				static_cast<TextFieldWidget*>(wEntity)->AddEventHandler(CC_CALLBACK_2(LayerEntity::OnTextFieldEvent, this, wEntity));

				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_SLIDER:
			{
				static_cast<Slider*>(wEntity->GetResource())->addEventListener(CC_CALLBACK_2(LayerEntity::OnSliderEvent, this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_PAGE_VIEW:
			{
				static_cast<PageView*>(wEntity->GetResource())->addEventTurnListener(CC_CALLBACK_2(LayerEntity::OnPageViewEvent, this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_TIME_PROCESS:
			{
				static_cast<TimeProcessWidget*>(wEntity)->addEventListener(CC_CALLBACK_2(LayerEntity::OnProgressTimer, this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW_TABLE:
			{
				static_cast<ListViewTableWidget*>(wEntity)->addEventListener(
					[this](cocos2d::Ref* sender, EventType_SetCell type, WidgetEntity* _widget)
				{
					this->OnSetCellTable(sender, type, _widget);
				});
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_TIME_DIGITAL:
			{
				static_cast<TimeDigitalWidget*>(wEntity)->addEventListener(CC_CALLBACK_2(LayerEntity::OnDigitalTimer, this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_CHECK_BOX:
			{
				static_cast<ui::CheckBox*>(wEntity->GetResource())->addEventListener(CC_CALLBACK_2(LayerEntity::OnCheckBoxEvent,this, wEntity));
				break;
			}
			case xml::TYPE_UI_WIDGET::WIDGET_ANIM:
			{
				static_cast<AnimWidget*>(wEntity)->addEventListener(CC_CALLBACK_2(LayerEntity::OnFinishAnimate, this, wEntity));
				break;
			}

			}
		}

		WidgetEntity * LayerEntity::InitWidgetEntity(xml::UILayerWidgetDec * widget, LayerEntity * parent_layer, WidgetEntity * parent_widget)
		{
			if (widget)
			{
				xml::UILayerWidgetDec* _widget = static_cast<xml::UILayerWidgetDec*>(widget);

				if (widget->typeValue == xml::TYPE_UI_WIDGET::WIDGET_LAYER)
				{
					PASSERT2(false, "ERROR : Dont init layer in this function");
					return nullptr;
				}
                WidgetEntity * wEntity = nullptr;
                if(widget->typeValue == xml::TYPE_UI_WIDGET::WIDGET_TEXT_FIELD)
                {
#if defined OS_IOS || defined OS_MAC || defined OS_ANDROID || defined OS_W10 || defined WP8 || defined OS_W8
                    auto text_field = new EditBoxWidget();
                    wEntity = text_field;
#else
                    auto text_field = new TextFieldHandleWidget();
                    wEntity = text_field;
#endif
                }
                else
                {
                    wEntity = p_factory_ui_widget.CreateNewProduct(widget->typeValue);
                }
                
                if (wEntity)
                {
                    wEntity->InitParamParent(widget->NameWidget, _widget, parent_layer, parent_widget);
                    wEntity->SetValueFromXML(_widget);
                    wEntity->FindAndInsertChildWidget(widget);
                }

				if (_widget->GeneralValue->GetDataInt("no_add_event") == 0)
				{
					AddEventType(widget->typeValue, wEntity, _widget);
				}


				return wEntity;
			}

			return nullptr;
		}

        
		void LayerEntity::OnTextFieldEvent(cocos2d::Ref* sender, EventType_TextField type, WidgetEntity* _widget)
		{
			//TextField* textField = dynamic_cast<TextField*>(sender);
			switch (type)
			{
            case EventType_TextField::ON_ATTACH_WITH_IME:
			{
				//textField->setPlaceHolder("");
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_ATTACH_IME", _widget->getUIWidgetType(), _widget);
					}
					menu_point_to->SetStageShowKeyboard(1, _widget->GetParentWidget());
				}
				break;
			}
			case EventType_TextField::ON_DETACH_WITH_IME:
			{
				//textField->setPlaceHolder(static_cast<TextFieldWidget*>(_widget)->GetDefaultPlaceHolder().GetString());
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_DETACH_IME", _widget->getUIWidgetType(), _widget);
					}
					menu_point_to->SetStageShowKeyboard(2, _widget->GetParentWidget());
				}
				break;
			}
			case EventType_TextField::ON_INSERT_TEXT:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_INSERT_TEXT", _widget->getUIWidgetType(), _widget);
					}
				}
				break;
			}
			case EventType_TextField::ON_DELETE_TEXT:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_DELETE_BACKWARD", _widget->getUIWidgetType(), _widget);
					}
				}

			//	if (textField->getString() == "" && !textField->isCursorEnable())
			//	{
			//		textField->setPlaceHolder(static_cast<TextFieldWidget*>(_widget)->GetDefaultPlaceHolder().GetString());
			//	}
				break;
			}
			}
		}

		void LayerEntity::OnSliderEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type, WidgetEntity* _widget)
		{
			if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
			{
				RKString name = _widget->GetName() + "_PERCENTAGE_CHANGED";
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, name, _widget->getUIWidgetType(), _widget);
					}
				}
			}
			else if (type == Slider::EventType::ON_SLIDEBALL_UP)
			{
				if (_widget->IsInteractive())
				{
					RKString name = _widget->GetName() + "_SLIDEBALL_UP";
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, name, _widget->getUIWidgetType(), _widget);
					}
				}
			}
		}

		void LayerEntity::OnScrollWidgetEvent(cocos2d::Ref* sender, cocos2d::ui::ScrollView::EventType type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case  ui::ScrollView::EventType::SCROLL_TO_TOP:
			{
				break;
			}
			case  ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
			{
				break;
			}
			case  ui::ScrollView::EventType::SCROLL_TO_LEFT:
			{
				break;
			}
			case  ui::ScrollView::EventType::SCROLL_TO_RIGHT:
			{
				break;
			}
			case  ui::ScrollView::EventType::SCROLLING:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						RKString name = _widget->GetName() + "_SCROLLING";
						func_call_widget(*menu_point_to, name, _widget->getUIWidgetType(), _widget);
					}
				}

				break;
			}
			case  ui::ScrollView::EventType::BOUNCE_TOP:
			{
				break;
			}
			case  ui::ScrollView::EventType::BOUNCE_BOTTOM:
			{
				break;
			}
			case  ui::ScrollView::EventType::BOUNCE_LEFT:
			{
				break;
			}
			case  ui::ScrollView::EventType::BOUNCE_RIGHT:
			{
				break;
			}
			case  ui::ScrollView::EventType::CONTAINER_MOVED:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						RKString name = _widget->GetName() + "_CONTAINER_MOVED";
						func_call_widget(*menu_point_to, name, _widget->getUIWidgetType(), _widget);
					}
				}

				break;
			}
			}
		}

		void LayerEntity::OnMovePageEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventMovePage type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case cocos2d::ui::ListView::EventMovePage::ON_MOVE_NEXT:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_MOVE_NEXT", _widget->getUIWidgetType(), _widget);
					}
				}
				break;
			}
			case cocos2d::ui::ListView::EventMovePage::ON_MOVE_BACK:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_MOVE_BACK", _widget->getUIWidgetType(), _widget);
					}
				}
				break;
			}
			case cocos2d::ui::ListView::EventMovePage::ON_MOVE_NONE:
			{
				break;
			}
			}
		}
		void LayerEntity::OnSelectListItem(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
			{
				break;
			}
			case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
			{
				ListViewWidget* listView = static_cast<ListViewWidget*>(_widget);
				CC_UNUSED_PARAM(listView);
				int current_index_item = static_cast<ListView*>(listView->GetResource())->getCurSelectedIndex();
				listView->SetCurrentIdxSelected(current_index_item);
				break;
			}
			default:
				break;
			}
		}

		void LayerEntity::OnTouchEvent(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case Widget::TouchEventType::BEGAN:
				switch (_widget->getUIWidgetType())
				{
				case UI_TYPE::UI_BUTTON:
				{
					auto btn = ((ButtonWidget*)_widget);
					if (btn->IsInteractive())
					{
						if (btn->GetIdSound())
						{
							GetSound->PlaySoundEffect(btn->GetIdSound()->id_click, btn->GetIdSound()->delayTime, btn->GetIdSound()->loop);
						}
					}
					break;
				}
				default:
					break;
				}
				break;

			case Widget::TouchEventType::MOVED:

				break;

			case Widget::TouchEventType::ENDED:
			{
				//auto btn = ((ButtonWidget*)_widget);
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName(), _widget->getUIWidgetType(), _widget);
					}
				}
			}
			break;

			case Widget::TouchEventType::CANCELED:

				break;

			default:
				break;
			}
		}

		void LayerEntity::OnPageViewEvent(cocos2d::Ref* sender, cocos2d::ui::PageView::EventType type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case PageView::EventType::TURNING:
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName(), _widget->getUIWidgetType(), _widget);
					}
				}
				break;
			}
		}

		void LayerEntity::OnSetCellTable(cocos2d::Ref* sender, EventType_SetCell type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case EventType_SetCell::ON_SET_CELL:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_SET_CELL", UI_TYPE::UI_LIST_VIEW_TABLE, _widget);
					}
				}
				break;
			}
			case EventType_SetCell::ON_END_DRAG_CELL:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_END_DRAG_CELL", UI_TYPE::UI_LIST_VIEW_TABLE, _widget);
					}
				}
			}
			case EventType_SetCell::ON_END_DROP_CELL:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName() + "_END_DROP_CELL", UI_TYPE::UI_LIST_VIEW_TABLE, _widget);
					}
				}
			}
			}
		}

		void LayerEntity::OnProgressTimer(cocos2d::Ref* sender, EventType_Process type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case EventType_Process::ON_TIMER_ZERO:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName(), _widget->getUIWidgetType(), _widget);
					}
				}

				break;
			}
			}

		}

		void LayerEntity::OnDigitalTimer(cocos2d::Ref* sender, EventType_Digital type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case EventType_Digital::ON_TIMER_ZERO:
			{
				if (_widget->IsInteractive())
				{
					if (menu_point_to)
					{
						func_call_widget(*menu_point_to, _widget->GetName(), _widget->getUIWidgetType(), _widget);
					}
				}

				break;
			}
			}
		}

		void LayerEntity::OnCheckBoxEvent(cocos2d::Ref* sender, cocos2d::ui::CheckBox::EventType type, WidgetEntity* _widget)
		{
			switch (type)
			{
				case ui::CheckBox::EventType::SELECTED:
				{
					if (_widget->IsInteractive())
					{
						if (menu_point_to)
						{
							func_call_widget(*menu_point_to, _widget->GetName() + "_SELECTED", _widget->getUIWidgetType(), _widget);
						}
					}
					break;
				}
				case ui::CheckBox::EventType::UNSELECTED:
				{
					if (_widget->IsInteractive())
					{
						if (menu_point_to)
						{
							func_call_widget(*menu_point_to, _widget->GetName() + "_UNSELECTED", _widget->getUIWidgetType(), _widget);
						}
					}
					break;
				}

			};
		}

		void LayerEntity::OnFinishAnimate(cocos2d::Ref* sender, EventType_Anim type, WidgetEntity* _widget)
		{
			switch (type)
			{
			case EventType_Anim::ON_FINISH_ACTION:
				{
					if (_widget->IsInteractive())
					{
						if (menu_point_to)
						{
							func_call_widget(*menu_point_to, _widget->GetName(), _widget->getUIWidgetType(), _widget);
						}
					}

					break;
				}
			}
		}

		void LayerEntity::SetCallBack( std::function<void(Screen&, RKString, UI_TYPE, WidgetEntity*)> func)
		{
			func_call_widget = func;
		}

		//the postion of layer not place at the center , it place at left bottom
		void LayerEntity::SetPosValueXML(xml::UILayerWidgetDec * xml_value)
		{
			if (!xml_value->FirstValue->HaveDataVector2("pos_ratio"))
			{
				return;
			}

			auto originPosRatio = p_parent_layer ? xml_value->OriginValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->OriginValue->GetDataVector2("pos_ratio"));
			auto firstPosRatio = p_parent_layer ? xml_value->FirstValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->FirstValue->GetDataVector2("pos_ratio"));

			auto posOrigin = Vec2(GetGameSize().width * originPosRatio.x, GetGameSize().height * originPosRatio.y);
			auto posFirst = Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y);

			PASSERT2(GetResource()->getContentSize().width != 0 || GetResource()->getContentSize().height != 0, "the content size of resource must bigger than 0");

			posFirst.x -= (GetResource()->getContentSize().width / 2);
			posFirst.y -= (GetResource()->getContentSize().height / 2);

			SetFirstPos(posFirst, xml_value->GeneralValue->GetDataInt("not_fade_pos") == 0);
		}

		void LayerEntity::SetInitPos(xml::UILayerWidgetDec * xml_value)
		{
			auto pos_ratio = p_parent_layer ? xml_value->OriginValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->OriginValue->GetDataVector2("pos_ratio"));
			auto pos = Vec2(GetGameSize().width * pos_ratio.x, GetGameSize().height * pos_ratio.y);

			pos.x -= (GetResource()->getContentSize().width / 2);
			pos.y -= (GetResource()->getContentSize().height / 2);

			SetPosition(pos);
			p_origin_value.ReplaceDataVector2("pos", pos);
		}

		void LayerEntity::SetActionProcessForAllChild(STATUS_MENU fade_state, unsigned int current_idx)
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				auto child = p_listWidgetChild.at(i);
				if (child->GetIndexAppear() != current_idx)
				{
					if (child->ContainChildIdxAppear(current_idx))
					{
						p_number_child_have_action += child->SetProcessActionChildIdxAppear(fade_state, current_idx);
					}

					continue;
				}
				bool res = child->SetActionProcess(fade_state);
				if (res)
				{
					p_number_child_have_action++;
				}
			}

			for (int j = 0; j < (int)p_listLayerChild.size(); j++)
			{
				auto child = p_listLayerChild.at(j);

				if (child->GetIndexAppear() != current_idx)
				{

					child->SetActionProcessForAllChild(fade_state, current_idx);
					continue;
				}
				bool res = child->SetActionProcess(fade_state);
				if (res)
				{
					p_number_child_have_action++;
				}
			}
		}

		bool LayerEntity::IsAllChildFinishAction()
		{
			return (p_number_child_have_action <= 0);
		}

		WidgetEntity * LayerEntity::GetWidgetChildAtIdx(int idx)
		{
			if (idx < (int)p_listWidgetChild.size() && p_listWidgetChild.size() > 0)
			{
				return p_listWidgetChild.at(idx);
			}
			return nullptr;
		}

		WidgetEntity * LayerEntity::GetWidgetChildByName(RKString name_path)
		{
			auto name_list = name_path.Split(".");

			auto layer_contain = GetLayerChildByName(name_list.GetAt(0));
			if (layer_contain)
			{
				RKString sub_name = "";
				for (int i = 1; i < (int)name_list.Size(); i++)
				{
					sub_name += name_list.GetAt(i);
					if (i != name_list.Size() - 1)
					{
						sub_name += ".";
					}
				}

				return layer_contain->GetWidgetChildByName(sub_name);
			}
			else
			{
				for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
				{
					auto widget_contain = p_listWidgetChild.at(i)->GetWidgetChildByName(name_path);
					if (widget_contain)
						return widget_contain;
				}
			}
			return nullptr;
		}

		LayerEntity	 * LayerEntity::GetLayerChildByName(RKString name_path)
		{
			auto name_list = name_path.Split(".");

			for (unsigned int i = 0; i < p_listLayerChild.size(); i++)
			{
				if (p_listLayerChild.at(i)->GetName() == name_list.GetAt(0))
				{
					if (name_list.Size() == 1 || name_list.GetAt(1) == "")
					{
						return p_listLayerChild.at(i);
					}
					else
					{
						RKString sub_name = "";
						for (unsigned int j = 1; j < name_list.Size(); j++)
						{
							sub_name += name_list.GetAt(j);
							if (j != name_list.Size() - 1)
							{
								sub_name += ".";
							}
						}
						return p_listLayerChild.at(i)->GetLayerChildByName(sub_name);
					}
				}
			}

			return 0;
		}

		bool LayerEntity::HaveChildWithIdxAppear(int checked_idx_appear)
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				auto child = p_listWidgetChild.at(i);
				if (child->GetIndexAppear() == checked_idx_appear)
				{
					return true;
				}
				else
				{
					if (child->ContainChildIdxAppear(checked_idx_appear) == true) { return true; }
				}
			}

			for (int j = 0; j < (int)p_listLayerChild.size(); j++)
			{
				auto child = p_listLayerChild.at(j);

				if (child->GetIndexAppear() != checked_idx_appear)
				{
					for (int i = 0; i < (int)child->GetListLayerChild().size(); i++)
					{
						bool value = child->GetListLayerChild().at(i)->HaveChildWithIdxAppear(checked_idx_appear);
						if (value)
							return value;
					}
				}
				else
				{
					return true;
				}
			}

			return false;
		}

		bool LayerEntity::UpdateActionProcessChild(float dt, int current_idx)
		{
			bool IsAllfinish = true;
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				auto child = p_listWidgetChild.at(i);
				if (child->GetIndexAppear() != current_idx)
				{
					continue;
				}
				IsAllfinish &= child->UpdateActionProcess(dt);
			}

			for (int j = 0; j < (int)p_listLayerChild.size(); j++)
			{
				auto child = p_listLayerChild.at(j);

				if (child->GetIndexAppear() != current_idx)
				{
					for (int i = 0; i < (int)(child->GetListLayerChild().size()); i++)
					{
						IsAllfinish &= child->GetListLayerChild().at(i)->UpdateActionProcessChild(dt, current_idx);
					}
					continue;
				}
				IsAllfinish &= child->UpdateActionProcess(dt);
			}

			return IsAllfinish;
		}
		
		void LayerEntity::RegisterUIWidget()
		{
			p_factory_ui_widget.RegisterProduct<ButtonWidget>(xml::TYPE_UI_WIDGET::WIDGET_BUTTON);
			p_factory_ui_widget.RegisterProduct<LayoutWidget>(xml::TYPE_UI_WIDGET::WIDGET_LAYOUT);
			p_factory_ui_widget.RegisterProduct<TextWidget>(xml::TYPE_UI_WIDGET::WIDGET_FONT);
			p_factory_ui_widget.RegisterProduct<PanelWidget>(xml::TYPE_UI_WIDGET::WIDGET_PANEL);
			p_factory_ui_widget.RegisterProduct<ScrollViewWidget>(xml::TYPE_UI_WIDGET::WIDGET_SCROLL_VIEW);
			p_factory_ui_widget.RegisterProduct<ListViewWidget>(xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW);
			p_factory_ui_widget.RegisterProduct<PageViewWidget>(xml::TYPE_UI_WIDGET::WIDGET_PAGE_VIEW);
			//p_factory_ui_widget.RegisterProduct<TextFieldWidget>(xml::TYPE_UI_WIDGET::WIDGET_TEXT_FIELD);
			p_factory_ui_widget.RegisterProduct<CheckBoxWidget>(xml::TYPE_UI_WIDGET::WIDGET_CHECK_BOX);
			p_factory_ui_widget.RegisterProduct<SliderWidget>(xml::TYPE_UI_WIDGET::WIDGET_SLIDER);
			p_factory_ui_widget.RegisterProduct<EditBoxWidget>(xml::TYPE_UI_WIDGET::WIDGET_EDIT_BOX);
			p_factory_ui_widget.RegisterProduct<PanelClippingWidget>(xml::TYPE_UI_WIDGET::WIDGET_PANEL_CLIPPING);
			p_factory_ui_widget.RegisterProduct<TimeProcessWidget>(xml::TYPE_UI_WIDGET::WIDGET_TIME_PROCESS);
			p_factory_ui_widget.RegisterProduct<LoadingBarWidget>(xml::TYPE_UI_WIDGET::WIDGET_LOADING_BAR);
			p_factory_ui_widget.RegisterProduct<RadioBtnWidget>(xml::TYPE_UI_WIDGET::WIDGET_RADIO_BTN);
			p_factory_ui_widget.RegisterProduct<ListViewTableWidget>(xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW_TABLE);
			p_factory_ui_widget.RegisterProduct<TimeDigitalWidget>(xml::TYPE_UI_WIDGET::WIDGET_TIME_DIGITAL);
			p_factory_ui_widget.RegisterProduct<AnimWidget>(xml::TYPE_UI_WIDGET::WIDGET_ANIM);
			p_factory_ui_widget.RegisterProduct<ParticleWidget>(xml::TYPE_UI_WIDGET::WIDGET_PARTICLE);
		}

		void LayerEntity::SetInteractive(bool value)
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				p_listWidgetChild.at(i)->SetInteractive(value);
			}
			
			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->SetInteractive(value);
			}
			ActionProcess::SetInteractive(value);
		}

		void LayerEntity::SetDisable(bool value)
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				p_listWidgetChild.at(i)->SetDisable(value);
			}

			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->SetDisable(value);
			}
			ActionProcess::SetDisable(value);
		}

		void LayerEntity::ReloadText()
		{
			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				p_listWidgetChild.at(i)->ReloadText();
			}

			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->ReloadText();
			}
		}

		void LayerEntity::ClearChild(int count)
		{

			for (int i = 0; i < (int)p_listWidgetChild.size(); i++)
			{
				p_listWidgetChild.at(i)->ClearChild();
			}

			for (int i = 0; i < (int)p_listLayerChild.size(); i++)
			{
				p_listLayerChild.at(i)->ClearChild(count);
			}

			ReleaseChildWidget();

			p_listWidgetChild.clear();
			p_listLayerChild.clear();
		}
	}
}