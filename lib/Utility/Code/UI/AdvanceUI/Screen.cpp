#include "Screen.h"
#include "../../InputManager.h"
#include "../../SoundManager.h"

namespace Utility
{
	namespace UI_Widget
	{

		UIDecs::UIDecs()
		{
			p_curStatus = STATUS_MENU::M_IDLE;
			p_IsActive = false;
			p_SoundFadeIn = "";
			p_SoundFadeOut = "";

			p_delayTime = 0;
			p_loop = 0;

		}

		UIDecs::UIDecs(RKString sou_1, RKString sou_2, bool IsActive)
		{
			p_curStatus = STATUS_MENU::M_IDLE;
			p_IsActive = IsActive;
			p_SoundFadeIn = sou_1;
			p_SoundFadeOut = sou_2;

			p_delayTime = 0;
			p_loop = 0;
		}

		UIDecs::UIDecs(RKString sou_1, RKString sou_2, bool IsActive, float delayTime, int loop)
		{
			p_curStatus = STATUS_MENU::M_IDLE;
			p_IsActive = IsActive;
			p_SoundFadeIn = sou_1;
			p_SoundFadeOut = sou_2;

			p_delayTime = delayTime;
			p_loop = loop;
		}


		Screen::Screen()
		{
			p_IsStopUpdate = false;

			p_DeltaMoveTime = 0;

			p_IdxMenuData = -1;
			p_IdxUITagAppear = 0;

			p_time_process = 0;
			p_max_idx_process_first = 0;

			name_ui = "";

			p_listLayerWidget.clear();
			p_ListStatus.clear();

			p_IsMenuInteractive = true;

			p_design_size = Vec2(0, 0);
            
            p_stage_show_keyboard = 0;
            p_textfield_infocus = nullptr;
		}


		Screen::~Screen()
		{
			for (int i = 0; i < (int)p_ListStatus.size(); i++)
			{
				delete p_ListStatus.at(i);
			}
			p_ListStatus.clear();
                    
            for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
            {
                p_listLayerWidget.at(i)->GetResource()->release();
            }
            p_listLayerWidget.clear();
            
            p_textfield_infocus = nullptr;

			removeAllChildren();
        }

		void Screen::onDrawMenuWidget(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				p_listLayerWidget.at(i)->Draw(renderer, transform, flags);
			}

		}

