#include "WidgetEntity.h"
#include "LayerEntity.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		WidgetEntity::WidgetEntity()
		{
			p_Resource = nullptr;

			p_typeUI = UI_TYPE::UI_NONE;

			p_listChildWidget.clear();

			p_isScale9Enable = false;

			p_stage_scale9 = 0;
			//p_xml_value = nullptr;

			p_mark_as_delete_resource = true;

			p_z_order = 0;
			//
			p_min_size_scale = 650;
			p_aligne_res.clear();
			p_state_aligne = true;
			ID_SOUND = nullptr;
		}

		WidgetEntity::~WidgetEntity()
		{
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();


			if (p_Resource && p_mark_as_delete_resource )
			{
				p_Resource->stopAllActions();
				p_Resource->removeAllChildren();
			}
			p_aligne_res.clear();
			//dont delete it, it the weak pointer
			p_Resource = nullptr;
			//
			if (ID_SOUND)
			{
				delete ID_SOUND;
				ID_SOUND = nullptr;
			}
		}

		cocos2d::Node * WidgetEntity::GetResource() {
			return p_Resource;
		}

		bool WidgetEntity::SetNewResource(cocos2d::Node * resource)
		{
			if (p_Resource)
			{
				p_Resource = static_cast<Widget*>(resource);
			}
			else
			{
				return false;
			}

			return true;
		}

		void WidgetEntity::CopyValue(WidgetEntity * data)
		{
			p_typeUI = data->p_typeUI;
			p_isScale9Enable = data->p_isScale9Enable;
			p_stage_scale9 = data->p_stage_scale9;
			p_mark_as_delete_resource = data->p_mark_as_delete_resource;
			p_z_order = data->p_z_order;
			p_min_size_scale = data->p_min_size_scale;
			if (data->p_Resource)
			{
				p_Resource = data->p_Resource->clone();
			}
			if (data->ID_SOUND)
			{
				ID_SOUND = new xml::sound_click_dec(data->ID_SOUND);
			}
		}

		/*xml::sound_click_dec* WidgetEntity::GetIdSound() 
		{ 
			return ID_SOUND; 
		}*/

		void WidgetEntity::CloneChild(WidgetEntity * p_clone)
		{
			//PASSERT2(p_clone->p_listChildWidget.size() == p_listChildWidget.size(), "number child of 2 object must be equal");
			//remove all child if exist 
			p_clone->SetParentWidget(this->GetParentWidget());
			p_clone->GetResource()->removeAllChildren();
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				auto child_widget = p_listChildWidget.at(i)->Clone();
				child_widget->SetParentWidget(p_clone);
				child_widget->SetParentLayer(GetParentLayer());
				child_widget->SetMenuParent(GetMenuParent());
				p_clone->p_listChildWidget.push_back(child_widget);
				p_clone->GetResource()->setName(child_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
				if (child_widget->p_z_order == 0)
				{
					child_widget->p_z_order = p_listChildWidget.size();
				}
				p_clone->GetResource()->setLocalZOrder(p_z_order);
				//p_clone->SetParentLayer(this->GetParentLayer());
				p_clone->GetResource()->addChild(child_widget->GetResource(), child_widget->p_z_order);
			}
		}

		void WidgetEntity::AddChildWidgetToList(WidgetEntity * child) {
			p_listChildWidget.push_back(child);
		}

		bool WidgetEntity::Update(float dt)
		{
			if (!Visible())
				return false;

			//p_Resource->update(dt);
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->Update(dt);
			}

			return true;
		}

		void WidgetEntity::SetPosValueXML(xml::UILayerWidgetDec * xml_value)
		{
			if (!xml_value->FirstValue->HaveDataVector2("pos_ratio"))
			{
				return;
			}

			auto originPosRatio = (xml_value->OriginValue->GetDataVector2("pos_ratio"));
			auto firstPosRatio = (xml_value->FirstValue->GetDataVector2("pos_ratio"));

			auto posOrigin = Vec2(GetGameSize().width * originPosRatio.x, GetGameSize().height * originPosRatio.y);
			auto posFirst = Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().width * firstPosRatio.y);

			SetFirstPos(Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y), xml_value->GeneralValue->GetDataInt("not_fade_pos") == 0);
		}


		void WidgetEntity::InsertChildWidget(WidgetEntity* child_widget)
		{
			p_listChildWidget.push_back(child_widget);
			child_widget->GetResource()->setName(child_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (child_widget->p_z_order == 0)
			{
				child_widget->p_z_order = p_listChildWidget.size();
			}
			child_widget->GetResource()->setLocalZOrder(p_z_order);
			child_widget->SetParentLayer(this->GetParentLayer());
			child_widget->SetMenuParent(this->GetMenuParent());
			child_widget->SetParentWidget(this);
			GetResource()->addChild(child_widget->GetResource(), child_widget->p_z_order);
		}

		void WidgetEntity::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			for (int i = 0; i < (int)(xml_value->p_list_child_widget.size()); i++)
			{
				auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(i), this->GetParentLayer(), this);

				ResizeAndPosChildWidget(_widget_child);

				_widget_child->SetMenuParent(this->GetMenuParent());
				_widget_child->SetParentWidget(this);
				InsertChildWidget(_widget_child);
			}
		}

		void WidgetEntity::InitParamParent(RKString name, xml::UILayerWidgetDec * xml_value, LayerEntity * layer_parent, WidgetEntity * widget_parent)
		{
			SetParentLayer(layer_parent);
			SetMenuParent(layer_parent->GetMenuParent());
			SetParentWidget(widget_parent);
			InitParam(name, xml_value);
		}

		bool WidgetEntity::ContainChildIdxAppear(unsigned int idx_appear)
		{
			if (GetIndexAppear() == idx_appear)
			{
				return true;
			}
			bool value = false;
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				value |= p_listChildWidget.at(i)->ContainChildIdxAppear(idx_appear);
			}
			return value;
		}

		WidgetEntity * WidgetEntity::GetWidgetChildAtIdx(int idx)
		{
			if (idx < (int)p_listChildWidget.size() && p_listChildWidget.size() > 0)
			{
				return p_listChildWidget.at(idx);
			}
			return nullptr;
		}

		WidgetEntity * WidgetEntity::GetWidgetChildByName(RKString name_path)
		{
			auto name_list = name_path.Split(".");
			int value_i = name_path.FindFirst(".", 0, 1);
			if (value_i == 0)
			{
				for (int i = 0; i < (int)p_listChildWidget.size(); i++)
				{

					RKString sub_name = "";
					for (int j = 0; j < (int)name_list.Size(); j++)
					{
						sub_name += name_list.GetAt(j);
						if (j != name_list.Size() - 1)
						{
							sub_name += ".";
						}
					}
					if (name_list.Size() == 1)
					{
						sub_name = name_list.GetAt(0);
					}

					auto value = p_listChildWidget.at(i)->GetWidgetChildByName(sub_name);
					if (value)
					{
						return value;
					}
				}
			}
			else
			{
				if ((GetName() == name_list.GetAt(0)) && name_list.Size() > 1)
				{
					for (int i = 0; i < (int)p_listChildWidget.size(); i++)
					{

						RKString sub_name = "";
						for (int j = 1; j < (int)name_list.Size(); j++)
						{
							sub_name += name_list.GetAt(j);
							if (j != name_list.Size() - 1)
							{
								sub_name += ".";
							}
						}
						auto value = p_listChildWidget.at(i)->GetWidgetChildByName(sub_name);
						if (value)
						{
							return value;
						}
					}
				}
				else if ((GetName() == name_list.GetAt(0)) && name_list.Size() <= 1)
				{
					return this;
				}
			}

			return nullptr;
		}

		int WidgetEntity::SetProcessActionChildIdxAppear(STATUS_MENU fade_state, unsigned int idx_appear)
		{
			if (GetIndexAppear() == idx_appear)
			{
				if (SetActionProcess(fade_state))
				{
					return 1;
				}
			}
			int value = 0;
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				value += p_listChildWidget.at(i)->SetProcessActionChildIdxAppear(fade_state, idx_appear);
			}

			return value;
		}

		void WidgetEntity::SetInteractive(bool value)
		{
			ActionProcess::SetInteractive(value);
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetInteractive(value);
			}
			//p_Resource->setTouchEnabled(value);
		}

		void WidgetEntity::SetDisable(bool value)
		{
			ActionProcess::SetDisable(value);
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDisable(value);
			}
		}

		void WidgetEntity::RemoveAllChild()
		{
			for (auto p : p_listChildWidget)
			{
				CCLOG("WidgetEntity::RemoveAllChild : ref p is : %d " , p->getReferenceCount());
				CCLOG("WidgetEntity::RemoveAllChild : ref NODE p is : %d ", p->GetResource()->getReferenceCount());
				p->release();
			}

			p_listChildWidget.clear();
		}

		void WidgetEntity::AddChildWidget(WidgetEntity * object)
		{
			if (object)
			{
				if (object->p_z_order == 0)
				{
					object->p_z_order = p_listChildWidget.size();
				}

				this->GetResource()->addChild(object->GetResource(), object->p_z_order);
				p_listChildWidget.push_back(object);
				object->GetResource()->setName(object->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
				object->GetResource()->setLocalZOrder(object->p_z_order);
				object->SetParentWidget(this);
				object->SetParentLayer(this->GetParentLayer());
				object->SetMenuParent(this->GetMenuParent());
			}
		}

		void WidgetEntity::ResizeAndPosNode(cocos2d::Node * child , WidgetEntity * child_w)
		{
			//set new size for widget child
			cocos2d::Vec2 parent_size = this->GetSize();
			cocos2d::Size content_size = this->GetResource()->getContentSize();


			float scale_parent_widget_x = parent_size.x / content_size.width;
			float scale_parent_widget_y = parent_size.y / content_size.height;

			Vec2 design_size = this->GetMenuParent()->GetDesignSize();
			Vec2 scale_screen = Vec2(GetGameSize().width / design_size.x, GetGameSize().height / design_size.y);
			cocos2d::Vec2 parent_origin_size = Vec2(0, 0);
			if (p_state_scale == 0)
			{
				parent_origin_size = Vec2(parent_size.x / scale_screen.x, this->p_IsStickScreen ? (parent_size.y / scale_screen.y) : (parent_size.y / scale_screen.x));
			}
			else if (p_state_scale == 1)
			{
				parent_origin_size = Vec2(parent_size.x / scale_screen.y, parent_size.y / scale_screen.y);
			}
			else
			{
				parent_origin_size = parent_size;
			}
            
            Vec2 origin_child_pos = Vec2(
                                  (child->getPosition().x / scale_screen.x),
                                  (child->getPosition().y / scale_screen.y));
            
                
            
            Vec2 pos_ratio = Vec2(origin_child_pos.x / parent_origin_size.x, origin_child_pos.y / parent_origin_size.y);
                
			Vec2 new_pos_child = Vec2((1 / scale_parent_widget_x) * parent_size.x *pos_ratio.x,
				(1 / scale_parent_widget_y) * parent_size.y *pos_ratio.y);

            child->setPosition(new_pos_child);
            

			//handle screen design is small
			if (scale_screen.x < 1.f)
			{
				if (child_w && child_w->p_state_scale == 2 && this->p_state_scale != 2)
				{

					Vec2 delta_scale_child = Vec2(1, 1);
					if (!this->p_IsStickScreen)
					{
						if (this->p_state_scale == 0)
						{
							delta_scale_child = Vec2(scale_screen.x * scale_parent_widget_x, scale_screen.x * scale_parent_widget_y);
						}
						else if (this->p_state_scale == 1)
						{
							delta_scale_child = Vec2(scale_screen.x * scale_parent_widget_x, scale_screen.x * scale_parent_widget_y);
						}
					}
					else
					{
						delta_scale_child = Vec2(scale_screen.x * scale_parent_widget_x, scale_screen.y * scale_parent_widget_y);
					}

					child->setScaleX(delta_scale_child.x);
					child->setScaleY(delta_scale_child.y);
				}
			}
        }
        
        void WidgetEntity::ResizeAndPosChildWidget(WidgetEntity * child)
		{
			//set new size for widget child
            ResizeAndPosNode(child->GetResource(), child);
		}

	    void WidgetEntity::SetPosition(Vec2 p)
		{
			if (HaveAligneRes())
			{
				Vec2 size_parent = Vec2(0, 0);
				if (GetParentWidget())
				{
					size_parent = GetParentWidget()->GetSize();
				}
				else if (GetParentLayer())
				{
					size_parent = GetParentLayer()->GetSize();
				}
				if (size_parent != Vec2(0, 0))
				{
					Vec2 anchor_point = this->GetResource()->getAnchorPoint();
					Vec2 pos_a = Vec2(p_aligne_res[1], p_aligne_res[2]);
					
					if (pos_a.x <= -1)
					{
						pos_a.x = p.x;
					}
					else
					{
						if (anchor_point.x == 1)
						{					
							pos_a.x = size_parent.x - pos_a.x;
						}
					}

					if (pos_a.y <= -1)
					{
						pos_a.y = p.y;
					}
					else
					{
						if (anchor_point.y == 1)
						{
							pos_a.y = size_parent.y - pos_a.y;
						}
					}

					SetPosition(pos_a);
					return;
				}
			}
			ActionProcess::SetPosition(p);
		}

		void WidgetEntity::SetSize(Vec2 s)
		{
			s.x = (int)(s.x + 0.5f);
			s.y = (int)(s.y + 0.5f);

			if (HaveAligneRes())
			{
				SetSize(Vec2(
					p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x,
					p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
				return;
			}

			if (p_isScale9Enable)
			{
				if (p_stage_scale9 == 0)
				{
					p_Resource->setContentSize(cocos2d::Size(s.x, s.y));
				}
				else if (p_stage_scale9 == 1)
				{
					Vec2 origin_size = GetResource()->getContentSize();
					float scale = s.x / origin_size.x;
                    
                    p_Resource->setContentSize(cocos2d::Size(origin_size.x, s.y * (1 / scale)));
					GetResource()->setScale(scale);
				}
				else if (p_stage_scale9 == 2)
				{
					Vec2 origin_size = GetResource()->getContentSize();
					float scale = s.y / origin_size.y;
                    
                    p_Resource->setContentSize(cocos2d::Size(s.x * (1 / scale), origin_size.y));

					GetResource()->setScale(scale);

				}
				else if (p_stage_scale9 == 3)
				{
					Vec2 origin_size = GetResource()->getContentSize();
					float delta_scale = GetGameSize().width / this->GetMenuParent()->GetDesignSize().x;
					Vec2 scale_size_design = Vec2(s.x / delta_scale, s.y / delta_scale);

					p_Resource->setContentSize(cocos2d::Size(scale_size_design.x, scale_size_design.y));

					GetResource()->setScale(delta_scale);

				}
				else
				{
					PASSERT2(false, "wrong parameter");
				}
			}
			else
			{
				ActionProcess::SetSize(s);
			}
		}

		Vec2 WidgetEntity::GetSize()
		{
			if (p_isScale9Enable)
			{
				return Vec2(p_Resource->getContentSize().width, p_Resource->getContentSize().height);
			}
			else
			{
				return ActionProcess::GetSize();
			}
		}

		void WidgetEntity::ReloadText()
		{
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->ReloadText();
			}
		}

		void WidgetEntity::ClearChild(int count)
		{
			if (count != 0)
			{
				//static_cast<Widget*>(this->GetResource())->removeFromParentAndCleanup(true);
				count++;
			}
			this->GetResource()->removeAllChildren();
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDeleteResource(false);
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();
		}

		void WidgetEntity::SetResourceToNull()
		{
			p_Resource = nullptr;
		}

		void WidgetEntity::SetDeleteResource(bool value, bool allChild) 
		{ 
			p_mark_as_delete_resource = value; 

			if (allChild)
			{
				for (int i = 0; i < (int)p_listChildWidget.size(); i++)
				{
					p_listChildWidget.at(i)->SetDeleteResource(value , allChild);
				}
			}
		}

		int WidgetEntity::GetZOrder() 
		{ 
			return p_z_order; 
		}
		void WidgetEntity::SetZOrder(int z) 
		{ 
			p_z_order = z; 
			GetResource()->setLocalZOrder(z);
		}

		float WidgetEntity::getWidth()
		{
			return this->GetResource()->getContentSize().width * this->GetResource()->getScaleX();
		}

		float WidgetEntity::getHieght()
		{
			return this->GetResource()->getContentSize().height * this->GetResource()->getScaleY();
		}

		void WidgetEntity::SetVisible(bool visible)
		{
			GetResource()->setVisible(visible);
		}

		bool WidgetEntity::IsVisible()
		{
			return GetResource()->isVisible();
		}

		void WidgetEntity::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			if (p_isScale9Enable && xml_value->GeneralValue->HaveDataInt("stage_scale9"))
			{
				p_stage_scale9 = xml_value->GeneralValue->GetDataInt("stage_scale9");
			}
			if (xml_value->GeneralValue->HaveDataInt("min_size_scale"))
			{
				p_min_size_scale = xml_value->GeneralValue->GetDataInt("min_size_scale");
			}
			if (xml_value->GeneralValue->HaveDataChar("aligne_res"))
			{
				RKString aligne_res = xml_value->GeneralValue->GetDataChar("aligne_res");
				aligne_res.ReplaceFirst("[", "");
				aligne_res.ReplaceLast("]", "");

				auto list_dec = aligne_res.Split("_");
				for (int i = 0; i < (int)list_dec.Size(); i++)
				{
					p_aligne_res.push_back(atoi(list_dec.GetAt(i).GetString()));
				}
			}

			ActionProcess::InitParam(name, xml_value);

			if (xml_value->sound_click)
			{
				ID_SOUND = new xml::sound_click_dec(xml_value->sound_click);
			}
		}

		bool WidgetEntity::HaveAligneRes()
		{
			if (p_aligne_res.size() > 0)
			{
				int res_change = p_aligne_res[0];
				if (GetGameSize().width >= res_change)
				{
					if (p_state_aligne == false)
					{
						p_state_aligne = true;
						return false;
					}
					p_state_aligne = false;
					return true;
				}
			}
			return false;
		}

	}
}