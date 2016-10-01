#include "ActionCommand.h"
#include "LayerEntity.h"
#include "../../LanguageMgr.h"
namespace Utility
{
	namespace UI_Widget
	{
		ActionCommand::ActionDetail * ActionCommand::ActionDetail::clone()
		{
			ActionDetail * clone = new ActionDetail();
			if (clone->value_action)
				delete clone->value_action;

			clone->value_action = value_action->Clone();
			if (_hook_value)
			{
				clone->_hook_value = _hook_value->Clone();
			}
			else
			{
				clone->_hook_value = nullptr;
			}
			clone->name_action = name_action;
			clone->time_action = time_action;

			return clone;
		}

		void ActionCommand::ActionDetail::ClearHookCommand(RKString name_value)
		{
			if (_hook_value)
			{
				delete _hook_value;
				_hook_value = nullptr;
			}
		}

		void ActionCommand::ActionDetail::SetValueHook(xml::BasicDec value, RKString name_value_t)
		{
			if (!_hook_value)
			{
				_hook_value = new xml::BasicDec();
			}
			std::string name_value = name_value_t.GetString();
			if (name_value == "TIME")
			{
				SetTimeAction(value.GetDatafloat(name_value));
				return;
			}

			if (value.HaveDataInt(name_value))
			{
				_hook_value->InsertDataInt(name_value, value.GetDataInt(name_value));
			}
			if (value.HaveDatafloat(name_value))
			{
				_hook_value->InsertDatafloat(name_value, value.GetDatafloat(name_value));
			}
			if (value.HaveDataChar(name_value))
			{
				_hook_value->InsertDataChar(name_value, value.GetDataChar(name_value));
			}
			if (value.HaveDataVector2(name_value))
			{
				_hook_value->InsertDataVector2(name_value, value.GetDataVector2(name_value));
			}
			if (value.HaveDataColor(name_value))
			{
				_hook_value->InsertDataColor(name_value, value.GetDataColor(name_value));
			}

		}

		ActionCommand::ActionRepeat * ActionCommand::ActionRepeat::clone()
		{
			ActionRepeat * ac_clone = new ActionRepeat();
			for (auto ls : p_list_action_)
			{
				std::vector<ActionDetail *> ac_ls;
				for (auto ac : ls)
				{
					ac_ls.push_back(ac->clone());
				}
				ac_clone->p_list_action_.push_back(ac_ls);
			};
			return ac_clone;
		}

		int ActionCommand::GetNumberDetailActInSquence(RKString name_act, int idx_squence)
		{
			if (p_list_actions.size() <= 0 || p_list_actions.find(name_act) == p_list_actions.end())
				return 0;
			auto list_act_detail_squence = p_list_actions.at(name_act);
			if (idx_squence < 0 || idx_squence >= (int)list_act_detail_squence->p_list_action_.size())
				return 0;
			auto act_detail_squence = list_act_detail_squence->p_list_action_.at(idx_squence);
			return (int)act_detail_squence.size();
		}

		int ActionCommand::GetNumberSquenceInAct(RKString name_act)
		{
			if (p_list_actions.size() <= 0 || p_list_actions.find(name_act) == p_list_actions.end())
				return 0;
			auto list_act_detail_squence = p_list_actions.at(name_act);

			return (int)list_act_detail_squence->p_list_action_.size();
		}

		std::vector<RKString> ActionCommand::GetNumberActInCmd()
		{
			std::vector<RKString> return_value;
			for (auto it : p_list_actions)
			{
				return_value.push_back(it.first);
			}
			return return_value;
		}

		ActionCommand::ActionDetail * ActionCommand::GetActionDetail(RKString name_act, int idx_squence, int idx_detail)
		{
			ActionCommand::ActionDetail * return_value = nullptr;

			if (p_list_actions.size() <= 0 || p_list_actions.find(name_act) == p_list_actions.end())
				return nullptr;

			auto list_act_detail_squence = p_list_actions.at(name_act);

			if (idx_squence < 0 || idx_squence >= (int)list_act_detail_squence->p_list_action_.size())
				return nullptr;

			auto act_detail_squence = list_act_detail_squence->p_list_action_.at(idx_squence);

			if (idx_detail < 0 || idx_detail >= (int)act_detail_squence.size())
				return nullptr;

			return act_detail_squence.at(idx_detail);
		}