		void Screen::onVisitMenuWidget(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				p_listLayerWidget.at(i)->Visit(renderer, transform, flags);
			}

		}

		void Screen::onUpdateMenuWidget(float dt)
		{
			if (p_IsStopUpdate)
			{
				return;
			}

			OnUpdateTouchMenu();

			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				if (p_listLayerWidget.at(i)->HaveActionNotFade())
				{
					p_listLayerWidget.at(i)->UpdateActionProcess(dt);
				}

				p_listLayerWidget.at(i)->Update(dt);
			}
            
            HandleMoveKeyboard();

			if ((p_IdxUITagAppear >= 0 && GetCurrentStatus(p_IdxUITagAppear) == STATUS_MENU::M_FADEIN) ||
				(p_IdxUITagAppear == 0 && GetCurrentStatus(p_IdxUITagAppear) == STATUS_MENU::M_FADEOUT))
			{
				bool res = OnUpdateFadeProcessChildByTagIdx(p_IdxUITagAppear,
					XMLMgr->GetUIWidgetByName(name_ui)->GetTimeProcess(p_IdxUITagAppear).time_process,
					dt,
					GetCurrentStatus(p_IdxUITagAppear));
			}
			else if (p_IdxUITagAppear > 0)
			{
				if (GetCurrentStatus(0) == STATUS_MENU::M_FADEOUT)
				{
					for (int i = 1; i < (int)p_ListStatus.size(); i++)
					{
						if (p_ListStatus.at(i)->p_IsActive)
						{
							bool res = OnUpdateFadeProcessChildByTagIdx(i,
								XMLMgr->GetUIWidgetByName(name_ui)->GetTimeProcess(i).time_process,
								dt,
								GetCurrentStatus(i));
						}
					}
				}
				else if (GetCurrentStatus(0) == STATUS_MENU::M_IDLE)
				{
					bool res = OnUpdateFadeProcessChildByTagIdx(p_IdxUITagAppear,
						XMLMgr->GetUIWidgetByName(name_ui)->GetTimeProcess(p_IdxUITagAppear).time_process,
						dt,
						GetCurrentStatus(p_IdxUITagAppear));
				}

			}
		}

		RKString Screen::GetNameUI() 
		{ 
			return name_ui; 
		}

		void Screen::SetIdxMenuData(int idx) 
		{ 
			p_IdxMenuData = idx; 
		}

		int Screen::GetMaxIdxFirst()
		{ 
			return p_max_idx_process_first;
		}

		bool Screen::IsInteractive()
		{ 
			return p_IsMenuInteractive; 
		}

		Vec2 Screen::GetDesignSize()
		{ 
			return p_design_size; 
		}

		void Screen::OnCheckTouchEvent(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
		{
			if (GetCurrentStatus() != STATUS_MENU::M_IDLE)
				return;

			//only can touch menu if it lays on the top of the menu ui stack 
			if (p_IdxMenuData != 0)
				return;

			//in some case the idxtagAppear have 1 , 2... so wait all the menu appear then can press button

			OnProcessWidgetChild(name, type_widget, _widget);
		}

		LayerEntity *  Screen::InitTheChildElement(RKString str, LayerEntity * parent_layer, xml::UILayerWidgetDec * layer)
		{
			//
			if (str == "" || layer == nullptr)
				return nullptr;

			RKString name_res = str;

			LayerEntity * _layer_return = new LayerEntity();
			_layer_return->SetParentLayer(parent_layer);
			_layer_return->InitParam(layer->NameWidget, layer);
			_layer_return->SetValueFromXML(layer);
			_layer_return->SetMenuParent(this);
			_layer_return->SetCallBack(std::mem_fn(&Screen::OnCheckTouchEvent));
			_layer_return->setCascadeOpacityEnabled(true);
			_layer_return->SetRootLayer(layer->GeneralValue->GetDataInt("root") == 1);
			for (int i = 0; i < (int)(layer->p_list_child_widget.size()); i++)
			{
				auto widget_type = layer->p_list_child_widget.at(i)->typeValue;
				switch (widget_type)
				{
				case xml::TYPE_UI_WIDGET::WIDGET_LAYER:
				{
					auto widget_layer_child = InitTheChildElement((layer->p_list_child_widget.at(i))->NameWidget, _layer_return, layer->p_list_child_widget.at(i));
					_layer_return->AddLayerEntity(widget_layer_child);
					break;
				}
				case xml::TYPE_UI_WIDGET::WIDGET_LAYOUT:
				case xml::TYPE_UI_WIDGET::WIDGET_BUTTON:
				case xml::TYPE_UI_WIDGET::WIDGET_PANEL:
				case xml::TYPE_UI_WIDGET::WIDGET_FONT:
				case xml::TYPE_UI_WIDGET::WIDGET_CHECK_BOX:
				case xml::TYPE_UI_WIDGET::WIDGET_LOADING_BAR:
				case xml::TYPE_UI_WIDGET::WIDGET_TEXT_FIELD:
				case xml::TYPE_UI_WIDGET::WIDGET_SLIDER:
				case xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW:
				case xml::TYPE_UI_WIDGET::WIDGET_SCROLL_VIEW:
				case xml::TYPE_UI_WIDGET::WIDGET_PAGE_VIEW:
				case xml::TYPE_UI_WIDGET::WIDGET_RADIO_BTN:
				case xml::TYPE_UI_WIDGET::WIDGET_EDIT_BOX:
				case xml::TYPE_UI_WIDGET::WIDGET_PANEL_CLIPPING:
				case xml::TYPE_UI_WIDGET::WIDGET_TIME_PROCESS:
				case xml::TYPE_UI_WIDGET::WIDGET_LIST_VIEW_TABLE:
				case xml::TYPE_UI_WIDGET::WIDGET_TIME_DIGITAL:
				case xml::TYPE_UI_WIDGET::WIDGET_ANIM:
				case xml::TYPE_UI_WIDGET::WIDGET_PARTICLE:
				{
					auto widget_entity = _layer_return->InitWidgetEntity(layer->p_list_child_widget.at(i), _layer_return, nullptr);
					_layer_return->AddWidgetEntity(widget_entity);
					break;
				}
				case xml::TYPE_UI_WIDGET::WIDGET_NONE:
				case xml::TYPE_UI_WIDGET::WIDGET_COUNT:
					PASSERT2(false, "ERROR : wrong parameter");
					break;
				default:
					break;
				}
			}
			return _layer_return;

		}

		void Screen::InitMenuWidgetEntityWithParam(RKString name, Vec2 design_size)
		{
			name_ui = name;
			p_design_size = design_size;
			p_max_idx_process_first = 0;
			p_time_process = 0.5f;

			p_ListStatus.push_back(
				new UIDecs("",
					"",
					true ,
					0.05f,
					1));
		}

		void Screen::InitMenuWidgetEntity(RKString str)
		{
			name_ui = str;

			auto UIDec = XMLMgr->GetUIWidgetByName(name_ui);
			auto listElementName = UIDec->GetListChildName();
            setAnchorPoint(Vec2(0,0));
			p_time_process = UIDec->GetTimeProcess(0).time_process;
			p_max_idx_process_first = UIDec->max_idx_first;
			p_design_size = UIDec->DesignSize;
			//
			for (int i = 0; i < (int)listElementName.size(); i++)
			{
				auto widget_root = InitTheChildElement(listElementName[i], nullptr, UIDec->GetElementChildByName(listElementName[i]));
				p_listLayerWidget.push_back(widget_root);
				this->addChild(widget_root);
			}
			//
			int number_time_process = UIDec->GetNumberTimeProcess();
			for (int i = 0; i < number_time_process; i++)
			{
				p_ListStatus.insert(p_ListStatus.begin() + i,
					new UIDecs(UIDec->GetTimeProcess(i).ID_SOUND_FADE_IN,
						UIDec->GetTimeProcess(i).ID_SOUND_FADE_OUT,
						i == 0 ? true : false,
						UIDec->GetTimeProcess(i).delayTime,
						UIDec->GetTimeProcess(i).loop));
			}

		}

		void Screen::OnReloadString()
		{
			/*for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
			if (p_ListMemberInGame.GetAt(i)->GetType() != UI_MENU_TYPE::MENU_TEXT)
			continue;
			auto txt = (TextEntity*)(p_ListMemberInGame.GetAt(i)->GetResource());
			txt->ReloadText();
			}*/
		}

		bool Screen::OnSetActionFadeAllResource(STATUS_MENU stateFade, unsigned int idx_tag_appear)
		{
			bool have_value = false;
			SetStatusAtIdx(stateFade, idx_tag_appear);
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				if (p_listLayerWidget.at(i)->GetIndexAppear() != idx_tag_appear)
				{
					have_value |= p_listLayerWidget.at(i)->HaveChildWithIdxAppear(idx_tag_appear);

					if (have_value &&
						(idx_tag_appear > 0 || stateFade == STATUS_MENU::M_FADEOUT) &&
						p_ListStatus.at(idx_tag_appear)->p_curStatus != STATUS_MENU::M_IDLE)
					{
						//set action for all child widget
						p_listLayerWidget.at(i)->SetActionProcessForAllChild(p_ListStatus.at(idx_tag_appear)->p_curStatus, idx_tag_appear);
					}

					continue;
				}
				have_value = true;
				p_listLayerWidget.at(i)->SetActionProcess(stateFade);
			}

			if (have_value)
			{
				p_DeltaMoveTime = XMLMgr->GetUIWidgetByName(name_ui)->GetTimeProcess(idx_tag_appear).time_process;
			}
			return have_value;
		}

		bool Screen::OnUpdateFadeProcessChildByTagIdx(unsigned int idx_tag_appear, float process_time, float dt, STATUS_MENU stateFade)
		{
			if (p_DeltaMoveTime == process_time)
			{
#if USE_CC_AUDIO
				if (stateFade == STATUS_MENU::M_FADEIN)
				{
					GetSound->PlaySoundEffect(p_ListStatus.at(idx_tag_appear)->p_SoundFadeIn,
						p_ListStatus.at(idx_tag_appear)->p_delayTime,
						p_ListStatus.at(idx_tag_appear)->p_loop);
				}
				if (stateFade == STATUS_MENU::M_FADEOUT)
				{
					GetSound->PlaySoundEffect(p_ListStatus.at(idx_tag_appear)->p_SoundFadeOut,
						p_ListStatus.at(idx_tag_appear)->p_delayTime,
						p_ListStatus.at(idx_tag_appear)->p_loop);
				}
#endif
			}
			p_DeltaMoveTime -= dt;
			if (p_DeltaMoveTime < 0)
			{
				p_DeltaMoveTime = 0;
			}

			bool AllFinish = true;
			//
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				if (p_listLayerWidget.at(i)->GetIndexAppear() != idx_tag_appear)
				{
					AllFinish &= p_listLayerWidget.at(i)->UpdateActionProcessChild(dt, idx_tag_appear);
					continue;
				}
				AllFinish &= p_listLayerWidget.at(i)->UpdateActionProcess(dt);
			}

			if (AllFinish)
			{
				if (GetCurrentStatus(idx_tag_appear) == STATUS_MENU::M_FADEOUT)
				{
					OnFinishFadeAtCurrentIdx(idx_tag_appear, STATUS_MENU::M_FADEOUT);

					SetStatusAtIdx(STATUS_MENU::M_IDLE, idx_tag_appear);
					//
					if (idx_tag_appear <= 0)
					{
						OnFadeOutComplete();

						//remove from main scene
						remove_layer_from_main_scene();

						//clear all the fade process tag when deactive an ui menu
						RemoveActionFadeAllChild(idx_tag_appear);

						p_DeltaMoveTime = 0;

						OnDeactiveCurrentMenu();
					}
				}
				else if (GetCurrentStatus(idx_tag_appear) == STATUS_MENU::M_FADEIN)
				{
					OnFinishFadeAtCurrentIdx(idx_tag_appear, STATUS_MENU::M_FADEIN);

					SetStatusAtIdx(STATUS_MENU::M_IDLE, idx_tag_appear);

					if (idx_tag_appear == p_max_idx_process_first)
					{
						OnFadeInComplete();
						p_DeltaMoveTime = 0;
					}
				}

				if (p_IdxUITagAppear > 0 && GetCurrentStatus(p_IdxUITagAppear) == STATUS_MENU::M_IDLE)
				{
					p_IdxUITagAppear = 0;
				}
			}

			return AllFinish;
		}

		void Screen::RemoveActionFadeAllChild(int idx)
		{
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				if (p_listLayerWidget.at(i)->GetIndexAppear() != idx)
					continue;
				p_listLayerWidget.at(i)->RemoveActionByTag(true);
			}
		}

		STATUS_MENU Screen::GetCurrentStatus(int idx)
		{
			if (idx >= 0 && idx < (int)p_ListStatus.size())
				return p_ListStatus.at(idx)->p_curStatus;
			return STATUS_MENU::M_IDLE;
		}

		void Screen::SetStatusAtIdx(STATUS_MENU status, int idx)
		{
			if (idx >= 0 && idx < (int)p_ListStatus.size())
			{
				auto status__ = p_ListStatus.at(idx);
				p_ListStatus.erase(p_ListStatus.begin() + idx);
				status__->p_curStatus = status;
				p_ListStatus.insert(p_ListStatus.begin() + idx, status__);
			}
		}

		bool Screen::GetActiveStatusAtidx(int idx)
		{
			if (idx >= 0 && idx < (int)p_ListStatus.size())
				return p_ListStatus.at(idx)->p_IsActive;
			return STATUS_MENU::M_IDLE;
		}

		void Screen::SetActiveStatusAtIdx(bool active, int idx)
		{
			if (idx >= 0 && idx < (int)p_ListStatus.size())
			{
				auto status__ = p_ListStatus.at(idx);
				if (status__->p_IsActive == active)
					return;
				p_ListStatus.erase(p_ListStatus.begin() + idx);
				status__->p_IsActive = active;
				p_ListStatus.insert(p_ListStatus.begin() + idx, status__);
			}
		}

		void Screen::OnFadeIn()
		{
			SetStatusAtIdx(STATUS_MENU::M_FADEIN, 0);
			OnSetActionFadeAllResource(GetCurrentStatus());

			OnBeginFadeIn();

			push_layer_to_main_scene();
		}

		void Screen::OnFadeOut()
		{
			int maximum_idx_appear = 0;
			for (int i = 0; i < (int)p_ListStatus.size(); i++)
			{
				if (p_ListStatus.at(i)->p_IsActive)
				{
					SetStatusAtIdx(STATUS_MENU::M_FADEOUT, i);
					if (i >= (p_ListStatus.size() > 1 ? 1 : 0))
					{
						OnSetActionFadeAllResource(GetCurrentStatus(i), i);
						maximum_idx_appear = i;
					}
				}
			}
			OnBeginFadeOut();

			p_IdxUITagAppear = maximum_idx_appear; //set maximum index
		}

		void Screen::OnShow()
		{
			OnFadeIn();

		}

		void Screen::OnHide()
		{
			//
			OnFadeOut();
		}

		WidgetEntity * Screen::GetWidgetChildByName(RKString name_path)
		{
			auto name_list = name_path.Split(".");
			auto layer_contain = Screen::GetLayerChildByName(name_list.GetAt(0));
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
			return nullptr;
		}

		LayerEntity* Screen::GetLayerChildByName(RKString name_path)
		{
			auto name_list = name_path.Split(".");
			for (unsigned int i = 0; i < p_listLayerWidget.size(); i++)
			{
				if ((!name_path.Contains(".") && p_listLayerWidget.at(i)->GetName() == name_path) ||
					(p_listLayerWidget.at(i)->GetName() == name_list.GetAt(0)))
				{
					if (name_list.Size() == 1 || name_list.GetAt(1) == "")
					{
						return p_listLayerWidget.at(i);
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

						return p_listLayerWidget.at(i)->GetLayerChildByName(sub_name);
					}
				}
			}
			return nullptr;
		}

		void Screen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
		{
			//the abstrast class may be not implement, which class use, that class implement ex: SelectionMode UI
		}

		void Screen::OnProcessWhenEndOrBegin(STATUS_MENU cur_status)
		{
			if (p_IdxUITagAppear == p_max_idx_process_first)
			{
				p_IdxUITagAppear = 0;
				return;
			}

			if (cur_status == M_FADEIN) //fade in
			{
				p_IdxUITagAppear++;
			}
			else //fade out
			{
				p_IdxUITagAppear = 0;
				return;
			}
			bool res = OnSetActionFadeAllResource(cur_status, p_IdxUITagAppear);
			if (!res)
			{
				p_IdxUITagAppear = 0;
			}
		}

		void Screen::OnProcessWhenTagIdxMulti(int max_idx, int idx, STATUS_MENU cur_status, STATUS_MENU status)
		{
			if (status == STATUS_MENU::M_FADEIN) //fade in
			{
				if (idx < max_idx)
				{
					p_IdxUITagAppear++;
					bool res = OnSetActionFadeAllResource(cur_status, p_IdxUITagAppear);
					if (!res)
					{
						//p_IsCanPressButton = true;
						p_IdxUITagAppear = 0;
					}
				}
				else if (idx >= max_idx)
				{
					//p_IsCanPressButton = true;
					p_IdxUITagAppear = 0;
				}

				SetActiveStatusAtIdx(true, idx);
			}
			else
			{
				SetActiveStatusAtIdx(false, idx);

				bool IsAllDeActive = true;
				if (p_ListStatus.size() > 1)
				{
					for (int i = max_idx < 1 ? 0 : 1; i < (int)p_ListStatus.size(); i++)
					{
						if (p_ListStatus.at(i)->p_IsActive == true)
						{
							IsAllDeActive = false;
							return;
						}
					}
				}
				if (IsAllDeActive)
				{
					p_IdxUITagAppear = 0;
					bool res = OnSetActionFadeAllResource(cur_status, p_IdxUITagAppear);
					if (!res)
					{
						p_IdxUITagAppear = 0;
					}
				}
			}
		}

		void Screen::OnAllProcessFinishFadeIdx(int idx, int max_idx, STATUS_MENU status)
		{
			STATUS_MENU cur_status = GetCurrentStatus(idx);
			if (idx == 0 && cur_status != STATUS_MENU::M_IDLE /*&& idx < max_idx*/)
			{
				OnProcessWhenEndOrBegin(cur_status);
			}
			else if ((idx >= 1 && cur_status != STATUS_MENU::M_IDLE) /*|| idx <= max_idx*/)
			{
				OnProcessWhenTagIdxMulti(max_idx, idx, cur_status, status);
			}

		}

		bool Screen::OnCallProcessOfChild(RKString name_child_path, RKString command_str, int type)
		{
			ActionProcess * object_layer = nullptr;
			if (type == 0) //call layer
			{
				object_layer = static_cast<ActionProcess*>(GetLayerChildByName(name_child_path));
				if (!object_layer)
				{
					type = 1;
				}
			}

			if (type > 0) //call widget
			{
				object_layer = static_cast<ActionProcess*>(GetWidgetChildByName(name_child_path));
				if (!object_layer)
				{
					PWARN1("WARNING : CHILD NOT FOUND: %s", name_child_path.GetString());
					return false;
				}
			}

			p_IdxUITagAppear = object_layer->GetIndexAppear();
			STATUS_MENU stt = M_IDLE;
			if (command_str == "FADE_IN")
			{
				stt = M_FADEIN;
			}
			else if (command_str == "FADE_OUT")
			{
				stt = M_FADEOUT;
			}
			bool val = object_layer->SetActionCommandWithName(command_str);
			if (val)
			{
				object_layer->SetActionProcess(stt, true);
				p_DeltaMoveTime = XMLMgr->GetUIWidgetByName(name_ui)->GetTimeProcess(p_IdxUITagAppear).time_process;
				SetStatusAtIdx(stt, p_IdxUITagAppear);

			}

			return val;
		}


		void Screen::OnShowOrHideIdxTarget(int idx_target)
		{
			if (idx_target >= (int)p_ListStatus.size() || GetCurrentStatus(idx_target) != STATUS_MENU::M_IDLE)
			{
				//p_IdxUITagAppear = 0;
				return;
			}
			p_IdxUITagAppear = idx_target;
			STATUS_MENU stt = M_IDLE;
			if (GetActiveStatusAtidx(p_IdxUITagAppear) == false)
			{
				stt = M_FADEIN;
			}
			else
			{
				stt = M_FADEOUT;
			}
			bool res = OnSetActionFadeAllResource(stt, p_IdxUITagAppear);
			if (!res)
			{
				p_IdxUITagAppear = 0;
			}
		}

		void Screen::OnBeginFadeIn()
		{
			//abstrast class not impelemt this 
			return;
		}

		void Screen::OnFadeInComplete()
		{
			SetLayerInteractive(true);
			//abstrast class not impelemt this 
			return;
		}

		void Screen::OnBeginFadeOut()
		{
			SetLayerInteractive(false);
			//abstrast class not impelemt this 
			return;
		}

		void Screen::OnFadeOutComplete()
		{
			//abstrast class not impelemt this 
			return;
		}

		void Screen::onSpecificSizeChange(float cur_width, float cur_height)
		{
			//abstrast class not impelemt this 
			return;
		}

		void Screen::push_layer_to_main_scene(int order)
		{
			Scene * scene = Director::getInstance()->getRunningScene();
			if (order == 0)
			{
				order = scene->getChildrenCount();
			}
			scene->addChild(this, order);
		}

		void Screen::remove_layer_from_main_scene()
		{
			Scene * scene = Director::getInstance()->getRunningScene();
            this->stopAllActions();
			scene->removeChild(this);
		}

		void Screen::SetLayerInteractive(bool value)
		{
			p_IsMenuInteractive = value;
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				p_listLayerWidget.at(i)->SetInteractive(value);
			}
		}

		void Screen::SetChildInteractive(RKString name_path, bool value)
		{
			auto layer_child = GetLayerChildByName(name_path);
			if (!layer_child)
			{
				auto widget_child = GetWidgetChildByName(name_path);
				if (widget_child)
				{
					widget_child->SetInteractive(value);
				}
				else
				{
					PWARN1("WARNING ! CHILD NOT FOUND TO SET VALUE : %s", name_path.GetString())
				}
			}
			else
			{
				layer_child->SetInteractive(value);
			}
		}

		void Screen::OnTouchMenuBegin(cocos2d::Point p)
		{
		}
		void Screen::OnTouchMenuEnd(cocos2d::Point p)
		{
		}

		void Screen::OnUpdateTouchMenu()
		{
			//when fade in or out do not process touch button
			if (GetCurrentStatus() != STATUS_MENU::M_IDLE)
				return;

			//only can touch menu if it lays on the top of the menu ui stack 
			if (p_IdxMenuData != 0)
				return;

			if (InputMgr->GetNumberTouch() > 0 && InputMgr->GetStateTouchOnScreen() == Control::STATE_TOUCH::STATE_PRESS)
			{
				auto p = InputMgr->GetPosAtId(0);
				OnTouchMenuBegin(p);
			}
			else if (InputMgr->GetStateTouchOnScreen() == Control::STATE_TOUCH::STATE_RELEASE)
			{
				auto p = InputMgr->GetPosAtIdRelease(0);
				OnTouchMenuEnd(p);
			}
		}

		void Screen::RemoveChildWidget(WidgetEntity * widget)
		{
			LayerEntity * layer_root = GetRoot();

			if (!layer_root)
			{
				PASSERT2(layer_root != nullptr, "layer root not found !");
				return;
			}
			bool result = layer_root->RemoveWidgetEntity(widget);
			PASSERT2(result == true, "warning ! wiget cannot be deleted !");
		}

		void Screen::RemoveChildWidgetByName(RKString name)
		{
			LayerEntity * layer_root = GetRoot();

			if (!layer_root)
			{
				PASSERT2(layer_root != nullptr, "layer root not found !");
				return;
			}
			bool result = layer_root->RemoveWigetEnityByName(name);
			PASSERT2(result == true, "warning ! wiget cannot be deleted !");
		}

		LayerEntity* Screen::GetRoot()
		{
			LayerEntity * layer_root = nullptr;
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				if (p_listLayerWidget.at(i)->IsRootLayer())
				{
					layer_root = p_listLayerWidget.at(i);
					break;
				}
			}
			if (p_listLayerWidget.size() == 0)
			{
				layer_root = new LayerEntity();
				layer_root->SetParentLayer(nullptr);
				layer_root->setContentSize(GetGameSize());
				layer_root->setAnchorPoint(Vec2(0.5f, 0.5f));
				layer_root->SetPosition(GetGameSize() / 2.f);
				layer_root->SetMenuParent(this);
				layer_root->SetCallBack(std::mem_fn(&Screen::OnCheckTouchEvent));
				layer_root->setCascadeOpacityEnabled(true);
				layer_root->SetRootLayer(true);
				layer_root->SetIndexAppear(0);
				p_listLayerWidget.push_back(layer_root);
			}
			else if (p_listLayerWidget.size() == 1)
			{
				layer_root = p_listLayerWidget.at(0);
			}
			else if (p_listLayerWidget.size() == 2)
			{
				layer_root = p_listLayerWidget.at(1);
			}
			PASSERT2(layer_root != nullptr, "layer root not found !");
			return layer_root;
		}

		void Screen::AddChildWidget(WidgetEntity * widget_entity)
		{
			LayerEntity * layer_root = GetRoot();

			if (!layer_root)
			{
				PASSERT2(layer_root != nullptr, "layer root not found !");
				return;
			}
			PASSERT2(widget_entity->GetName() != "", "widget child must have name!");
			widget_entity->SetParentLayer(layer_root);
			widget_entity->SetMenuParent(this);
			widget_entity->SetParentWidget(nullptr);
			layer_root->SetIndexAppear(layer_root->GetIndexAppear());
			layer_root->AddWidgetEntity(widget_entity);
		}

		void Screen::RegisterKeyTouch()
		{
            return;
			auto listener_key = EventListenerKeyboard::create();
			listener_key->onKeyPressed = CC_CALLBACK_2(Screen::onKeyPressed, this);
			listener_key->onKeyReleased = CC_CALLBACK_2(Screen::onKeyReleased, this);

			_eventDispatcher->addEventListenerWithSceneGraphPriority(listener_key, this);

			auto listener = EventListenerTouchAllAtOnce::create();
			listener->onTouchesBegan = CC_CALLBACK_2(Screen::onTouchesBegan, this);
			listener->onTouchesMoved = CC_CALLBACK_2(Screen::onTouchesMoved, this);
			listener->onTouchesEnded = CC_CALLBACK_2(Screen::onTouchesEnded, this);
			_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		}

		void Screen::onKeyHold(float interval)
		{

		}
		void Screen::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
		{

		}
		void Screen::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
		{

		}

		void Screen::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{

		}

		void Screen::onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{

		}

		void Screen::onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{

		}

		void Screen::onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event  *event)
		{

		}

		void Screen::onLanguageChanged(LANGUAGE_TYPE lang)
		{
			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				p_listLayerWidget.at(i)->ReloadText();
			}
		}
        
        bool Screen::MoveScreenUpWhenKeyboardAppear(cocos2d::Rect rect_text_field)
        {
            auto rect_keyboard = GetSizeKeyboard();
            
            if(rect_keyboard.equals(cocos2d::Rect::ZERO))
            {
                return false;
            }
            
            Vec2 top_right = Vec2(rect_text_field.getMaxX() , rect_text_field.getMaxY());
            top_right = convertToWorldSpace(top_right);
            float delta_move = 0;
            rect_keyboard.size.height *= 1.2f;
            if(rect_keyboard.containsPoint(top_right))
            {
                float max_y_key =rect_keyboard.getMaxY();
                delta_move = abs(max_y_key - top_right.y) + rect_text_field.size.height * 1.2f;
            }
            else{
                return false;
            }
            Vec2 current_position = getPosition();
            current_position.y +=delta_move;
            setPosition(current_position);
            
            return true;
        }
        
        bool Screen::ResetScreenWhenKeyboardHide()
        {
          //  auto rect_keyboard = GetSizeKeyboard();
          //
          //  if(!rect_keyboard.equals(cocos2d::Rect::ZERO))
          //  {
           //     return false;
          //  }
            setPosition(0,0);
            
            return true;
        }
        
        void Screen::SetStageShowKeyboard(int stage , WidgetEntity * text_field )
        {
#ifdef _WIN32
            return;
#endif
            p_stage_show_keyboard = stage;
            p_textfield_infocus = text_field;
        }
        
        void Screen::HandleMoveKeyboard()
        {
            if(p_textfield_infocus == nullptr)
                return;
            if(p_stage_show_keyboard == 0)
                return;
            
            cocos2d::Rect rect = cocos2d::Rect(p_textfield_infocus->GetPosition_BottomLeft(),cocos2d::Size(p_textfield_infocus->GetSize().x,p_textfield_infocus->GetSize().y));
            if(p_stage_show_keyboard == 1)
            {
                bool res = MoveScreenUpWhenKeyboardAppear(rect);
                if(res)
                {
                    p_stage_show_keyboard = 0;
                    p_textfield_infocus = nullptr;
                }
            }
            if(p_stage_show_keyboard == 2)
            {
                bool res = ResetScreenWhenKeyboardHide();
                if(res)
                {
                    p_stage_show_keyboard = 0;
                    p_textfield_infocus = nullptr;
                }
            }
        }

		void Screen::ReloadUI(RKString path)
		{
			for (int i = 0; i < (int)p_ListStatus.size(); i++)
			{
				delete p_ListStatus.at(i);
			}
			p_ListStatus.clear();

			for (int i = 0; i < (int)p_listLayerWidget.size(); i++)
			{
				p_listLayerWidget.at(i)->GetResource()->release();
			}
			p_listLayerWidget.clear();

			p_textfield_infocus = nullptr;
			removeAllChildren();
			
			XMLMgr->ReloadUIMEnuWidgetDec(name_ui, path);
		}

		void Screen::SetStopUpdate(bool value)
		{
			p_IsStopUpdate = value;
		}
	}
}