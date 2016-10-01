#include "BasicObject.h"
#include "../ObjectTexture.h"
#include "../DisplayView.h"

namespace Utility
{
	namespace Object
	{
		BasicObject::BasicObject()
		{
			p_Object = NULL;
			p_ObjectTimer = NULL;
			p_Armature = NULL;
			p_textLabel = NULL;

			time_move_to = 0.f;
			p_IsUseAnimatEditor = false;
			p_IsText = false;

            p_IsVisible = true;
            
            p_listActionDone.Clear();
            p_listActionProcess.Clear();
            
            p_IsOnUpdate = true;
            
            p_origin_color = Color4B::WHITE;
            
            p_name_cotain ="";
            
		}

		BasicObject::~BasicObject()
		{
			FreeObject();
		}

		void BasicObject::FreeObject()
		{
			//
			if (p_Object)
			{
				p_Object->getActionManager()->removeAllActionsFromTarget(p_Object);
				p_Object->removeAllChildrenWithCleanup(true);
				CC_SAFE_DELETE(p_Object);
			}
			//
			if (p_Armature)
			{
				p_Armature->getActionManager()->removeAllActionsFromTarget(p_Armature);
				p_Armature->removeAllChildrenWithCleanup(true);
				CC_SAFE_DELETE(p_Armature);
			}
			//
			if (p_ObjectTimer)
			{ 
				p_ObjectTimer->getActionManager()->removeAllActionsFromTarget(p_ObjectTimer);
				p_ObjectTimer->removeAllChildrenWithCleanup(true);
				CC_SAFE_DELETE(p_ObjectTimer);
			}
			//
			if (p_textLabel)
			{
				p_textLabel->getActionManager()->removeAllActionsFromTarget(p_textLabel);
				p_textLabel->removeAllChildrenWithCleanup(true);
				CC_SAFE_DELETE(p_textLabel);
			}
				

			p_listActionDone.Clear();
			p_listActionProcess.Clear();
		}

		bool BasicObject::OnInitFirstValueBasicObject(RKString name, BASIC_ACTION action, bool UseAntiAlias, bool IsUseProcessSprite)
		{
			auto object_dec = XMLMgr->GetBasicByName(name);

			bool res = OnInitFirstValueBasicObjectByObject(object_dec, action, UseAntiAlias, IsUseProcessSprite);

			return res;
		}

		bool BasicObject::OnInitFirstValueBasicObjectByObject(xml::ObjectDec * object_dec, BASIC_ACTION action, bool UseAntiAlias, bool IsUseProcessSprite)
		{
			if (!object_dec)
			{
				CCASSERT(false, "no object loaded");
				return false;
			}
			p_name_cotain = object_dec->name;
			//

			if (object_dec->name_source)
			{
				Sprite * p = TextureMgr->GetSpriteByName(object_dec->name_source);
				LoadObjectFromSprite(p, UseAntiAlias);
				LoadAnimateFromXML(object_dec->animation_dec);
				if (IsUseProcessSprite)
				{
						p_ObjectTimer = ProgressTimer::create(p_Object);
						p_ObjectTimer->retain();
				}
			}
			else
			{
				if (object_dec->text_dec)
				{
					LoadFontDecFromXML(object_dec->text_dec);
				}
				else
				{
					auto amature_dec = object_dec->animature_dec;
					if (amature_dec)
					{
						LoadAnimateFromEditor(amature_dec);
					}
					else
					{
						CCASSERT(false, "no texture loaded");
						return false;
					}
				}
			}
			//
			LoadValueBasicObject(object_dec);
			LoadValueProcessObject(object_dec, action);
			//
			return true;
		}