		void ActionCommand::ActionDetail::ParseActionDetail(const Json::Value value, RKString name_value_t)
		{
			bool AddName = true;
			std::string name_value = name_value_t.GetString();
			if (name_value == "SIZE")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 size = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, size);
			}
			else if (name_value == "OPACITY")
			{
				unsigned int detail_value = value[name_value].asUInt();
				value_action->InsertDataInt(name_value, detail_value);
			}
			else if (name_value == "POSITION_3D")
			{
				RKString detail_value = value[name_value].asString();
				Vec3 pos = XMLMgr->ConvertToVector3(detail_value);
				value_action->InsertDataVector3(name_value, pos);
			}
			else if (name_value == "POSITION")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 pos = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, pos);
			}
			else if (name_value == "ROTATE")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "ROTATE_Z")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "ROTATE_AROUND")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 pos = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, pos);
			}
			else if (name_value == "ANGLE_ROTATE_AROUND")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "SCALE")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 scale = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, scale);
			}
			else if (name_value == "ANIMATE_TAG")
			{
				unsigned int detail_value = value[name_value].asUInt();
				value_action->InsertDataInt(name_value, detail_value);
			}
			else if (name_value == "TIME")
			{
				float detail_value = value[name_value].asFloat();
				time_action = detail_value;
				AddName = false;
			}
			
			else if (name_value == "DELAY")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "DESIGN")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 design_size = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, design_size);
				AddName = false;
			}
			else if (name_value == "FORCE")
			{
				int IsForce = value[name_value].asInt();
				value_action->InsertDataInt(name_value, IsForce);
				AddName = false;
			}
			else if (name_value == "STICK")
			{
				int IsStick = value[name_value].asInt();
				value_action->InsertDataInt(name_value, IsStick);
				AddName = false;
			}
			else if (name_value == "9SCALE")
			{
				int Is9Scale = value[name_value].asInt();
				value_action->InsertDataInt(name_value, Is9Scale);
				AddName = false;
			}
			else if (name_value == "ORIGIN_SIZE")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 origin_size = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, origin_size);
				AddName = false;
			}
			else if (name_value == "STYLE")
			{
				int Style = value[name_value].asInt();
				value_action->InsertDataInt(name_value, Style);
				AddName = false;
			}
			else if (name_value == "SET_POSITION")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 pos = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, pos);
			}
			else if (name_value == "SET_POSITION_3D")
			{
				RKString detail_value = value[name_value].asString();
				Vec3 pos = XMLMgr->ConvertToVector3(detail_value);
				value_action->InsertDataVector3(name_value, pos);
			}
			else if (name_value == "SET_ROTATE")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "SET_ROTATE_Z")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
			}
			else if (name_value == "SET_SIZE")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 size = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, size);
			}
			else if (name_value == "SET_SCALE")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 size = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, size);
			}
			else if (name_value == "SET_OPACITY")
			{
				unsigned int detail_value = value[name_value].asUInt();
				value_action->InsertDataInt(name_value, detail_value);
			}
			else if (name_value == "SET_TEXT")
			{
				RKString detail_value = value[name_value].asString();
				value_action->InsertDataChar(name_value, detail_value.GetString());
			}
			else if (name_value == "SET_COLOR")
			{
				RKString detail_value = value[name_value].asCString();
				value_action->InsertDataColor(name_value, XMLMgr->ConvertToColor(detail_value));
			}
			else if (name_value == "COLOR")
			{
				RKString detail_value = value[name_value].asCString();
				value_action->InsertDataColor(name_value, XMLMgr->ConvertToColor(detail_value));
			}
			else if (name_value == "SET_VISIBLE")
			{
				int detail_value = value[name_value].asInt();
				value_action->InsertDataInt(name_value, detail_value);
			}
			else if (name_value == "SET_ANCHOR")
			{
				RKString detail_value = value[name_value].asString();
				value_action->InsertDataVector2(name_value, XMLMgr->ConvertToVector2(detail_value));
			}
			else if (name_value == "BEZIER" || name_value == "BEZIER_P1" || name_value == "BEZIER_P2")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 pos = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, pos);
			}
			else if (name_value == "JUMP_TO")
			{
				RKString detail_value = value[name_value].asString();
				Vec2 pos = XMLMgr->ConvertToVector2(detail_value);
				value_action->InsertDataVector2(name_value, pos);
			}
			else if (name_value == "JUMP_NUM")
			{
				unsigned int detail_value = value[name_value].asUInt();
				value_action->InsertDataInt(name_value, detail_value);
				AddName = false;
			}
			else if (name_value == "JUMP_HEIGHT")
			{
				float detail_value = value[name_value].asFloat();
				value_action->InsertDatafloat(name_value, detail_value);
				AddName = false;
			}

			if (AddName)
			{
				name_action = name_value;
				if (name_action == "BEZIER_P2")
				{
					name_action = "BEZIER";
				}
			}
		}

		FiniteTimeAction * ActionCommand::ActionDetail::GetAction(int & action_tag, cocos2d::Node * it_res, cocos2d::Node * parent, bool IsLayer)
		{
			FiniteTimeAction * action = nullptr;

			if (name_action == "SIZE")
			{
				Vec2 size = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					size = _hook_value->GetDataVector2(name_action);
				}

				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				bool IsStick = value_action->GetDataInt("STICK") != 0;
				bool IsNinePath = value_action->GetDataInt("9SCALE") != 0;
				size = Vec2(size.x * (GetGameSize().width / design_size.x),
					size.y * (IsStick ? (GetGameSize().height / design_size.y) : (GetGameSize().width / design_size.x)));
				if (parent != nullptr)
				{
					size.x = size.x * (1.f / parent->getScaleX());
					size.y = size.y * (1.f / parent->getScaleY());
				}

				Vec2 CurrentSize = (it_res)->getContentSize();
				Vec2 scale = Vec2((size.x / CurrentSize.x), (size.y / CurrentSize.y));
				if (!IsNinePath)
				{
					action = ScaleTo::create(time_action, scale.x, scale.y);
				}
				else
				{
					Vec2 from_size = value_action->GetDataVector2("ORIGIN_SIZE");
					from_size = Vec2(
						from_size.x * (GetGameSize().width / design_size.x),
						from_size.y * (IsStick ? (GetGameSize().height / design_size.y) : (GetGameSize().width / design_size.x)));
					if (parent != nullptr)
					{
						from_size.x = from_size.x * (1.f / parent->getScaleX());
						from_size.y = from_size.y * (1.f / parent->getScaleY());
					}
					action = SizeTo::create(time_action, size.x, size.y, from_size.x, from_size.y);
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_ZOOM;
			}
			else if (name_action == "SET_SIZE")
			{
				Vec2 size = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					size = _hook_value->GetDataVector2(name_action);
				}
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				bool IsStick = value_action->GetDataInt("STICK") != 0;
				bool IsNinePath = value_action->GetDataInt("9SCALE") != 0;
				size = Vec2(size.x * (GetGameSize().width / design_size.x),
					size.y * (IsStick ? (GetGameSize().height / design_size.y) : (GetGameSize().width / design_size.x)));

				if (parent != nullptr)
				{
					size.x = size.x * (1.f / parent->getScaleX());
					size.y = size.y * (1.f / parent->getScaleY());
				}

				Vec2 CurrentSize = (it_res)->getContentSize();
				Vec2 scale = Vec2((size.x / CurrentSize.x), (size.y / CurrentSize.y));
				action_tag += 31;
				if (!IsNinePath)
				{
					action = IScale::create(scale);
				}
				else
				{
					action = CallFunc::create([it_res, size]() {
						(it_res)->setContentSize(cocos2d::Size(size.x, size.y));
					});
				}
			}
			else if (name_action == "OPACITY")
			{
				int value = value_action->GetDataInt(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataInt(name_action))
				{
					value = _hook_value->GetDataInt(name_action);
				}
				action = FadeTo::create(time_action, (unsigned char)value);
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_TRANS;
			}
			else if (name_action == "SET_OPACITY")
			{
				int value = value_action->GetDataInt(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataInt(name_action))
				{
					value = _hook_value->GetDataInt(name_action);
				}
				action = IOpacity::create((unsigned char)value);
				/*action = CallFunc::create([it_res, value]() {
					it_res->setOpacity(value);
				});*/
				action_tag += 26;
			}
			else if (name_action == "SET_VISIBLE")
			{
				int value = value_action->GetDataInt(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataInt(name_action))
				{
					value = _hook_value->GetDataInt(name_action);
				}
				if (value == 1)
				{
					action = Show::create();
				}
				else
				{
					action = Hide::create();
				}
				action_tag += 23;
			}
			else if (name_action == "SET_COLOR")
			{
				Color4B value = value_action->GetDataColor(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataColor(name_action))
				{
					value = _hook_value->GetDataColor(name_action);
				}
				action = IColor::create(value);
				/*action = CallFunc::create([it_res, value]() {
					it_res->setColor(Color3B(value));
				});*/
				action_tag += 19;
			}
			else if (name_action == "COLOR")
			{
				Color4B value = value_action->GetDataColor(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataColor(name_action))
				{
					value = _hook_value->GetDataColor(name_action);
				}

				int style = value_action->GetDataInt("STYLE") != 0;
				if (style == 0)
				{
					action = TintTo::create(time_action, Color3B(value.r, value.g, value.b));
				}
				else
				{
					Color3B current_color = it_res->getColor();
					short delta_r = value.r - current_color.r;
					short delta_g = value.g - current_color.g;
					short delta_b = value.b - current_color.b;

					action = TintBy::create(time_action, delta_r , delta_g, delta_b);
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_TINT;
			}
			else if (name_action == "SET_TEXT")
			{
				RKString value = value_action->GetDataChar(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataChar(name_action))
				{
					value = _hook_value->GetDataChar(name_action);
				}
				action_tag += 17;
				action = CallFunc::create([it_res, value]() {
					static_cast<Text*>(it_res)->setString(LANG_STR(value).GetString());
				});
			}
			else if (name_action == "ROTATE")
			{
				float value = value_action->GetDatafloat(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDatafloat(name_action))
				{
					value = _hook_value->GetDatafloat(name_action);
				}
				int style = value_action->GetDataInt("STYLE") != 0;
				if (style == 0)
				{
					action = RotateTo::create(time_action, value);
				}
				else
				{
					action = RotateBy::create(time_action, value);
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_ROTATE;
			}
			else if (name_action == "ROTATE_Z")
			{
				float value = value_action->GetDatafloat(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDatafloat(name_action))
				{
					value = _hook_value->GetDatafloat(name_action);
				}
				int style = value_action->GetDataInt("STYLE") != 0;
				Vec3 current_rotate = it_res->getRotation3D();
				if (style == 0)
				{
					action = RotateTo::create(time_action, Vec3(current_rotate.x, value, current_rotate.z));
				}
				else
				{
					action = RotateBy::create(time_action, Vec3(current_rotate.x, value, current_rotate.z));
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_ROTATE;
			}
			else if (name_action == "ROTATE_AROUND")
			{
				auto value = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					value = _hook_value->GetDataVector2(name_action);
				}
				auto begin_angle = value_action->GetDatafloat("ANGLE_ROTATE_AROUND");
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				Vec2 point_rotate = Vec2(value.x * GetGameSize().width / design_size.x, value.y * GetGameSize().height / design_size.y);
				action = RotateAround::create(
					time_action, begin_angle, point_rotate
				);

				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_ROTATE_AROUND;
			}
			else if (name_action == "SET_ROTATE")
			{
				float value = value_action->GetDatafloat(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDatafloat(name_action))
				{
					value = _hook_value->GetDatafloat(name_action);
				}
				action_tag += 13;
				action = IRotate::create(value);
				/*action = CallFunc::create([it_res, value]() {
					it_res->setRotation(value);
				});*/
			}
			else if (name_action == "SET_ROTATE_Z")
			{
				float value = value_action->GetDatafloat(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDatafloat(name_action))
				{
					value = _hook_value->GetDatafloat(name_action);
				}
				action_tag += 11;
				//action = IRotateZ::create(value);
				Vec3 current_rotate = it_res->getRotation3D();
				action = CallFunc::create([it_res, value , current_rotate]() {
					it_res->setRotation3D(Vec3(current_rotate.x, value, current_rotate.z));
				});
			}
			else if (name_action == "SET_POSITION")
			{
				Vec2 value = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					value = _hook_value->GetDataVector2(name_action);
				}
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				int force = value_action->GetDataInt("FORCE");

				Vec2 pos = Vec2(0, 0);
				if (force == 0)
				{
					pos = Vec2(value.x * (GetGameSize().width / design_size.x),
						value.y * (GetGameSize().height / design_size.y));
				}
				else
				{
					pos = value;
				}

				if (IsLayer)
				{
					pos = ConvertPos1(pos);
					pos.x -= (it_res->getContentSize().width / 2);
					pos.y -= (it_res->getContentSize().height / 2);
				}

				if (parent != nullptr)
				{
					pos.x = pos.x * (1.f / parent->getScaleX());
					pos.y = pos.y * (1.f / parent->getScaleY());
				}
				action_tag += 9;
				action = Place::create(pos);
				/*action = CallFunc::create([it_res, pos]() {
					it_res->setPosition(pos);
				});*/
			}
			else if (name_action == "SET_POSITION_3D")
			{
				Vec3 value = value_action->GetDataVector3(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector3(name_action))
				{
					value = _hook_value->GetDataVector3(name_action);
				}
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				int force = value_action->GetDataInt("FORCE");

				Vec3 pos = Vec3(0, 0,0);
				if (force == 0)
				{
					pos = Vec3(value.x * (GetGameSize().width / design_size.x),
						value.y * (GetGameSize().height / design_size.y),
						value.z
					);
				}
				else
				{
					pos = value;
				}

				
				if (parent != nullptr)
				{
					pos.x = pos.x * (1.f / parent->getScaleX());
					pos.y = pos.y * (1.f / parent->getScaleY());
					pos.z = pos.z * (1.f / parent->getScaleZ());
				}
				action_tag += 7;
				action = Place3D::create(pos);
				/*action = CallFunc::create([it_res, pos]() {
					it_res->setPosition3D(pos);
				});*/
			}
			else if (name_action == "POSITION")
			{
				Vec2 value = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					value = _hook_value->GetDataVector2(name_action);
				}
				int style = value_action->GetDataInt("STYLE");
				int force = value_action->GetDataInt("FORCE");
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				if (style == 0)
				{
					Vec2 pos = Vec2(0, 0);
					if (force == 0)
					{
						pos = Vec2(value.x * (GetGameSize().width / design_size.x),
							value.y * (GetGameSize().height / design_size.y));
					}
					else
					{
						pos = value;
					}



					if (IsLayer)
					{
						pos = ConvertPos1(pos);
						pos.x -= (it_res->getContentSize().width / 2);
						pos.y -= (it_res->getContentSize().height / 2);
					}

					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
					}

					action = MoveTo::create(time_action, pos);
				}
				else if (style == 1)
				{
					Vec2 delta_pos = Vec2(value.x - (it_res)->getPosition().x, value.y - (it_res)->getPosition().y);
					Vec2 pos = Vec2(0, 0);
					if (force == 0)
					{
						Vec2 pos = Vec2(delta_pos.x * (GetGameSize().width / design_size.x),
							delta_pos.y * (GetGameSize().height / design_size.y));
					}
					else
					{
						pos = delta_pos;
					}


					if (IsLayer)
					{
						pos.x -= (it_res->getContentSize().width / 2);
						pos.y -= (it_res->getContentSize().height / 2);
					}

					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
					}

					action = MoveBy::create(time_action, pos);
				}
				else if (style == 2)
				{
					Vec2 delta_pos = value;
					Vec2 pos = Vec2(0, 0);
					if (force == 0)
					{
						pos = Vec2(delta_pos.x * (GetGameSize().width / design_size.x),
							delta_pos.y * (GetGameSize().height / design_size.y));
					}
					else
					{
						pos = delta_pos;
					}

					if (IsLayer)
					{
						pos.x -= (it_res->getContentSize().width / 2);
						pos.y -= (it_res->getContentSize().height / 2);
					}

					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
					}

					action = MoveBy::create(time_action, pos);
				}
				else if (style == 3)
				{
					Vec2 pos = Vec2(0, 0);
					Vec2 current_pos = it_res->getPosition();
					if (force == 0)
					{
						pos = Vec2(value.x * (GetGameSize().width / design_size.x),
							value.y * (GetGameSize().height / design_size.y));
					}
					else
					{
						pos = value;
					}

					pos.x = current_pos.x + pos.x;
					pos.y = current_pos.y + pos.y;

					if (IsLayer)
					{
						pos = ConvertPos1(pos);
						pos.x -= (it_res->getContentSize().width / 2);
						pos.y -= (it_res->getContentSize().height / 2);
					}

					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
					}

					action = MoveTo::create(time_action, pos);
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_MOVE;
			}
			else if (name_action == "POSITION_3D")
			{
				Vec3 value = value_action->GetDataVector3(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector3(name_action))
				{
					value = _hook_value->GetDataVector3(name_action);
				}
				int style = value_action->GetDataInt("STYLE");
				int force = value_action->GetDataInt("FORCE");
				Vec2 design_size = value_action->GetDataVector2("DESIGN");
				if (style == 0)
				{
					Vec3 pos = Vec3(0, 0, 0);
					if (force == 0)
					{
						pos = Vec3(
							value.x * (GetGameSize().width / design_size.x),
							value.y * (GetGameSize().height / design_size.y),
							value.z
							);
					}
					else
					{
						pos = value;
					}


					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
						pos.z = pos.z * (1.f / parent->getScaleZ());
					}

					action = MoveTo::create(time_action, pos);
				}
				else if (style == 1)
				{
					Vec3 delta_pos = Vec3(
						value.x - (it_res)->getPosition3D().x, 
						value.y - (it_res)->getPosition3D().y,
						value.z - (it_res)->getPosition3D().z);
					Vec3 pos = Vec3(0, 0 , 0);
					if (force == 0)
					{
						Vec3 pos = Vec3(
							delta_pos.x * (GetGameSize().width / design_size.x ),
							delta_pos.y * (GetGameSize().height / design_size.y),
							delta_pos.z );
					}
					else
					{
						pos = delta_pos;
					}


					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
						pos.z = pos.z * (1.f / parent->getScaleZ());
					}

					action = MoveBy::create(time_action, pos);
				}
				else if (style == 2)
				{
					Vec3 delta_pos = value;
					Vec3 pos = Vec3(0, 0 , 0);
					if (force == 0)
					{
						pos = Vec3(
							delta_pos.x * (GetGameSize().width / design_size.x),
							delta_pos.y * (GetGameSize().height / design_size.y),
							delta_pos.z);
					}
					else
					{
						pos = delta_pos;
					}

					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
						pos.z = pos.z * (1.f / parent->getScaleZ());
					}

					action = MoveBy::create(time_action, pos);
				}
				else if (style == 3)
				{
					Vec3 pos = Vec3(0, 0, 0);
					Vec3 current_pos = it_res->getPosition3D();
					if (force == 0)
					{
						pos = Vec3(
							value.x * (GetGameSize().width / design_size.x),
							value.y * (GetGameSize().height / design_size.y),
							value.z);
					}
					else
					{
						pos = value;
					}

					pos.x = current_pos.x + pos.x;
					pos.y = current_pos.y + pos.y;
					pos.z = current_pos.z + pos.z;


					if (parent != nullptr)
					{
						pos.x = pos.x * (1.f / parent->getScaleX());
						pos.y = pos.y * (1.f / parent->getScaleY());
						pos.z = pos.z * (1.f / parent->getScaleZ());
					}

					action = MoveTo::create(time_action, pos);
				}
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_MOVE;
			}

			else if (name_action == "DELAY")
			{
				float value = value_action->GetDatafloat(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDatafloat(name_action))
				{
					value = _hook_value->GetDatafloat(name_action);
				}
				action = DelayTime::create(value);
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_DELAY;
			}
			else if (name_action == "BEZIER")
			{
				Vec2 value_bezier = value_action->GetDataVector2(name_action);
				Vec2 value_bezier_p1 = value_action->GetDataVector2("BEZIER_P1");
				Vec2 value_bezier_p2 = value_action->GetDataVector2("BEZIER_P2");
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					value_bezier = _hook_value->GetDataVector2(name_action);
				}
				if (_hook_value && _hook_value->HaveDataVector2("BEZIER_P1"))
				{
					value_bezier_p1 = _hook_value->GetDataVector2("BEZIER_P1");
				}
				if (_hook_value && _hook_value->HaveDataVector2("BEZIER_P2"))
				{
					value_bezier_p2 = _hook_value->GetDataVector2("BEZIER_P2");
				}

				int style = value_action->GetDataInt("STYLE");

				Vec2 design_size = value_action->GetDataVector2("DESIGN");

				_ccBezierConfig be_zonfig;

				if (style == 0)
				{
					Vec2 pos_to = Vec2(value_bezier.x * (GetGameSize().width / design_size.x),
						value_bezier.y * (GetGameSize().height / design_size.y));
					Vec2 pos_p1 = Vec2(value_bezier_p1.x * (GetGameSize().width / design_size.x),
						value_bezier_p1.y * (GetGameSize().height / design_size.y));
					Vec2 pos_p2 = Vec2(value_bezier_p2.x * (GetGameSize().width / design_size.x),
						value_bezier_p2.y * (GetGameSize().height / design_size.y));

					if (IsLayer)
					{
						pos_to = ConvertPos1(pos_to);
						pos_to.x -= (it_res->getContentSize().width / 2);
						pos_to.y -= (it_res->getContentSize().height / 2);

						pos_p1 = ConvertPos1(pos_to);
						pos_p1.x -= (it_res->getContentSize().width / 2);
						pos_p1.y -= (it_res->getContentSize().height / 2);

						pos_p2 = ConvertPos1(pos_to);
						pos_p2.x -= (it_res->getContentSize().width / 2);
						pos_p2.y -= (it_res->getContentSize().height / 2);
					}

					be_zonfig.endPosition = pos_to;
					be_zonfig.controlPoint_1 = pos_p1;
					be_zonfig.controlPoint_2 = pos_p2;

					action = BezierTo::create(time_action, be_zonfig);
				}


				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_BENZIER;
			}
			else if (name_action == "JUMP_TO")
			{
				Vec2 value = value_action->GetDataVector2(name_action);
				unsigned int number_jump = value_action->GetDataInt("JUMP_NUM");
				float height_jump = value_action->GetDatafloat("JUMP_HEIGHT");
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					value = _hook_value->GetDataVector2(name_action);
				}
				if (_hook_value && _hook_value->HaveDataVector2("JUMP_NUM"))
				{
					number_jump = _hook_value->GetDataInt("JUMP_NUM");
				}
				if (_hook_value && _hook_value->HaveDataVector2("JUMP_HEIGHT"))
				{
					height_jump = _hook_value->GetDataInt("JUMP_HEIGHT");
				}

				int style = value_action->GetDataInt("STYLE");
				Vec2 design_size = value_action->GetDataVector2("DESIGN");


				if (style == 0)
				{
					Vec2 pos_to = Vec2(value.x * (GetGameSize().width / design_size.x),
						value.y * (GetGameSize().height / design_size.y));

					height_jump = height_jump * (GetGameSize().width / design_size.x);
					if (IsLayer)
					{
						pos_to = ConvertPos1(pos_to);
						pos_to.x -= (it_res->getContentSize().width / 2);
						pos_to.y -= (it_res->getContentSize().height / 2);

					}

					action = JumpTo::create(time_action, pos_to, height_jump, number_jump);
				}
				else if (style == 1)
				{
					Vec2 delta_pos = Vec2(value.x - (it_res)->getPosition().x, value.y - (it_res)->getPosition().y);
					Vec2 pos = Vec2(delta_pos.x * (GetGameSize().width / design_size.x),
						delta_pos.y * (GetGameSize().height / design_size.y));

					height_jump = height_jump * (GetGameSize().width / design_size.x);

					if (IsLayer)
					{
						pos = ConvertPos1(pos);
						pos.x -= (it_res->getContentSize().width / 2);
						pos.y -= (it_res->getContentSize().height / 2);

					}

					action = JumpBy::create(time_action, pos, height_jump, number_jump);
				}

				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_JUMP;
			}
			else if (name_action == "SCALE")
			{
				Vec2 scale = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					scale = _hook_value->GetDataVector2(name_action);
				}
				int force = value_action->GetDataInt("FORCE");
				//current scale
				Vec2 current_scale = Vec2(0, 0);
				if (force == 0)
				{
					current_scale = Vec2(it_res->getScaleX(), it_res->getScaleY());
				}
				else
				{
					current_scale = Vec2(1, 1);
				}

				action = ScaleTo::create(time_action, scale.x * current_scale.x, scale.y * current_scale.y);
				action_tag |= (int)UI_STATE_FADE_ACTION::FADE_ZOOM;
			}
			else if (name_action == "SET_SCALE")
			{
				Vec2 scale = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					scale = _hook_value->GetDataVector2(name_action);
				}
				int force = value_action->GetDataInt("FORCE");
				//current scale
				Vec2 current_scale = Vec2(0, 0);
				if (force == 0)
				{
					current_scale = Vec2(it_res->getScaleX(), it_res->getScaleY());
				}
				else
				{
					current_scale = Vec2(1, 1);
				}
				action = IScale::create(Vec2(scale.x * current_scale.x, scale.y * current_scale.y));
				action_tag += 5;
				/*action = CallFunc::create([it_res, scale, current_scale]() {
						it_res->setScale(scale.x * current_scale.x, scale.y * current_scale.y);
				});*/
			}
			else if (name_action == "ANIMATE_TAG")
			{
				int value = value_action->GetDataInt(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataInt(name_action))
				{
					value = _hook_value->GetDataInt(name_action);
				}
				action_tag += value;
				action = static_cast<Animate*>(it_res->getActionByTag(value));
				action->startWithTarget(it_res);
				return action;
				//action_tag |= (int)UI_STATE_FADE_ACTION::FADE_DELAY;
			}
			else if (name_action == "SET_ANCHOR")
			{
				Vec2 anchor_point = value_action->GetDataVector2(name_action);
				//hook
				if (_hook_value && _hook_value->HaveDataVector2(name_action))
				{
					anchor_point = _hook_value->GetDataVector2(name_action);
				}
				
				action_tag += 3;
				action = IAnchor::create(anchor_point);
				/*action = CallFunc::create([it_res, anchor_point]() {
					it_res->setAnchorPoint(anchor_point);
				});*/
			}
			
			if (action_tag <= 0)
			{
				action_tag = 1001;
			}
