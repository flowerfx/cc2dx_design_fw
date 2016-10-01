#include "wObject.h"
#include "../ObjectTexture.h"
#include "../DisplayView.h"
#include "cocos/ui/UIScale9Sprite.h"
#define HANDLE_ACTION 0
#define USE_ACTION_QUEUE 1
namespace Utility
{
	namespace wObject
	{
		ActionQueue::ActionQueue(int tag, cocos2d::Sequence * action, RKString name)
				:p_action_tag(tag),
				action_name(name)
		{
			p_action = action;
		}

		ActionQueue::~ActionQueue()
		{
			p_action_tag = 0;
			if (p_action->getReferenceCount() != 1)
			{
				CCLOG("leak detected in action , fix leak...");
				//fix leak
				while (p_action->getReferenceCount() > 2)
				{
					p_action->release();
				}
			}
			p_action->release();
		}

		wBasicObject::wBasicObject()
		{
			p_Resource = nullptr;

			p_TypeObject = TYPE_OBJECT::OBJ_NONE;

			p_list_command.clear();

			p_name_cotain = "";

			p_OnActionLock = false;

			p_parent_object = nullptr;
			p_listChildObject.clear();

			//p_valueContain = new xml::BasicDec();

			p_ActionListQueue.clear();

			design_size = Vec2(0, 0);

			p_store_animate_list = nullptr;
			_mark_remove_resource = true;
		}

		wBasicObject::~wBasicObject()
		{
			FreeObject();
		}

		void wBasicObject::FreeObject()
		{	

			//
			for (auto it = p_list_command.begin(); it != p_list_command.end(); it++)
			{
				delete p_list_command.at(it->first);
			}
			p_list_command.clear();
			p_valueContain.clearAllData();

			//dont delete parent
			p_parent_object = nullptr;

			for (auto c :  p_listChildObject)
			{
				c->release();
				if (c->getReferenceCount() >= 2 && c->getReferenceCount() < 100)
				{
					PASSERT2(false, "leak detected");
					c->release();
				}
			}
			p_listChildObject.clear();


			if (p_Resource && _mark_remove_resource) //maybe the resouce have delete
			{			
				p_Resource->removeAllChildren();
				p_Resource->release();
			}
			p_Resource = nullptr;
#if USE_ACTION_QUEUE
			for (auto ac : p_ActionListQueue)
			{
				delete ac;
			}
#endif
			p_ActionListQueue.clear();

			if (p_store_animate_list)
			{
				p_store_animate_list->release();
			}

		}

		void wBasicObject::Flip(bool UseY, bool b)
		{
			CCASSERT(GetResource(), "object sprite invalid");

			switch (p_TypeObject)
			{
			case Utility::wObject::OBJ_NONE:
				break;
			case Utility::wObject::OBJ_ARMATURE:
			case Utility::wObject::OBJ_LABEL:
			{
				if (!UseY)
				{
					float cur_scale = (p_Resource)->getScaleX();
					(p_Resource)->setScaleX(b ? -abs(cur_scale) : abs(cur_scale));
				}
				else
				{
					float cur_scale = (p_Resource)->getScaleY();
					(p_Resource)->setScaleY(b ? -abs(cur_scale) : abs(cur_scale));
				}
				break;
			}
			case Utility::wObject::OBJ_ANIMATION:
			case Utility::wObject::OBJ_SPRITE:
			case Utility::wObject::OBJ_PROCESS_TIMER:
			{
				if (!UseY)
				{
					static_cast<Sprite*>(p_Resource)->setFlippedX(b);
				}
				else
				{
					static_cast<Sprite*>(p_Resource)->setFlippedX(b);
				}
				break;
			}
			case Utility::wObject::OBJ_COUNT:
				break;
			default:
				break;
			}
		}

		void wBasicObject::UpdateActionProcess(float dt)
		{
#if ! USE_ACTION_QUEUE
			return;
#endif

#if HANDLE_ACTION
			if (p_ActionListQueue.size() > 0 && !p_OnActionLock) //check the queue
			{
				if (p_ActionListQueue.at(0)->p_action_tag > 0)
				{
					auto action = p_ActionListQueue.at(0)->p_action;
					if (action)
					{
						action->step(dt);
					}
				}
			}
#endif
			if (p_OnActionLock && p_ActionListQueue.size() > 0 )
			{

#if ! HANDLE_ACTION
				this->GetResource()->stopAllActions();
				GetResource()->getActionManager()->removeAllActionsFromTarget(GetResource());
#endif
				delete p_ActionListQueue[0];
				p_ActionListQueue.erase(p_ActionListQueue.begin());

				if (p_ActionListQueue.size() > 0)
				{
#if HANDLE_ACTION
					p_ActionListQueue.at(0)->p_action->startWithTarget(this->GetResource());
#else
					GetResource()->runAction(p_ActionListQueue.at(0)->p_action);
#endif
				}
				p_OnActionLock = false;
			}
			else if (p_OnActionLock)
			{
				p_OnActionLock = false;
			}

			for (auto object : p_listChildObject)
			{
				object->UpdateActionProcess(dt);
			}

			return;
		}