		void BasicObject::Flip(bool UseY, bool b)
		{
			CCASSERT(GetObject(), "object sprite invaid");

			if (p_IsText)
			{
				if (!UseY)
				{
					float cur_scale = p_textLabel->getScaleX();
					p_textLabel->setScaleX(b ? -abs(cur_scale) : abs(cur_scale));
				}
				else
				{
					float cur_scale = p_textLabel->getScaleY();
					p_textLabel->setScaleY(b ? -abs(cur_scale) : abs(cur_scale));
				}
			}

			if (p_IsUseAnimatEditor)
			{
				if (!UseY)
				{
					float cur_scale = p_Armature->getScaleX();
					p_Armature->setScaleX(b ? -abs(cur_scale) : abs(cur_scale));
				}
				else
				{
					float cur_scale = p_Armature->getScaleY();
					p_Armature->setScaleY(b ? -abs(cur_scale) : abs(cur_scale));
				}
				return;
			}
			if (!UseY)
			{
				p_Object->setFlippedX(b);
			}
			else
			{
				p_Object->setFlippedX(b);
			}
		}

		cocos2d::Node*  BasicObject::GetObject()
		{
			if (p_IsText)
			{
				return p_textLabel;
			}

			if (p_IsUseAnimatEditor)
			{
				return p_Armature;
			}
			if (p_ObjectTimer)
			{
				return p_ObjectTimer;
			}
			return p_Object;
		}

		void BasicObject::UpdateActionProcess(float dt)
		{
			if (p_listActionProcess.Size() <= 0)
			{
				EventActionFinish();
				return;
			}
			RKList<int> listIdxactionNeedToRemove;
			for (int i = 0; i < p_listActionProcess.Size(); i++)
			{
				auto cur_action = p_listActionProcess.GetAt(i);
				if (cur_action.Status == STATUS_ACTION::STATUS_ACTIVE)
				{
					BASIC_ACTION ab = cur_action.Action;
					auto action = GetObject()->getActionByTag(ab);
					if (action)
					{
						action->step(dt);
					}
					else
					{
						//this action was removed out of action manager now remove it from list process
						listIdxactionNeedToRemove.Insert(i, 0);
					}
					action = NULL;
				}
			}
			//check is done
			for (int j = 0; j < p_listActionDone.Size(); j++)
			{
				RemoveAction(p_listActionDone.GetAt(j));
			}
			if (p_listActionDone.Size() > 0)
			{
				p_listActionDone.Clear();
			}
			//
			if (listIdxactionNeedToRemove.Size() > 0)
			{
				for (int i = 0; i < listIdxactionNeedToRemove.Size(); i++)
				{
					p_listActionProcess.EraseAt(listIdxactionNeedToRemove.GetAt(i));
				}
			}
		}

		void BasicObject::LoadObjectFromSprite(Sprite * p, bool UseAntiAlias)
		{
#if USE_SPRITE_FRAME
			p_Object = p;
#else
			p_Object = p->createWithTexture(p->getTexture(), p->getTextureRect());
#endif
#if USE_ANTIALIAS
			if (UseAntiAlias)
			{
				p_Object->getTexture()->generateMipmap();
				Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
				p_Object->getTexture()->setTexParameters(&texParams);
			}
#endif
			p_Object->retain();

		}

		void BasicObject::LoadAnimateFromXML(xml::AnimateDec * animation_dec)
		{
			if (animation_dec)
			{
				CCASSERT(p_Object != NULL, "object not init");

				Vector<SpriteFrame*> animFrames;

				for (size_t i = 0; i < animation_dec->GetNumberSprite(); i++)
				{
					Sprite * temp = TextureMgr->GetSpriteByName(animation_dec->GetSpriteFrameNameAtIdx(i));
					auto frame = SpriteFrame::createWithTexture(temp->getTexture(), temp->getTextureRect());
					animFrames.pushBack(frame);
				}
				Animation * p_Animate = CCAnimation::createWithSpriteFrames(animFrames, animation_dec->time_delay, animation_dec->loop);
				auto t = CCAnimate::create(p_Animate);
				t->setTag(BASIC_ACTION::BA_ANIMATION);
				t->autorelease();
				p_Object->runAction(t);
			}
		}