#if MANAGE_ACTION
			if (action)
			{
				PoolManager::getInstance()->getCurrentPool()->removeLastObject();
				action->setName(name_action);
			}
#endif
			return action;
		}


		ActionCommand::ActionCommand()
		{
			p_list_actions.clear();
		}

		ActionCommand::~ActionCommand()
		{
			for (auto it = p_list_actions.begin(); it != p_list_actions.end(); it++)
			{
				delete p_list_actions.at(it->first);
			}
			p_list_actions.clear();
		}

		bool  ActionCommand::ReadJsonString(const char * jstr)
		{
			Json::Reader jreader;

			bool parsingSuccessful = jreader.parse(jstr, JRootValue);
			if (!parsingSuccessful)
			{
				// report to the user the failure and their locations in the document.
				PINFO((std::string("Failed to parse configuration : ") + jreader.getFormatedErrorMessages()).c_str());
				return false;
			}

			for (size_t i = 0; i < JRootValue.size(); i++)
			{
				RKString name_act = "act" + std::to_string(i);
				auto action_value = JRootValue[name_act.GetString()];
				auto list_action_each_act = ReadJsonAction(action_value);

				p_list_actions.insert(std::pair<RKString, ActionRepeat*>(name_act, list_action_each_act));
			}
			return true;
		}

		ActionCommand::ActionDetail* ActionCommand::ParseDetailJsonAction(const Json::Value value)
		{
			//PASSERT2(value.size() == 2, "the value must 2 vlue detail_action and time_action");

			ActionDetail* return_value = new ActionDetail();
			//get the first value
			for (auto it = value.begin(); it != value.end(); ++it)
			{
				RKString name_value = it.memberName();
				return_value->ParseActionDetail(value, name_value);			
			}

			return return_value;
		}

		ActionCommand::ActionRepeat * ActionCommand::ReadJsonAction(const Json::Value value)
		{
			ActionRepeat* list_action_each_act = new ActionRepeat();
			for (auto it = value.begin(); it != value.end(); ++it)
			{
				std::vector<ActionDetail*> list_detail_act;
				RKString name_value = it.memberName();
				bool dont_push = false;
				if (name_value.Contains("single"))
				{
					ActionDetail* act_detail = ParseDetailJsonAction(value[name_value.GetString()]);
					list_detail_act.push_back(act_detail);
				}
				else if (name_value.Contains("squence"))
				{
					auto value_squence = value[name_value.GetString()];
					for (auto it_squence = value_squence.begin(); it_squence != value_squence.end(); it_squence++)
					{
						int idx_value = it_squence.index();
						ActionDetail* act_detail = ParseDetailJsonAction(value_squence[idx_value]);
						list_detail_act.push_back(act_detail);
					}
				}
				else if (name_value == "repeat")
				{
					list_action_each_act->number_repeat = value[name_value.GetString()].asInt();
					dont_push = true;
				}

				if (!dont_push)
				{
					list_action_each_act->p_list_action_.push_back(list_detail_act);
				}
			}

			return list_action_each_act;
		}

		Vector<FiniteTimeAction*> ActionCommand::GetSquenceActionDetail(RKString name, int idx, int & action_tag, cocos2d::Node * it_res, cocos2d::Node * parent, bool IsLayer) const
		{
			auto list_action_detail = (p_list_actions.at(name)->p_list_action_).at(idx);
			Vector<FiniteTimeAction*> array_Action;
			for (size_t i = 0; i < list_action_detail.size(); i++)
			{
				auto action = list_action_detail.at(i)->GetAction(action_tag, it_res, parent, IsLayer);
				if (action)
				{
					array_Action.pushBack(action);
#if MANAGE_ACTION
					action->release();
#endif
				}
			}
			//array_Action.pushBack(nullptr);
			//auto squence = Sequence::create(array_Action);

			return array_Action;
		}

		Vector<FiniteTimeAction*> ActionCommand::GetSequenceActionData(int & action_tag, cocos2d::Node * it_res, cocos2d::Node * parent, int & flag, bool IsLayer) const
		{
			//
			//Sequence * action = NULL;
			Vector<FiniteTimeAction*> array_final_Action;
			for (auto it = p_list_actions.begin(); it != p_list_actions.end(); it++)
			{
				Vector<FiniteTimeAction*> array_Action;
				for (size_t i = 0; i < it->second->p_list_action_.size(); i++)
				{
					auto squence_action = GetSquenceActionDetail(it->first, i, action_tag, it_res, parent, IsLayer);
					Sequence * ac = Sequence::create(squence_action);
					array_Action.pushBack(ac);
					if (ac)
					{
						ac->setName("[SEQUENCE of DETAIL ACTION]");
#if MANAGE_ACTION
						PoolManager::getInstance()->getCurrentPool()->removeLastObject();
						ac->release();
#endif
					}
				}
				//array_Action.pushBack(nullptr);
				auto spawn_action = Spawn::create(array_Action);
				if (spawn_action)
				{
					spawn_action->setName("[SPAWN of SEQUENCE ACTION]");
#if MANAGE_ACTION
					PoolManager::getInstance()->getCurrentPool()->removeLastObject();
#endif
				}
				if (it->second->number_repeat > 0)
				{
					Vector<FiniteTimeAction*> array_Action_repeat;
					int number_repeat = it->second->number_repeat;
					do
					{
						array_Action_repeat.pushBack(spawn_action);
#if MANAGE_ACTION
						if (spawn_action)
						{
							spawn_action->release();
						}
#endif
						number_repeat--;
					} while (number_repeat > 0);

					flag += it->second->number_repeat;

					Sequence* action_continue = Sequence::create(array_Action_repeat);
					action_tag *= it->second->number_repeat;
					array_final_Action.pushBack(action_continue);
					if (action_continue)
					{
						spawn_action->setName("[REPEAT of MANY SPAWN ACTION]");
#if MANAGE_ACTION
						PoolManager::getInstance()->getCurrentPool()->removeLastObject();
						action_continue->release();
#endif
					}
				}
				else // repeate forever
				{
					RepeatForever * repeatAction = RepeatForever::create(spawn_action);
					flag = -1;
					array_final_Action.pushBack(repeatAction);
					if (repeatAction)
					{
#if MANAGE_ACTION
						spawn_action->setName("[REPET FOREVER of MANY SPAWN ACTION]");
						PoolManager::getInstance()->getCurrentPool()->removeLastObject();
						repeatAction->release();
#endif
					}
					//return repeatAction;
				}
			}
			//array_final_Action.pushBack(nullptr);

			//action = Sequence::create(array_final_Action);

			return array_final_Action;
		}

		ActionCommand * ActionCommand::Clone()
		{
			ActionCommand * p_clone = new ActionCommand();
			
			p_clone->JRootValue = JRootValue;

			for (auto it : p_list_actions)
			{
				p_clone->p_list_actions.insert({ it.first,it.second->clone() });
			}

			return p_clone;
					
		}

	}
}