		void wBasicObject::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			GetResource()->draw(renderer, transform, flags);
		}

		void wBasicObject::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			GetResource()->visit(renderer, transform, flags);
		}

		void wBasicObject::Update(float dt)
		{
			UpdateActionProcess(dt);
		}

#pragma region init_method
		bool wBasicObject::OnInitWideObject(RKString name, bool UseAntiAlias)
		{
			auto object_dec = XMLMgr->GetWideObjectByName(name);

			bool res = OnInitWideObjectXML(object_dec, UseAntiAlias);

			return res;
		}

		void wBasicObject::LoadObjectSpritePatch(RKString name_texture, xml::Wide_ObjectDec * xml_obj)
		{
			Vec2 number_sprite = xml_obj->CommonValue->GetDataVector2("number_sprite");
			Vec2 pos_sprite = xml_obj->CommonValue->GetDataVector2("idx_sprite");
			auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_texture.GetString());
			Vec2 size_spite_frame = sprite_frame->getOriginalSize();

			Vec2 size_each_sprite = Vec2(size_spite_frame.x / number_sprite.x , size_spite_frame.y / number_sprite.y);
			
			Vec2 pos_sprite_with_texture = Vec2(0, 0);
			if (!sprite_frame->isRotated())
			{
				pos_sprite_with_texture = Vec2(
					sprite_frame->getRect().origin.x + pos_sprite.x*size_each_sprite.x ,
					sprite_frame->getRect().origin.y + pos_sprite.y*size_each_sprite.y
				);
			}
			else
			{
				pos_sprite_with_texture = Vec2(
					sprite_frame->getRect().origin.x + ((number_sprite.y - pos_sprite.y - 1) * size_each_sprite.y),
					sprite_frame->getRect().origin.y + ((number_sprite.x - pos_sprite.x - 1) * size_each_sprite.y)
				);
			}
			Sprite * p = Sprite::createWithTexture(
					sprite_frame->getTexture(),
                                                   cocos2d::Rect(pos_sprite_with_texture.x, pos_sprite_with_texture.y, size_each_sprite.x, size_each_sprite.y)
				,
					sprite_frame->isRotated()
				);
			p_TypeObject = TYPE_OBJECT::OBJ_SPRITE_PATCH;
			p_Resource = p;

		}

		bool wBasicObject::OnInitWideObjectXML(xml::Wide_ObjectDec * object_dec, bool UseAntiAlias)
		{
			if (!object_dec)
			{
				CCASSERT(false, "no object loaded");
				return false;
			}
			p_name_cotain = object_dec->name_object;
			//
			design_size = object_dec->design_size;

			switch (object_dec->type_object)
			{
			case 0:
			case 4:
			case 5:
			case 6:
			case 7:
			{
				if (object_dec->type_object == 0 || object_dec->type_object == 4)
				{
					Sprite * p = TextureMgr->GetSpriteByName(object_dec->name_source);
					if (object_dec->type_object == 0)
					{
						LoadObjectFromSprite(p, UseAntiAlias);
					}
					else
					{
						auto p_ObjectTimer = ProgressTimer::create(p);
						p_TypeObject = TYPE_OBJECT::OBJ_PROCESS_TIMER;
						p_Resource = p_ObjectTimer;
					}
				}
				else if (object_dec->type_object == 5)
				{
					LoadObjectSpritePatch(object_dec->name_source, object_dec);
				}
				else if (object_dec->type_object == 6)
				{
					auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(object_dec->name_source.GetString());
					p_TypeObject = TYPE_OBJECT::OBJ_SCALE9;
					Scale9Sprite * p = Scale9Sprite::createWithSpriteFrame(sprite_frame);
						
					p_Resource = p;
				}
				else if (object_dec->type_object == 7)
				{
					//Layer *l = Layer::create();
					ui::Layout * l = ui::Layout::create();
					l->setClippingEnabled(false);
					l->setBackGroundColorType(ui::Layout::BackGroundColorType::NONE);
					p_TypeObject = TYPE_OBJECT::OBJ_LAYER;
					auto scale_ra = object_dec->OriginValue->GetDataVector2("scale_ratio");
					Vec2 actual_size = Vec2(scale_ra.x * object_dec->design_size.x, scale_ra.y * object_dec->design_size.x);
					
					l->setContentSize(cocos2d::Size(actual_size.x, actual_size.y));
					p_Resource = l;
				}

				break;
			}
			case 1:
			{
				LoadAnimateFromEditor(object_dec->animature_dec);
				break;
			}
			case 2:
			{
				LoadAnimateFromXML(object_dec->animation_dec);
				break;
			}
			case 3:
			{
				LoadFontDecFromXML(object_dec->text_dec);
				break;
			}
			default:
				PASSERT2(false, "no object loaded");
				return false;
				break;
			}

			//
			bool casca_color = object_dec->CommonValue->GetDataInt("casca_color") == 1;
			bool casca_opacity = object_dec->CommonValue->GetDataInt("casca_opacity") == 1;
			p_Resource->setCascadeColorEnabled(casca_color);
			p_Resource->setCascadeOpacityEnabled(casca_opacity);
			//
			LoadValueBasicObject(object_dec);
			//
			p_Resource->retain();
			//
			return true;
		}

		void wBasicObject::LoadObjectFromSprite(Sprite * p, bool UseAntiAlias)
		{
#ifdef USE_SPRITE_FRAME
			p_Resource = p;
#else
			p_Resource = p->createWithTexture(p->getTexture(), p->getTextureRect());
#endif
#if USE_ANTIALIAS
			if (UseAntiAlias)
			{
				p_Resource->getTexture()->generateMipmap();
				Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
				p_Resource->getTexture()->setTexParameters(&texParams);
			}
#endif
			p_TypeObject = TYPE_OBJECT::OBJ_SPRITE;

		}

		void wBasicObject::LoadAnimateFromXML(xml::AnimateDec * animation_dec)
		{
			if (animation_dec)
			{
				//CCASSERT(p_Object != NULL, "object not init");

				Vector<SpriteFrame*> animFrames;
				PASSERT2(animation_dec->GetNumberSprite() > 0, "wrong data!");
				Sprite * temp = TextureMgr->GetSpriteByName(animation_dec->GetSpriteFrameNameAtIdx(0).str_img);
				for (int i = 0; i < animation_dec->GetNumberSprite(); i++)
				{
					auto rec = animation_dec->GetSpriteFrameNameAtIdx(i).rect_img;
					auto frame = SpriteFrame::createWithTexture(temp->getTexture(), rec);
					animFrames.pushBack(frame);
				}
				p_store_animate_list = CCAnimation::createWithSpriteFrames(animFrames, animation_dec->time_delay, animation_dec->loop);
				p_store_animate_list->retain();
				//auto t = CCAnimate::create(p_store_animate_list);
				//t->setTag(animation_dec->tag_action);

				Sprite * p = Sprite::createWithTexture(temp->getTexture(), animation_dec->GetSpriteFrameNameAtIdx(0).rect_img);
				Sprite * p_Object = nullptr;
#ifdef USE_SPRITE_FRAME
				p_Object = p;
#else
				p_Object = p->createWithTexture(p->getTexture(), p->getTextureRect());
#endif
				//p_Object->runAction(t);
				p_TypeObject = TYPE_OBJECT::OBJ_ANIMATION;
				p_Resource = p_Object;
			}
		}

		void wBasicObject::LoadFontDecFromXML(xml::FontDec * font_dec)
		{
			if (font_dec)
			{
				auto label_temp = TextureMgr->GetFontByIdx(font_dec->idx);
				TTFConfig config(label_temp->getTTFConfig().fontFilePath.c_str(), font_dec->d_size * GetGameSize().width, font_dec->IsUnicode ? GlyphCollection::ASCII : GlyphCollection::DYNAMIC);
				auto p_textLabel = label_temp->createWithTTF(config, "0");

				p_TypeObject = TYPE_OBJECT::OBJ_LABEL;
				p_Resource = p_textLabel;

				SetColor(Color4B(font_dec->d_color[0], font_dec->d_color[1], font_dec->d_color[2], font_dec->d_color[3]));
			}
		}

		void wBasicObject::LoadAnimateFromEditor(xml::AnimatureDec * animature_dec)
		{
			if (animature_dec)
			{
				bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
				IsUseArk = true;
				g_IsUseArk = IsUseArk;
#endif
				//async loading export file
				ArmatureDataManager::getInstance()->addArmatureFileInfo(
					File::ResolveNamePath(animature_dec->pathTexture.GetString()).GetString(),
					File::ResolveNamePath(animature_dec->pathPlist.GetString()).GetString(),
					File::ResolveNamePath(animature_dec->PathJson.GetString()).GetString()
					);

				//Creat armature by name
				auto p_Armature = CCArmature::create(animature_dec->name.GetString());

				//play animation by animation index
				p_Armature->getAnimation()->playWithIndex(animature_dec->idxPlay);

				p_Armature->getAnimation()->setSpeedScale(animature_dec->SpeedScale);

#ifdef USE_ARK_ARCHIVE
				IsUseArk = false;
				g_IsUseArk = IsUseArk;
#endif
				p_Resource = p_Armature;

				p_TypeObject = TYPE_OBJECT::OBJ_ARMATURE;
			}
		}

		void wBasicObject::LoadValueBasicObject(xml::Wide_ObjectDec * object_dec)
		{
			if (object_dec)
			{
				/*
					set the origin value of the object
					*/

				bool IsVisible = (object_dec->CommonValue->GetDataInt("visible") == 1);
				SetVisible(IsVisible);

				bool IsStickScreen = (object_dec->CommonValue->GetDataInt("stick_screen") != 0);

				if (object_dec->CommonValue->HaveDataVector2("anchor_point"))
				{
					GetResource()->setAnchorPoint(object_dec->CommonValue->GetDataVector2("anchor_point"));
				}
				else
				{
					GetResource()->setAnchorPoint(Vec2(0.5f, 0.5f));
				}

				//set size of object
				if (IsStickScreen)
				{
					auto size_t = GetGameSize();
					auto scale_ra = object_dec->OriginValue->GetDataVector2("scale_ratio");
					Vec2 size_object = Vec2(
						scale_ra.x * size_t.width,
						scale_ra.y * size_t.height);
					SetSize(size_object);
					p_valueContain.InsertDataVector2("size_origin", size_object);
				}
				else
				{
					auto size_t = GetGameSize();
					auto scale_ra = object_dec->OriginValue->GetDataVector2("scale_ratio");
					Vec2 size_object = Vec2(
						scale_ra.x * size_t.width,
						scale_ra.y * size_t.width);
					SetSize(size_object);
					p_valueContain.InsertDataVector2("size_origin", size_object);
				}

				SetPosition(Vec2(object_dec->OriginValue->GetDataVector2("pos_ratio").x * GetGameSize().width,
					object_dec->OriginValue->GetDataVector2("pos_ratio").y * GetGameSize().height));

				p_valueContain.InsertDataVector2("pos_origin", Vec2(object_dec->OriginValue->GetDataVector2("pos_ratio").x * GetGameSize().width,
					object_dec->OriginValue->GetDataVector2("pos_ratio").y * GetGameSize().height));

				//set opacity of object
				SetOpacity(object_dec->OriginValue->GetDataInt("opacity"));
				p_valueContain.InsertDataInt("opacity_origin", object_dec->OriginValue->GetDataInt("opacity"));

				//set color of object
				SetColor(object_dec->OriginValue->GetDataColor("tint"));
				p_valueContain.InsertDataColor("tint_origin", object_dec->OriginValue->GetDataColor("tint"));

				//set angle object
				SetAngle(object_dec->OriginValue->GetDatafloat("rotate"));
				p_valueContain.InsertDatafloat("rotate_origin", object_dec->OriginValue->GetDatafloat("rotate"));

				//set command
				SetCommandValueXML(object_dec);

				//set child
				int idx = 0;
				for (auto object : object_dec->list_child_object)
				{
					auto child_object = OnLoadChildObject(this, object);

					child_object->p_Resource->setLocalZOrder(idx);
					child_object->p_Resource->setName(child_object->GetName().GetString());
					this->ResizeAndPosChildWidget(child_object);
					this->p_Resource->addChild(child_object->GetResource());
					this->p_listChildObject.pushBack(child_object);
					idx++;
				}

			//	GetResource()->setCascadeOpacityEnabled(true);
			//	GetResource()->setCascadeColorEnabled(true);

				LoadFirstValue(object_dec);
			}
		}

		void wBasicObject::LoadFirstValue(xml::Wide_ObjectDec * object_dec)
		{
			if (object_dec)
			{
				/*
				set the origin value of the object
				*/

				bool IsStickScreen = (object_dec->CommonValue->GetDataInt("stick_screen") != 0);

				//set size of object
				if (object_dec->FirstValue->HaveDataVector2("scale_ratio"))
				{
					if (IsStickScreen)
					{
						Vec2 size_object = Vec2(
							object_dec->FirstValue->GetDataVector2("scale_ratio").x * GetGameSize().width,
							object_dec->FirstValue->GetDataVector2("scale_ratio").y * GetGameSize().height);
						SetSize(size_object);
					}
					else
					{
						Vec2 size_object = Vec2(
							object_dec->FirstValue->GetDataVector2("scale_ratio").x * GetGameSize().width,
							object_dec->FirstValue->GetDataVector2("scale_ratio").y * GetGameSize().width);
						SetSize(size_object);
					}
				}

				if (object_dec->FirstValue->HaveDataVector2("pos_ratio"))
				{
					SetPosition(Vec2(object_dec->FirstValue->GetDataVector2("pos_ratio").x * GetGameSize().width,
						object_dec->FirstValue->GetDataVector2("pos_ratio").y * GetGameSize().height)
					);
				}

				if (object_dec->FirstValue->HaveDataInt("opacity"))
				{
					//set opacity of object
					SetOpacity(object_dec->FirstValue->GetDataInt("opacity"));
				}

				if (object_dec->FirstValue->HaveDataColor("tint"))
				{
					//set color of object
					SetColor(object_dec->FirstValue->GetDataColor("tint"));
				}

				if (object_dec->FirstValue->HaveDatafloat("rotate"))
				{
					//set angle object
					SetAngle(object_dec->FirstValue->GetDatafloat("rotate"));
				}
			}
		}

		void wBasicObject::SetCommandValueXML(xml::Wide_ObjectDec * xml_value)
		{
			if (xml_value->GetCommandSize() <= 0)
				return;

			for (size_t i = 0; i < xml_value->GetCommandSize(); i++)
			{
				ActionCommand * p_command = new ActionCommand();

				auto cmd_pair = xml_value->GetIterCommandAtIdx(i);

				if (cmd_pair.first != "" && p_command->ReadJsonString(cmd_pair.second.GetString()))
				{
					p_list_command.insert(std::pair<RKString, ActionCommand*>(cmd_pair.first, p_command));
				}
			}
		}

		bool wBasicObject::SetActionCommandWithName(RKString name, CallFunc * call_back, bool force)
		{
			if (p_list_command.size() < 0 || p_list_command.find(name) == p_list_command.end())
			{
				return false;
			}
#if USE_ACTION_QUEUE
			force = false;
			//check the action already added
			if (!force)
			{
				if (p_ActionListQueue.size() > 0)
				{
					for (auto ac : p_ActionListQueue)
					{
						if (ac->action_name == name)
						{
							//PASSERT2(false, "action have same name ????");
							return false;
						}
					}
				}

			}
#endif
			//get the action command with name 

			int p_action_process = 0;

			ActionCommand* action_command = p_list_command.at(name);

			int flag = 0;
			auto action_squence = action_command->GetSequenceActionData(p_action_process, this->GetResource(), this->p_parent_object ? this->p_parent_object->GetResource() : nullptr, flag);
			action_squence.pushBack(call_back ? call_back : CallFunc::create(CC_CALLBACK_0(wBasicObject::EventActionFinish, this)));
			//action_squence.pushBack(nullptr);
			auto action = Sequence::create(action_squence);
#if MANAGE_ACTION
			if (action)
			{
				PoolManager::getInstance()->getCurrentPool()->removeLastObject();
			}
#endif
			if (p_action_process <= 0)
			{
				p_action_process = 1;
			}

			action->setTag(p_action_process);
#if USE_ACTION_QUEUE
			action->retain();
			PASSERT2(action->getReferenceCount() <= 2, "ref break limit");
			if (!force)
			{
				p_ActionListQueue.push_back(new ActionQueue(p_action_process, action, name));

				if (p_ActionListQueue.size() == 1)
				{
#if HANDLE_ACTION
					action->startWithTarget(this->GetResource());
#else
					GetResource()->runAction(action);
#endif
					p_OnActionLock = false;
				}
			}
			else
			{
				GetResource()->runAction(action);
			}
#else
			GetResource()->runAction(action);
#endif
			CCLOG("action object ref count : %d", action->getReferenceCount());
			return true;
		}

		bool wBasicObject::ClearHookCommand(RKString name_command, RKString idx_act, int idx_squence, int idx_detail)
		{
			if (p_list_command.size() < 0 || p_list_command.find(name_command) == p_list_command.end())
			{
				return false;
			}

			ActionCommand* action_command = p_list_command.at(name_command);

			auto action_detail = action_command->GetActionDetail(idx_act, idx_squence, idx_detail);

			if (action_detail)
			{
				action_detail->ClearHookCommand();
				return true;
			}
			return false;
		}

		void wBasicObject::StopCurrentActionWithName(RKString name)
		{
			int idx_name = -1;
			int idx = 0;
			for (auto ac : p_ActionListQueue)
			{
				if (ac->action_name == name)
				{
					idx_name = idx;
					break;
				}
				idx++;
			}

			if (idx_name >= 0)
			{
				GetResource()->stopAllActionsByTag(p_ActionListQueue.at(idx_name)->p_action_tag);
				p_ActionListQueue.erase(p_ActionListQueue.begin() + idx_name);
			}
		}

		bool wBasicObject::SetActionCustomWithName(RKString name, cocos2d::Sequence* action_custom, CallFunc * call_back )
		{
			//check the action already added
#if USE_ACTION_QUEUE
			if (p_ActionListQueue.size() > 0)
			{
				for (auto ac : p_ActionListQueue)
				{
					if (ac->action_name == name)
					{
						return false;
					}
				}
			}
#endif
			auto action = Sequence::create(action_custom,
				call_back ? call_back : CallFunc::create(CC_CALLBACK_0(wBasicObject::EventActionFinish, this)),
				nullptr);

			action->setTag(action_custom->getTag());
#if USE_ACTION_QUEUE
			action->retain();
			p_ActionListQueue.push_back(new ActionQueue(action_custom->getTag(), action, name));

			if (p_ActionListQueue.size() == 1)
			{

#if HANDLE_ACTION
				action->startWithTarget(this->GetResource());
#else
				GetResource()->runAction(action);
#endif
				p_OnActionLock = false;
			}
#else
			GetResource()->runAction(action);
#endif
			CCLOG("action object ref count : %d", action->getReferenceCount());
			return true;
		}

		void wBasicObject::ResizeAndPosChildWidget(wBasicObject * child)
		{
			Vec2 size_origin_parent = p_valueContain.GetDataVector2("size_origin");

			//set new size for widget child
			float scale_parent_widget_x = size_origin_parent.x / this->GetResource()->getContentSize().width;
			float scale_parent_widget_y = size_origin_parent.y / this->GetResource()->getContentSize().height;

			Vec2 design_size = child->design_size;

			Vec2 first_pos = Vec2((child->GetPosition().x / GetGameSize().width) * design_size.x, (child->GetPosition().y / GetGameSize().height) * design_size.y);
			//child->SetPosition(first_pos);

			Vec2 first_size = Vec2(size_origin_parent.x * design_size.x / GetGameSize().width,
								   size_origin_parent.y * design_size.x / GetGameSize().width);


			Vec2 pos_ratio = Vec2(first_pos.x / first_size.x, first_pos.y / first_size.y);
			//if (scale_parent_widget_x != 1.0f)
			{
				child->GetResource()->setScaleX((1 / scale_parent_widget_x) * child->GetResource()->getScaleX());
				child->GetResource()->setScaleY((1 / scale_parent_widget_x) * child->GetResource()->getScaleY());

				child->SetPosition(
					Vec2(
					(1 / scale_parent_widget_x) * size_origin_parent.x *pos_ratio.x,
					(1 / scale_parent_widget_y) * size_origin_parent.y *pos_ratio.y));
			}
		}

		wBasicObject * wBasicObject::OnLoadChildObject(wBasicObject * parent, xml::Wide_ObjectDec * objec)
		{
			if (objec)
			{
				wBasicObject * _child = new wBasicObject();

				_child->OnInitWideObjectXML(objec);

				_child->p_parent_object = parent;

				return _child;
			}
			return nullptr;
		}