		void BasicObject::LoadFontDecFromXML(xml::FontDec * font_dec)
		{
			if (font_dec)
			{
				auto label_temp = TextureMgr->GetFontByIdx(font_dec->idx);
				TTFConfig config(label_temp->getTTFConfig().fontFilePath.c_str(), font_dec->d_size * GetGameSize().width, font_dec->IsUnicode ? GlyphCollection::ASCII : GlyphCollection::DYNAMIC);
				p_textLabel = label_temp->createWithTTF(config, "0");

				p_IsText = true;
				SetColor(Color4B(font_dec->d_color[0], font_dec->d_color[1], font_dec->d_color[2], font_dec->d_color[3]));

				p_textLabel->retain();
			}
		}

		void BasicObject::LoadAnimateFromEditor(xml::AnimatureDec * animature_dec)
		{
			if (animature_dec)
			{
				bool IsUseArk = false;
#ifdef USE_ARK_ARCHIVE
				IsUseArk = true;
				g_IsUseArk = IsUseArk;
#endif
				//async loading export file
				ArmatureDataManager::getInstance()->addArmatureFileInfo(File::ResolveNamePath(animature_dec->pathTexture).GetString(),
					File::ResolveNamePath(animature_dec->pathPlist).GetString(),
					File::ResolveNamePath(animature_dec->PathJson).GetString()
					);

				//Creat armature by name
				p_Armature = CCArmature::create(animature_dec->name.GetString());

				//retain
				p_Armature->retain();

				//play animation by animation index
				p_Armature->getAnimation()->playByIndex(animature_dec->idxPlay);

				p_Armature->getAnimation()->setSpeedScale(animature_dec->SpeedScale);

				p_IsUseAnimatEditor = true;
#ifdef USE_ARK_ARCHIVE
				IsUseArk = false;
				g_IsUseArk = IsUseArk;
#endif
			}
		}

		void BasicObject::LoadValueBasicObject(xml::ObjectDec * object_dec)
		{
			if (object_dec)
			{
				/*
					set the origin value of the object
					*/

				//set size of object
				if (object_dec->StickScreen == 1)
				{
					Vec2 size_object = Vec2(object_dec->Size_dec->GetDataVector2("origin").x * GetGameSize().width,
						object_dec->Size_dec->GetDataVector2("origin").y * GetGameSize().height);
					SetSize(size_object);
				}
				else
				{
					Vec2 size_object = Vec2(object_dec->Size_dec->GetDataVector2("origin").x * GetGameSize().width,
						object_dec->Size_dec->GetDataVector2("origin").y * GetGameSize().width);
					SetSize(size_object);
				}

				//set opacity of object
				SetOpacity(object_dec->Opacity_dec->GetDataInt("origin"));

				//set color of object
				if (object_dec->Color_dec)
				{
					p_origin_color = object_dec->Color_dec->GetDataColor("origin");
				}
				else
				{
					p_origin_color = Color4B::WHITE;
				}
				SetColor(p_origin_color);

				//set angle object
				if (object_dec->Rotate_dec)
				{
					SetAngle(object_dec->Rotate_dec->GetDatafloat("origin"));
				}
				else
				{
					SetAngle(0);
				}


			}
		}