#pragma endregion init_method


#pragma region normal_method
		cocos2d::Node*  wBasicObject::GetResource()
		{
			return p_Resource;
		}

		void wBasicObject::SetSize(Vec2 Size)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			if (p_TypeObject == TYPE_OBJECT::OBJ_SCALE9)
			{
				static_cast<Scale9Sprite*>(p_Resource)->setContentSize(cocos2d::Size(Size.x,Size.y));
			}
			else
			{
				Vec2 p_Size(1, 1);
				//
				p_Size.x = Size.x / GetResource()->getContentSize().width;
				p_Size.y = Size.y / GetResource()->getContentSize().height;
				if (p_TypeObject == TYPE_OBJECT::OBJ_LABEL)
				{
					GetResource()->setScaleX(p_Size.x);
					GetResource()->setScaleY(p_Size.x);
				}
				else
				{
					GetResource()->setScaleX(p_Size.x);
					GetResource()->setScaleY(p_Size.y);
				}
			}

		}

		Vec2 wBasicObject::GetSize()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			if (p_TypeObject == TYPE_OBJECT::OBJ_SCALE9)
			{
				return static_cast<Scale9Sprite*>(p_Resource)->getContentSize();
			}
			else
			{
				Vec2 p_Size(1, 1);
				//
				p_Size.x = GetResource()->getContentSize().width * GetResource()->getScaleX();
				p_Size.y = GetResource()->getContentSize().height * GetResource()->getScaleY();
				//
				return p_Size;
			}
		}

		void wBasicObject::SetScale(Vec2 scale)
		{
			if (p_TypeObject == TYPE_OBJECT::OBJ_SCALE9)
			{
				//static_cast<Scale9Sprite*>(GetResource())->SetSi
			}
			else
			{
				Vec2 p_scale(1, 1);
				int mipmap = TextureMgr->GetLevelMipMap();

				p_scale.x = GetMulMul(scale.x, mipmap);
				p_scale.y = GetMulMul(scale.y, mipmap);


				CCASSERT(GetResource(), "object sprite invaid");
				GetResource()->setScaleX(p_scale.x);
				GetResource()->setScaleY(p_scale.y);
			}
		}

		Vec2 wBasicObject::GetScale()
		{
			if (p_TypeObject == TYPE_OBJECT::OBJ_SCALE9)
			{
				return Vec2(1, 1);
			}
			else
			{
				Vec2 p_scale(1, 1);
				int mipmap = TextureMgr->GetLevelMipMap();

				CCASSERT(GetResource(), "object sprite invaid");
				p_scale.x = GetMulDiv(GetResource()->getScaleX(), mipmap);
				p_scale.y = GetMulDiv(GetResource()->getScaleY(), mipmap);

				return p_scale;
			}
		}

        void wBasicObject::SetOpacity(RKUtils::BYTE opacity)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			GetResource()->setOpacity(opacity);
		}

		RKUtils::BYTE  wBasicObject::GetOpacity()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			return GetResource()->getOpacity();
		}

		void wBasicObject::SetAngle(float angle)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			GetResource()->setRotation(angle);
		}

		float wBasicObject::GetAngle()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			return GetResource()->getRotation();
		}

		void wBasicObject::SetColor(Color4B color)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			if (p_TypeObject == TYPE_OBJECT::OBJ_LABEL)
			{
				static_cast<Label*>(GetResource())->setTextColor(color);
			}
			else
			{
				GetResource()->setColor(Color3B(color.r, color.g, color.b));
			}
		}

		Color4B wBasicObject::GetColor()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			if (p_TypeObject == TYPE_OBJECT::OBJ_LABEL)
			{
				return static_cast<Label*>(GetResource())->getTextColor();
			}
			else
			{
				Color3B color = GetResource()->getColor();
				return Color4B(color.r, color.g, color.b, GetOpacity());
			}
		}

		void wBasicObject::SetPosition(Vec2 pos)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			GetResource()->setPosition(pos);
		}

		Vec2 wBasicObject::GetPosition()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			return GetResource()->getPosition();
		}

		void wBasicObject::SetX(float x)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			GetResource()->setPositionX(x);
		}

		float wBasicObject::GetX()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			return GetResource()->getPositionX();
		}

		void wBasicObject::SetY(float y)
		{
			CCASSERT(GetResource(), "object sprite invaid");
			GetResource()->setPositionY(y);
		}

		float wBasicObject::GetY()
		{
			CCASSERT(GetResource(), "object sprite invaid");
			return GetResource()->getPositionY();
		}

        cocos2d::Rect wBasicObject::GetBound()
		{
			Vec2 anchor = GetResource()->getAnchorPoint();

			Vec2 pos = GetPosition();
			Vec2 size = GetSize();
			Vec2 pos_bot_left = Vec2(
				pos.x - (anchor.x * (size.x / 2)),
				pos.y - (anchor.y * (size.y / 2))
			);

            cocos2d::Rect rec(
				pos_bot_left.x,
				pos_bot_left.y,
				size.x,
				size.y
				);
			return rec;
		}

		void wBasicObject::SetBlendFunc(BlendFunc blend)
		{
			if (p_TypeObject == TYPE_OBJECT::OBJ_LABEL)
			{
				static_cast<Label*>(p_Resource)->setBlendFunc(blend);
				return;
			}
			else if (p_TypeObject == TYPE_OBJECT::OBJ_SPRITE)
			{
				static_cast<Sprite*>(p_Resource)->setBlendFunc(blend);
			}
		}

		bool wBasicObject::IsVisible() 
		{ 
			return p_Resource->isVisible(); 
		}

		void wBasicObject::SetVisible(bool val) 
		{
			p_Resource->setVisible(val);
		}