		void BasicObject::OnSetProcessScaleAction(float delta_scale, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Size_dec;

			OnSetProcessScaleActionByDec(_dec, delta_scale, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessRotateAction(float angle_rotate, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Rotate_dec;

			OnSetProcessRotateActionByDec(_dec, angle_rotate, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessFadeAction(int fade_from, int fade_to, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Opacity_dec;

			OnSetProcessFadeActionByDec(_dec, fade_from, fade_to, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessColorAction(Color4B color_to, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Color_dec;

			OnSetProcessColorActionByDec(_dec, color_to, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessMoveDelayToAction(Vec2 position, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Move_delay_dec;

			OnSetProcessMoveDelayToActionByDec(_dec, position, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessMoveDelayByAction(Vec2 vector, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Move_delay_dec;

			OnSetProcessMoveDelayByActionByDec(_dec, vector, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetProcessProcessByAction(float percent, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Process_dec;

			OnSetProcessProcessByActionByDec(_dec, percent, loop, time, forceSet, reserve, delayTime, delay_first);
		}

		void BasicObject::OnSetActionCore(BASIC_ACTION type_ac, ActionInterval * ac1, ActionInterval * ac_reserve, int loop, float time_delay, float delay_first)
		{
			Action * action = NULL;
			if (loop < 0)
			{
				if (time_delay <= 0)
				{
					action = RepeatForever::create(
						Sequence::create(ac1, ac_reserve, nullptr)
						);
				}
				else
				{
					if (delay_first <= 0)
					{
						action = RepeatForever::create(
							Sequence::create(ac1, DelayTime::create(time_delay), ac_reserve, nullptr)
							);
					}
					else
					{
						action = RepeatForever::create(
							Sequence::create(DelayTime::create(delay_first), ac1, DelayTime::create(time_delay), ac_reserve, nullptr)
							);
					}
				}
			}
			else
			{
				Vector<FiniteTimeAction*> list_action;
				do
				{
					if (delay_first > 0)
					{
						list_action.insert(list_action.size(), DelayTime::create(delay_first));
					}
					list_action.insert(list_action.size(), ac1);
					if (time_delay > 0)
					{
						list_action.insert(list_action.size(), DelayTime::create(time_delay));
					}
					if (ac_reserve)
					{
						list_action.insert(list_action.size(), ac_reserve);
					}
					loop--;
				} while (loop > 0);

				auto action_complete = CallFunc::create(
					[=]()
				{
					//after move complete just remove it out of the list
					//RemoveAction(BA_ROTATE_TO);
					bool res = OnFinishAction(type_ac);
					if (res)
					{
						p_listActionDone.Insert(type_ac, 0);
					}
				});
				list_action.insert(list_action.size(), action_complete);

				action = Sequence::create(list_action);
			}
			action->setTag(type_ac);
			action->retain();
			AddAction(type_ac);
			action->autorelease();
			if (type_ac == BA_PROGRESS)
			{
				p_ObjectTimer->setType(ProgressTimer::Type::RADIAL);
				p_ObjectTimer->runAction(action);
			}
			else
			{
				GetObject()->runAction(action);
			}
		}

		void BasicObject::SetTimeMoveTo(float time)
		{
			if (time <= 0.f)
			{
				xml::BasicDec* _dec = XMLMgr->GetBasicByName(p_name_cotain)->Position_dec;
				if (_dec)
				{
					time_move_to = _dec->GetDatafloat("time");
				}
			}
			else
			{
				time_move_to = time;
			}
		}

		void BasicObject::OnSetProcessScaleActionByDec(xml::BasicDec* _dec, float delta_scale, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (delta_scale == 0 && !forceSet)
			{
				delta_scale = _dec->GetDatafloat("scale") * GetGameSize().width;
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}

			if (loop != 0)
			{
				Vec2 size_scale_to = Vec2(GetSize().x + delta_scale, GetSize().y + delta_scale);
				if (size_scale_to.x < 0) { size_scale_to.x = 0; }
				if (size_scale_to.y < 0) { size_scale_to.y = 0; }
				auto action_process = ScaleBy::create(time, size_scale_to.x / GetSize().x);
				OnSetActionCore(BA_ZOOM_TO, action_process, reserve ? action_process->reverse() : nullptr, loop, delayTime, delay_first);
			}
		}
		
		void BasicObject::OnSetProcessRotateActionByDec(xml::BasicDec* _dec, float angle_rotate, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (angle_rotate <= 0 && !forceSet)
			{
				angle_rotate = _dec->GetDatafloat("angle");
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}

			if (angle_rotate != 0 && loop != 0)
			{
				auto action_process = RotateBy::create(time, angle_rotate);
				auto action_process_reserve = RotateBy::create(time, angle_rotate > 360 ? 360 : 360 - angle_rotate);
				OnSetActionCore(BA_ROTATE_TO, action_process, reserve ? action_process_reserve : nullptr, loop, delayTime, delay_first);
			}
		}
		
		void BasicObject::OnSetProcessFadeActionByDec(xml::BasicDec* _dec, int fade_from, int fade_to, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (fade_to < 0 && !forceSet)
			{
				fade_to = _dec->GetDataInt("first");
			}

			if (fade_from < 0 && !forceSet)
			{
				fade_from = _dec->GetDataInt("origin");
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}

			if (fade_to != fade_from && loop != 0)
			{
				auto action_process = FadeTo::create(time, fade_to);
				auto action_process_reserve = FadeTo::create(time, fade_from);
				OnSetActionCore(BA_FADE_TO, action_process, reserve ? action_process_reserve : nullptr, loop, delayTime, delay_first);
			}
		}
		
		void BasicObject::OnSetProcessColorActionByDec(xml::BasicDec* _dec, Color4B color_to, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (color_to != Color4B::WHITE && !forceSet)
			{
				color_to = _dec->GetDataColor("target");
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}

			if (color_to != Color4B::WHITE && loop != 0)
			{
				auto action_process = TintBy::create(time, color_to.r, color_to.g, color_to.b);
				OnSetActionCore(BA_TINT_TO, action_process, reserve ? action_process->reverse() : nullptr, loop, delayTime, delay_first);
			}
		}

		void BasicObject::OnSetProcessMoveDelayToActionByDec(xml::BasicDec* _dec, Vec2 position, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (position == Vec2(0,0) && !forceSet)
			{
				position.x = _dec->GetDataVector2("pos_move_to").x * GetGameSize().width;
				position.y = _dec->GetDataVector2("pos_move_to").y * GetGameSize().height;
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}
			if (position != Vec2(0, 0) && loop != 0)
			{
				auto action_process = MoveTo::create(time, position);
				OnSetActionCore(BA_MOVE_DELAY_TO, action_process, reserve ? action_process->reverse() : nullptr, loop, delayTime, delay_first);
			}
		}
		
		void BasicObject::OnSetProcessMoveDelayByActionByDec(xml::BasicDec* _dec, Vec2 Vector_move, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (Vector_move == Vec2(0, 0) && !forceSet)
			{
				Vector_move.x = _dec->GetDataVector2("pos_move_to").x * GetGameSize().width;
				Vector_move.y = _dec->GetDataVector2("pos_move_to").y * GetGameSize().height;
			}

			Vec2 cur_pos = GetObject()->getPosition();
			Vec2 vector_move = Vec2(Vector_move.x - cur_pos.x, Vector_move.y - cur_pos.y);

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}
			if (vector_move != Vec2(0, 0) && loop != 0)
			{
				auto action_process = MoveBy::create(time, vector_move);
				OnSetActionCore(BA_MOVE_DELAY_BY, action_process, reserve ? action_process->reverse() : nullptr, loop, delayTime, delay_first);
			}
		}

		void BasicObject::OnSetProcessProcessByActionByDec(xml::BasicDec* _dec, float percent, float loop, float time, bool forceSet, bool reserve, float delayTime, float delay_first)
		{
			if (percent <= 0 && !forceSet)
			{
				percent = _dec->GetDatafloat("percent");
			}

			if (loop <= 0 && !forceSet)
			{
				loop = _dec->GetDataInt("loop");
			}

			if (time <= 0 && !forceSet)
			{
				time = _dec->GetDatafloat("time");
			}

			if (delay_first <= 0 && !forceSet && _dec)
			{
				delay_first = _dec->GetDataInt("delay_first");
			}

			if (percent != 0 && loop != 0)
			{
				auto action_process = ProgressTo::create(time, percent);
				auto action_reseve = ProgressTo::create(0, 0);
				OnSetActionCore(BA_PROGRESS, action_process, reserve ? action_reseve : nullptr, loop, delayTime, delay_first);
			}
		}

		void BasicObject::SetToMoveToPoint(Vec2 point, float time_move)
		{
			BASIC_ACTION basic_action = BA_MOVE_TO;

			bool have_move_by = ContainAction(basic_action);

			if (have_move_by)
			{
				RemoveAction(basic_action);

			}
			auto action_move = MoveTo::create(time_move, point);
			auto action_complete = CallFunc::create(
				[=]()
			{
				//after move complete just remove it out of the list
				bool res = OnFinishAction(basic_action);
				if (res)
				{
					p_listActionDone.Insert(basic_action, 0);
				}

			});
			auto action = Sequence::create(
				action_move, action_complete, nullptr
				);
			action->setTag(basic_action);
			action->retain();
			action->autorelease();
			AddAction(basic_action);
			GetObject()->runAction(action);
		}

		void BasicObject::SetToMoveByVector(Vec2 point, float time_move)
		{
			Vec2 cur_pos = GetObject()->getPosition();
			Vec2 vector_move = Vec2(point.x - cur_pos.x, point.y - cur_pos.y);
			//vector_move.normalize();
			BASIC_ACTION basic_action = BA_MOVE_BY;


			bool have_move_by = ContainAction(basic_action);

			if (have_move_by)
			{
				//also remove from action manager
				RemoveAction(basic_action);
			}
			auto action_move = MoveBy::create(time_move, vector_move);
			auto action_complete = CallFunc::create(
				[=]()
			{
				//after move complete just remove it out of the list
				//also remove from action manager
				bool res = OnFinishAction(basic_action);
				if (res)
				{
					p_listActionDone.Insert(basic_action, 0);
				}
			});
			auto action = Sequence::create(
				action_move, action_complete, nullptr
				);
			action->setTag(basic_action);
			action->retain();
			AddAction(basic_action);
			action->autorelease();
			GetObject()->runAction(action);
		}


		void BasicObject::LoadValueProcessObject(xml::ObjectDec * object_dec, BASIC_ACTION LoadState)
		{
			if (object_dec)
			{
				/*

					set the action process in game
				
				*/
				xml::BasicDec * process_dec = NULL;
				/*
						M O V E _ P R O C C E S S ?
				*/
				if (LoadState == BA_MOVE_TO || LoadState == BA_MOVE_BY || LoadState == BA_ALL)
				{
					process_dec = object_dec->Position_dec;
					if (process_dec)
					{
						SetTimeMoveTo(process_dec->GetDatafloat("time"));
					}
				}

				/*
				M O V E _ D E L A Y _ P R O C C E S S ?
				*/
				if (LoadState == BA_MOVE_DELAY_TO || LoadState == BA_MOVE_DELAY_BY || LoadState == BA_ALL)
				{
					process_dec = object_dec->Move_delay_dec;
					if (process_dec)
					{
						Vec2 position = Vec2(0, 0);
						position.x = process_dec->GetDataVector2("pos_move_to").x  * GetGameSize().width;
						position.y = process_dec->GetDataVector2("pos_move_to").y * GetGameSize().height;
						int loop = process_dec->GetDataInt("loop");
						float time = process_dec->GetDatafloat("time");
						float delay_time = process_dec->GetDatafloat("delay");
						if (delay_time != 0)
						{
							if (LoadState == BA_MOVE_DELAY_TO || LoadState == BA_ALL)
							{
								OnSetProcessMoveDelayToAction(position, loop, time, true, true, delay_time);
							}
							else
							{
								OnSetProcessMoveDelayByAction(position, loop, time, true, true, delay_time);
							}
						}

					}
				}

				/*
						S C A L E _ P R O C C E S S ?
				*/
				if (LoadState == BA_ZOOM_TO || LoadState == BA_ALL)
				{
					process_dec = object_dec->Size_dec;
					if (process_dec)
					{
						float delta_scale = process_dec->GetDatafloat("scale") * GetGameSize().width;
						int loop = process_dec->GetDataInt("loop");
						float time = process_dec->GetDatafloat("time");
						//
						OnSetProcessScaleAction(delta_scale, loop, time, true);
					}
				}

				/*
						R O T A T E _ P R O C C E S S ?
				*/
				if (LoadState == BA_ROTATE_TO || LoadState == BA_ALL)
				{
					process_dec = object_dec->Rotate_dec;
					if (process_dec)
					{
						float angle_rotate = process_dec->GetDatafloat("angle");
						int		loop = process_dec->GetDataInt("loop");
						float	time = process_dec->GetDatafloat("time");

						OnSetProcessRotateAction(angle_rotate, loop, time, true);
					}
				}

				/*
						F A D E _ P R O C C E S S ?
				*/
				if (LoadState == BA_FADE_TO || LoadState == BA_ALL)
				{
					process_dec = object_dec->Opacity_dec;
					if (process_dec)
					{
						RKUtils::BYTE  fade_to = process_dec->GetDataInt("first");
						int		loop = process_dec->GetDataInt("loop");
						float	time = process_dec->GetDatafloat("time");
						RKUtils::BYTE  fade_from = process_dec->GetDataInt("origin");

						OnSetProcessFadeAction(fade_from, fade_to, loop, time, true);
					}
				}

				/*
						C O L O R _ P R O C C E S S ?
				*/
				if (LoadState == BA_TINT_TO || LoadState == BA_ALL)
				{
					process_dec = object_dec->Color_dec;
					if (process_dec)
					{
						Color4B color_to = process_dec->GetDataColor("target");
						int loop = process_dec->GetDataInt("loop");
						float time = process_dec->GetDatafloat("time");

						OnSetProcessColorAction(color_to, loop, time, true);
					}
				}
				/*
						P R O C E S S _ P R O C C E S S ?
				*/
				if (LoadState == BA_PROGRESS || LoadState == BA_ALL)
				{
					process_dec = object_dec->Process_dec;
					if (process_dec)
					{
						float percent = process_dec->GetDatafloat("float");
						int loop = process_dec->GetDataInt("loop");
						float time = process_dec->GetDatafloat("time");

						OnSetProcessProcessByAction(percent, loop, time, true);
					}
				}

			}
		}

		void BasicObject::DrawObject(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (!p_IsVisible)
				return;
			GetObject()->draw(renderer, transform, flags);
		}

		void BasicObject::VisitObject(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (!p_IsVisible)
				return;
			GetObject()->visit(renderer, transform, flags);
		}

		bool BasicObject::UpdateObject(float dt)
		{
			if (!p_IsOnUpdate)
				return false;

			UpdateActionProcess(dt);

			return true;
		}

		/*

		TODO GET / SET method of basic object

		*/

		void BasicObject::SetSize(Vec2 Size)
		{
			CCASSERT(GetObject(), "object sprite invaid");
			Vec2 p_Size(1, 1);
			//
			p_Size.x = Size.x / GetObject()->getContentSize().width;
			p_Size.y = Size.y / GetObject()->getContentSize().height;
			if (p_IsText)
			{
				GetObject()->setScaleX(p_Size.x);
				GetObject()->setScaleY(p_Size.x);
			}
			else
			{
				GetObject()->setScaleX(p_Size.x);
				GetObject()->setScaleY(p_Size.y);
			}

		}

		Vec2 BasicObject::GetSize()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			Vec2 p_Size(1, 1);
			//
			p_Size.x = GetObject()->getContentSize().width * GetObject()->getScaleX();
			p_Size.y = GetObject()->getContentSize().height * GetObject()->getScaleY();
			//
			return p_Size;
		}

		void BasicObject::SetScale(Vec2 scale)
		{
			Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			p_scale.x = GetMulMul(scale.x, mipmap);
			p_scale.y = GetMulMul(scale.y, mipmap);


			CCASSERT(GetObject(), "object sprite invaid");
			GetObject()->setScaleX(p_scale.x);
			GetObject()->setScaleY(p_scale.y);
		}

		Vec2 BasicObject::GetScale()
		{
			Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			CCASSERT(GetObject(), "object sprite invaid");
			p_scale.x = GetMulDiv(GetObject()->getScaleX(), mipmap);
			p_scale.y = GetMulDiv(GetObject()->getScaleY(), mipmap);

			return p_scale;
		}

        void BasicObject::SetOpacity(RKUtils::BYTE opacity)
		{
			CCASSERT(GetObject(), "object sprite invaid");
			GetObject()->setOpacity(opacity);
		}

		RKUtils::BYTE  BasicObject::GetOpacity()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			return GetObject()->getOpacity();
		}

		void BasicObject::SetAngle(float angle)
		{
			CCASSERT(GetObject(), "object sprite invaid");
			GetObject()->setRotation(angle);
		}

		float BasicObject::GetAngle()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			return GetObject()->getRotation();
		}

		void BasicObject::SetOriginColor()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			SetColor(p_origin_color);
		}

		void BasicObject::SetColor(Color4B color)
		{
			CCASSERT(GetObject(), "object sprite invaid");
			GetObject()->setColor(Color3B(color.r, color.g, color.b));
		}

		Color4B BasicObject::GetColor()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			Color3B color = GetObject()->getColor();
			return Color4B(color.r, color.g, color.b, GetOpacity());
		}

		void BasicObject::SetPosition(Vec2 pos)
		{
			CCASSERT(GetObject(), "object sprite invaid");
			GetObject()->setPosition(pos);
		}

		Vec2 BasicObject::GetPosition()
		{
			CCASSERT(GetObject(), "object sprite invaid");
			return GetObject()->getPosition();
		}

        cocos2d::Rect BasicObject::GetBound()
		{
			Vec2 pos = GetPosition();
			Vec2 size = GetSize();
            cocos2d::Rect rec(
				pos.x - size.x / 2,
				pos.y - size.y / 2,
				size.x,
				size.y
				);
			return rec;
		}

		bool BasicObject::ContainAction(BASIC_ACTION ba)
		{
			for (int i = 0; i < p_listActionProcess.Size(); i++)
			{
				if (p_listActionProcess.GetAt(i).Action == ba)
					return true;
			}
			return false;
		}

		void BasicObject::RemoveAction(BASIC_ACTION ba)
		{
			for (int i = 0; i < p_listActionProcess.Size(); i++)
			{
				if (p_listActionProcess.GetAt(i).Action == ba)
				{
					p_listActionProcess.EraseAt(i);
			

					//auto ac = GetObject()->getActionManager()->getActionByTag(ba, GetObject());
					//if (ac)
					{

						GetObject()->getActionManager()->removeActionByTag(ba, GetObject());
					}
				}
			}
		}

		void BasicObject::AddAction(BASIC_ACTION ba)
		{
			//if (ContainAction(ba))
				//return;
			p_listActionProcess.Insert(STATE_ACTION(ba), 0);
		}

		void BasicObject::OnStopActionIm(BASIC_ACTION ba)
		{
			RemoveAction(ba);
		}

		void BasicObject::OnPauseActionIm(BASIC_ACTION ba)
		{
			if (ContainAction(ba))
				return;
			for (int i = 0; i < p_listActionProcess.Size(); i++)
			{
				if (p_listActionProcess.GetAt(i).Action == ba)
				{
					p_listActionProcess.EraseAt(i);
					p_listActionProcess.Insert(STATE_ACTION(ba, STATUS_ACTION::STATUS_IDLE), i);
					return;
				}
			}
		}

		void BasicObject::OnResumeActionIm(BASIC_ACTION ba)
		{
			if (ContainAction(ba))
				return;
			for (int i = 0; i < p_listActionProcess.Size(); i++)
			{
				if (p_listActionProcess.GetAt(i).Action == ba)
				{
					p_listActionProcess.EraseAt(i);
					p_listActionProcess.Insert(STATE_ACTION(ba, STATUS_ACTION::STATUS_ACTIVE), i);
					return;
				}
			}
		}

		bool BasicObject::OnFinishAction(BASIC_ACTION ba)
		{
			//abtrast class dont implement this function.....
			return true;
		}

		void BasicObject::EventActionFinish()
		{
			//abtrast class dont implement this function...
		}

		void BasicObject::SetBlendFunc(BlendFunc blend)
		{
			if (p_IsText)
			{
				p_textLabel->setBlendFunc(blend);
				return;
			}

			if (p_Object)
			{
				p_Object->setBlendFunc(blend);
			}
		}
	}
}