#pragma endregion normal_method

		bool wBasicObject::OnFinishAction()
		{
			//abtrast class dont implement this function.....
			return true;
		}

		void wBasicObject::EventActionFinish()
		{
#if USE_ACTION_QUEUE
			p_OnActionLock = true;
#else
			p_Resource->getActionManager()->removeAllActionsFromTarget(p_Resource);
#endif
			OnFinishAction();
			//abtrast class dont implement this function...
		}

		wBasicObject * wBasicObject::GetObjectByName(RKString name_path)
		{
			if (name_path != "")
			{
				auto name_list = name_path.Split(".");
				int value_i = name_path.FindFirst(".", 0, 1);
				if (value_i == 0)
				{
					for (auto obj : p_listChildObject)
					{

						RKString sub_name = "";
						for (size_t j = 0; j < name_list.Size(); j++)
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

						auto value = obj->GetObjectByName(sub_name);
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
						for (auto obj : p_listChildObject)
						{

							RKString sub_name = "";
							for (size_t j = 1; j < name_list.Size(); j++)
							{
								sub_name += name_list.GetAt(j);
								if (j != name_list.Size() - 1)
								{
									sub_name += ".";
								}
							}
							auto value = obj->GetObjectByName(sub_name);
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
			return nullptr;
		}
	
		bool wBasicObject::SetHookCommand(RKString name_command, RKString idx_act, int idx_squence, int idx_detail, xml::BasicDec value, RKString name_value)
		{
			if (p_list_command.size() < 0 || p_list_command.find(name_command) == p_list_command.end())
			{
				return false;
			}

			ActionCommand* action_command = p_list_command.at(name_command);

			auto action_detail = action_command->GetActionDetail(idx_act, idx_squence, idx_detail);

			if (action_detail)
			{
				action_detail->SetValueHook(value, name_value);
				return true;
			}
			return false;
		}
	
		void wBasicObject::InsertObjectXMLDec(xml::Wide_ObjectDec * objec)
		{
			auto child_obj = OnLoadChildObject(this, objec);
			int i = this->p_listChildObject.size();
			child_obj->p_Resource->setLocalZOrder(i);
			child_obj->p_Resource->setName(child_obj->GetName().GetString());
			this->ResizeAndPosChildWidget(child_obj);
			this->p_Resource->addChild(child_obj->GetResource());
			this->p_listChildObject.pushBack(child_obj);
		}

		void wBasicObject::RemoveObjectChild(RKString name_child, bool All)
		{
			if (All)
			{
				this->p_Resource->removeAllChildren();
				//this->p_Resource->release();
				for (auto obj : p_listChildObject)
				{
					//obj->SetMarkRemoveResource(false);
					obj->RemoveObjectChild(name_child, All);
					obj->release();
					/*if (obj->getReferenceCount() >= 2 && obj->getReferenceCount() < 100)
					{
						PASSERT2(false, "leak detected");
						obj->release();
					}*/
				}
				p_listChildObject.clear();
			}
			else
			{
				auto obj = GetObjectByName(name_child);
				if (obj)
				{
					this->p_Resource->removeChildByName(obj->GetName().GetString());

					for (auto obj1 : p_listChildObject)
					{
						if (obj1 == obj)
						{
							p_listChildObject.eraseObject(obj1);
							break;
						}
					}
				}
			}

		}

		void wBasicObject::RunActionAnimate(CallFunc * call_back)
		{
			if (p_store_animate_list)
			{
				auto t = CCAnimate::create(p_store_animate_list);
				Vector<FiniteTimeAction*> array_final_Action;
				array_final_Action.pushBack(t);
				Sequence * ac = Sequence::create(array_final_Action);
				ac->setTag(100);
				SetActionCustomWithName("ACTION_ANIMATION", ac, call_back);
			}
		}

		void wBasicObject::SetMarkRemoveResource(bool value)
		{
			_mark_remove_resource = value;
		}
	}
}