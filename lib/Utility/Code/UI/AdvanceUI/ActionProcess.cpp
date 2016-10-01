#include "ActionProcess.h"

#include "LayerEntity.h"

#include "WidgetEntity.h"
namespace Utility
{


	namespace UI_Widget
	{
		ActionProcess::ActionProcess()
		{
			p_action_process = 0;

			//p_origin_value = new xml::BasicDec();
			//p_first_value = new xml::BasicDec();

			p_origin_value.InsertDataVector2("pos", Vec2(0, 0));
			p_first_value.InsertDataVector2("pos", Vec2(0, 0));

			p_origin_value.InsertDatafloat("rotate", 0);
			p_first_value.InsertDatafloat("rotate", 0);

			p_origin_value.InsertDataInt("alpha", 255);
			p_first_value.InsertDataInt("alpha", 255);

			p_origin_value.InsertDatafloat("scale", 1.f);
			p_first_value.InsertDatafloat("scale", 1.f);

			p_origin_value.InsertDataColor("tint", Color4B::WHITE);
			p_first_value.InsertDataColor("tint", Color4B::WHITE);


			p_Visible = true;
			menu_point_to = nullptr;
			p_parent_layer = nullptr;
			p_parent_widget = nullptr;

			p_currentStatusMenu = STATUS_MENU::M_IDLE;

			p_list_command.clear();

			p_IsInteractive = true;
			p_disable = false;
			p_IsStickScreen = false;
			p_z_order = 0;

			p_action = nullptr;
			p_state_scale = 0;
		}

		ActionProcess::~ActionProcess()
		{
			p_origin_value.clearAllData();
			p_first_value.clearAllData();

			for (auto it = p_list_command.begin(); it != p_list_command.end(); it++)
			{
				delete p_list_command.at(it->first);
			}

			p_list_command.clear();

			menu_point_to = nullptr;
			p_parent_layer = nullptr;
			p_parent_widget = nullptr;

			if (p_action && (p_action->getReferenceCount() > 0 && p_action->getReferenceCount() < 10))
			{
				if (p_action->getReferenceCount() != 1)
				{
					CCLOG("leak detected : .. fix..");
					while (p_action->getReferenceCount() > 1)
					{
						p_action->release();
					}
				}
				p_action->release();
			}

			p_action = nullptr;
		}

#pragma region normal_method

		void ActionProcess::CloneThis(ActionProcess * p_clone)
		{
			p_clone->p_Name = p_Name;
			p_clone->p_typeUI = p_typeUI;
			p_clone->p_action_process = p_action_process;
			p_clone->p_count_action_process = p_count_action_process;

			p_clone->p_timeAction = p_timeAction;
			p_clone->p_Visible = p_Visible;

			p_clone->p_origin_value = *(p_origin_value.Clone());
			p_clone->p_first_value = *(p_first_value.Clone());

			p_clone->p_currentStatusMenu = p_currentStatusMenu;
			p_clone->p_l_fd = p_l_fd;
			//
			p_clone->p_index_appear = p_index_appear;

			p_clone->menu_point_to = menu_point_to;
			p_clone->p_parent_layer = p_parent_layer;
			p_clone->p_parent_widget = p_parent_widget;

			p_clone->p_IsInteractive = p_IsInteractive;
			p_clone->p_IsStickScreen = p_IsStickScreen;
			p_clone->p_z_order = p_z_order;
			p_clone->p_disable = p_disable;

			for (auto it : p_list_command)
			{
				p_clone->p_list_command.insert({ it.first, it.second->Clone() });
			}
		}

		bool ActionProcess::Visible()
		{
			//return p_Visible;
			return GetResource()->isVisible();
		}

		void ActionProcess::Visible(bool b)
		{
			p_Visible = b;
			GetResource()->setVisible(b);
		}

		unsigned int ActionProcess::GetIndexAppear()
		{
			return p_index_appear;
		}

		void ActionProcess::SetIndexAppear(unsigned int idx)
		{
			p_index_appear = idx;
		}

		void ActionProcess::SetScale(cocos2d::Vec2 s)
		{
			cocos2d::Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			p_scale.x = GetMulMul(s.x, mipmap);
			p_scale.y = GetMulMul(s.y, mipmap);


			GetResource()->setScaleX(p_scale.x);
			GetResource()->setScaleY(p_scale.y);

		}

		Vec2 ActionProcess::GetScale()
		{
			cocos2d::Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			p_scale.x = GetMulDiv(GetResource()->getScaleX(), mipmap);
			p_scale.y = GetMulDiv(GetResource()->getScaleY(), mipmap);


			return p_scale;
		}

		Vec2 ActionProcess::GetSize()
		{
			cocos2d::Vec2 p_Size(1, 1);
			p_Size.x = GetResource()->getContentSize().width * GetResource()->getScaleX();
			p_Size.y = GetResource()->getContentSize().height * GetResource()->getScaleY();

			return p_Size;
		}

		void ActionProcess::SetSize(cocos2d::Vec2 s)
		{
			cocos2d::Vec2 p_Size(1, 1);

			p_Size.x = s.x / GetResource()->getContentSize().width;
			p_Size.y = s.y / GetResource()->getContentSize().height;
			GetResource()->setScaleX(p_Size.x);
			GetResource()->setScaleY(p_Size.y);


		}

		void ActionProcess::SetRotate(float r)
		{
			GetResource()->setRotation(r);
		}

		float ActionProcess::GetRotate()
		{
			return GetResource()->getRotation();
		}

		void ActionProcess::SetPosition(Vec2 p)
		{
			GetResource()->setPosition(p);
		}

		Vec2 ActionProcess::GetPosition()
		{
			return GetResource()->getPosition();
		}

		void ActionProcess::SetOpacity(RKUtils::BYTE p)
		{
			GetResource()->setOpacity(p);
		}

		RKUtils::BYTE ActionProcess::GetOpacity()
		{
			return GetResource()->getOpacity();
		}

		void ActionProcess::SetColor(Color3B p)
		{

			GetResource()->setColor(p);

		}

		Color3B ActionProcess::GetColor()
		{
			return GetResource()->getColor();
		}

		void ActionProcess::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			//if (p_Visible)
			//{
				//GetResource()->visit(renderer, transform, flags);
			//}
		}

		void ActionProcess::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			//if (p_Visible)
			//{
				//GetResource()->draw(renderer, transform, flags);
			//}
		}

#pragma endregion normal_method

#pragma region action_process_method
		bool ActionProcess::SetActionProcess(STATUS_MENU stateFade, bool force_only)
		{
			//check
			if (force_only)
			{
				p_currentStatusMenu = stateFade;
				return false;
			}

			if (p_list_command.size() > 0)
			{
				RKString name_cmd = "";
				if (stateFade == STATUS_MENU::M_FADEIN)
				{
					name_cmd = "FADE_IN";
				}
				else if (stateFade == STATUS_MENU::M_FADEOUT)
				{
					name_cmd = "FADE_OUT";
				}

				bool val = SetActionCommandWithName(name_cmd);

				if (val)
				{
					p_currentStatusMenu = stateFade;
					return true;
				}

			}

			//this time we support 4 action s
			if (p_l_fd.size() >= UI_STATE_FADE_ACTION::FADE_COUNT)
			{
				CCASSERT(false, "check size again"); // check size again
				return false;
			}
			p_currentStatusMenu = stateFade;
			Sequence * action = NULL;
			action = GetSequenceActionFromList(stateFade, p_l_fd, p_timeAction);
			if (!action)
			{
				p_currentStatusMenu = STATUS_MENU::M_IDLE;
				return false;
			}

			GetResource()->runAction(action);
			return true;

		}

		void ActionProcess::ForceFinishAction()
		{
			//
			FinishRunAction();

			//reset all action tag
			RemoveActionByTag(true);

			p_count_action_process = 0;
			p_action_process = 0;
		}


		void ActionProcess::ActionProcessFinish(UI_STATE_FADE_ACTION current_fd)
		{
			p_count_action_process ^= current_fd;
			if (p_count_action_process <= 0)
			{
				//
				FinishRunAction();

				//reset all action tag
				RemoveActionByTag();
			}
		}

		void ActionProcess::RemoveActionByTag(bool IsAll)
		{
			if (GetResource())
			{
				GetResource()->stopAllActions();
				if (IsAll)
				{
					GetResource()->getActionManager()->removeAllActionsFromTarget(GetResource());
				}
				else
				{
					GetResource()->getActionManager()->removeActionByTag(p_action_process, GetResource());
				}

				if (p_action && p_action->getReferenceCount() < 20)
				{
					if (p_action->getReferenceCount() != 1)
					{
						CCLOG("leak detected : .. fix..");
						while (p_action->getReferenceCount() > 1)
						{
							p_action->release();
						}
					}
				}
				else
				{
					p_action = nullptr;
				}
			}
			ResetActionFade();
			p_currentStatusMenu = STATUS_MENU::M_IDLE;
		}

		Vector<FiniteTimeAction*> ActionProcess::CreateListActionFromSingleAction(UI_STATE_FADE_ACTION action_state, int loop, bool reserve, float time)
		{
			Vector<FiniteTimeAction*> list_action;

			ActionInterval * ac1 = NULL;
			ActionInterval * ac_reserve = NULL;

			if (loop == 0)
				return list_action;

			if (time <= 0)
			{
				time = p_timeAction;
			}

			if (action_state == UI_STATE_FADE_ACTION::FADE_ZOOM)
			{
				ac1 = ScaleTo::create(time, GetFirstScale());
				if (reserve)
				{
					ac_reserve = ScaleTo::create(time, GetOriginScale());
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_TRANS)
			{
				ac1 = FadeTo::create(time, GetFirstAlpha());
				if (reserve)
				{
					ac_reserve = FadeTo::create(time, GetOriginAlpha());
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_ROTATE)
			{
				ac1 = RotateTo::create(time, abs(GetFirstAngle() - GetOriginAngle()));
				if (reserve)
				{
					ac_reserve = RotateTo::create(time, abs(GetOriginAngle() - GetFirstAngle()));
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_MOVE)
			{
				ac1 = MoveTo::create(time, GetFirstPos());
				if (reserve)
				{
					ac_reserve = MoveTo::create(time, GetOriginPos());
				}
			}

			p_action_process |= (action_state * loop);

			if (ac_reserve)
			{
				p_action_process |= ((action_state << 1) * loop);
			}



			if (loop < 0)
			{
				list_action.insert(list_action.size(), RepeatForever::create(
					Sequence::create(ac1, ac_reserve, nullptr)
				));

			}
			else if (loop > 0)
			{

				do
				{

					list_action.insert(list_action.size(), ac1);
					if (reserve)
					{
						list_action.insert(list_action.size(), ac_reserve);
					}
					loop--;
				} while (loop > 0);

			}

			auto action_complete = CallFunc::create(
				[=]()
			{
				RemoveActionByTag();
				///
			});

			list_action.insert(list_action.size(), action_complete);
			//list_action.insert(list_action.size(), nullptr);

			return list_action;
		}

		Sequence * ActionProcess::CreateActionFromList(Vector<ActionInterval*> list_spawn_action)
		{
			Sequence * action = NULL;

			int size_list = (int)list_spawn_action.size();

			if (size_list <= 0)
			{
				return NULL;
			}

			if (size_list == 1) {
				action = Sequence::create(list_spawn_action.at(0), nullptr);
			}
			else if (size_list == 2) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), nullptr);
			}
			else if (size_list == 3) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), nullptr);
			}
			else if (size_list == 4) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), nullptr);
			}
			else if (size_list == 5) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), nullptr);
			}
			else if (size_list == 6) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), nullptr);
			}
			else if (size_list == 7) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), nullptr);
			}
			else if (size_list == 8) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), nullptr);
			}
			else if (size_list == 9) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), list_spawn_action.at(8), nullptr);
			}
			else if (size_list == 10) {
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), list_spawn_action.at(8), list_spawn_action.at(9), nullptr);
			}

			return action;
		}

		void ActionProcess::SetResourceRunManyAction(std::vector<Vector<FiniteTimeAction*>> list_action_)
		{
			Action * action = NULL;

			if (list_action_.size() <= 1)
			{
				CCASSERT(false, "use this function if the list have 2 or more action");
				p_action_process = 0;
				p_count_action_process = 0;
				return;
			}

			//Sequence * action = NULL;
			if (list_action_.size() == 1)
			{
				action = Sequence::create(
					list_action_.at(0));
			}
			else if (list_action_.size() == 2)
			{
				auto list_action_1 = list_action_.at(0);
				auto list_action_2 = list_action_.at(1);

				if (list_action_1.size() != list_action_2.size())
				{
					CCASSERT(false, "size of all list must be equal");
					p_action_process = 0;
					p_count_action_process = 0;
					return;
				}

				Vector<ActionInterval*> list_spawn_action;

				for (int i = 0; i < list_action_1.size() - 1; i++)
				{
					list_spawn_action.pushBack(Spawn::create(
						list_action_1.at(i), list_action_2.at(i), nullptr));
				}

				action = Sequence::create(
					CreateActionFromList(list_spawn_action),
					list_action_1.at(list_action_1.size() - 1), list_action_2.at(list_action_2.size() - 1),
					nullptr);
			}
			else if (list_action_.size() == 3)
			{
				auto list_action_1 = list_action_.at(0);
				auto list_action_2 = list_action_.at(1);
				auto list_action_3 = list_action_.at(2);

				if (list_action_1.size() != list_action_2.size() || list_action_1.size() != list_action_3.size() || list_action_2.size() != list_action_3.size())
				{
					CCASSERT(false, "size of all list must be equal");
					p_action_process = 0;
					p_count_action_process = 0;
					return;
				}

				Vector<ActionInterval*> list_spawn_action;

				for (int i = 0; i < list_action_1.size() - 1; i++)
				{
					list_spawn_action.pushBack(Spawn::create(
						list_action_1.at(i), list_action_2.at(i), list_action_3.at(i), nullptr));
				}

				action = Sequence::create(
					CreateActionFromList(list_spawn_action),
					list_action_1.at(list_action_1.size() - 1), list_action_2.at(list_action_2.size() - 1), list_action_3.at(list_action_3.size() - 1),
					nullptr);
			}
			else if (list_action_.size() == 4)
			{
				auto list_action_1 = list_action_.at(0);
				auto list_action_2 = list_action_.at(1);
				auto list_action_3 = list_action_.at(2);
				auto list_action_4 = list_action_.at(2);

				if (list_action_1.size() != list_action_2.size() || list_action_1.size() != list_action_3.size() || list_action_2.size() != list_action_3.size() ||
					list_action_1.size() != list_action_4.size() || list_action_2.size() != list_action_4.size() || list_action_3.size() != list_action_4.size()
					)
				{
					CCASSERT(false, "size of all list must be equal");
					p_action_process = 0;
					p_count_action_process = 0;
					return;
				}

				Vector<ActionInterval*> list_spawn_action;

				for (int i = 0; i < list_action_1.size() - 1; i++)
				{
					list_spawn_action.pushBack(Spawn::create(
						list_action_1.at(i), list_action_2.at(i), list_action_3.at(i), list_action_4.at(i), nullptr));
				}

				action = Sequence::create(
					CreateActionFromList(list_spawn_action),
					list_action_1.at(list_action_1.size() - 1), list_action_2.at(list_action_2.size() - 1), list_action_3.at(list_action_3.size() - 1), list_action_4.at(list_action_4.size() - 1),
					nullptr);
			}
			else
			{
				//CCASSERT(false,"no thing in list action");
				return;
			}

			p_count_action_process = p_action_process;
			action->setTag(p_action_process);
			action->retain();

			GetResource()->runAction(action);
		}

		void ActionProcess::SetResourceRunAction(UI_STATE_FADE_ACTION action_state, int loop, bool reserve, float time)
		{
			Action * action = NULL;

			Vector<FiniteTimeAction*> list_action = CreateListActionFromSingleAction(action_state, loop, reserve, time);

			action = Sequence::create(list_action);

			p_count_action_process = p_action_process;
			action->setTag(p_action_process);
			action->retain();

			GetResource()->runAction(action);

		}

		bool ActionProcess::HaveActionNotFade()
		{
			return (p_currentStatusMenu == STATUS_MENU::M_IDLE && p_action_process != 0);
		}

        Sequence * ActionProcess::GetSequenceActionFromList(STATUS_MENU stateFade, std::vector<UI_STATE_FADE_ACTION> l_fd, float time)
		{
			//
			RKList<FiniteTimeAction *> listActions;
			RKList<FiniteTimeAction *> listActionDones;
			int state_fd_insert[UI_STATE_FADE_ACTION::FADE_COUNT] = { 0 };
			//
			for (int i = 0; i < UI_STATE_FADE_ACTION::FADE_COUNT; i++)
			{
				if (i >= (int)l_fd.size())
				{
					break;
				}
				if (l_fd.at(i) == UI_STATE_FADE_ACTION::FADE_MOVE)
				{
					if (state_fd_insert[0] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					//
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(MoveTo::create(p_timeAction, GetOriginPos()));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(MoveTo::create(p_timeAction, GetFirstPos()));
					//
					state_fd_insert[0] = 1;
				}
				else if (l_fd.at(i) == UI_STATE_FADE_ACTION::FADE_TRANS)
				{
					if (state_fd_insert[1] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(FadeTo::create(p_timeAction, GetOriginAlpha()));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(FadeTo::create(p_timeAction, GetFirstAlpha()));
					state_fd_insert[1] = 1;
				}
				else if (l_fd.at(i) == UI_STATE_FADE_ACTION::FADE_ROTATE)
				{
					if (state_fd_insert[2] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(RotateTo::create(p_timeAction, abs(GetOriginAngle() - GetFirstAngle())));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(RotateTo::create(p_timeAction, abs(GetFirstAngle() - GetOriginAngle())));
					state_fd_insert[2] = 1;
				}
				else if (l_fd.at(i) == UI_STATE_FADE_ACTION::FADE_ZOOM)
				{
					if (state_fd_insert[3] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(ScaleTo::create(p_timeAction, GetOriginScale()));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(ScaleTo::create(p_timeAction, GetFirstScale()));
					state_fd_insert[3] = 1;
				}
				AddActionFade(l_fd.at(i));
				listActionDones.Append(CallFunc::create(CC_CALLBACK_0(ActionProcess::ActionProcessFinish, this, l_fd.at(i))));

			}
			Sequence * action = NULL;
			if (listActions.Size() == 1)
			{
				action = Sequence::create(
					listActions[0],
					listActionDones[0],
					nullptr);
			}
			else if (listActions.Size() == 2)
			{
				action = Sequence::create(
					Spawn::create(
						listActions[0], listActions[1], nullptr),
					listActionDones[0], listActionDones[1],
					nullptr);
			}
			else if (listActions.Size() == 3)
			{
				action = Sequence::create(
					Spawn::create(
						listActions[0], listActions[1], listActions[2], nullptr),
					listActionDones[0], listActionDones[1], listActionDones[2],
					nullptr);
			}
			else if (listActions.Size() == 4)
			{
				action = Sequence::create(
					Spawn::create(
						listActions[0], listActions[1], listActions[2], listActions[3], nullptr),
					listActionDones[0], listActionDones[1], listActionDones[2], listActionDones[3],
					nullptr);
			}
			else
			{
				//CCASSERT(false,"no thing in list action");
				return 0;
			}
			action->setTag(p_action_process);

			return action;
		}

#pragma endregion action_process_method

		bool ActionProcess::UpdateActionProcess(float dt)
		{
			if (!IsAllChildFinishAction() && p_action_process == 0)
				return false;

			if (p_currentStatusMenu == STATUS_MENU::M_IDLE && p_action_process == 0)
			{
				return true;
			}

			if (p_action_process == 0)
				return true;

			//auto actionTag = GetResource()->getActionByTag(p_action_process);
			//if (actionTag)
			//	actionTag->step(dt);

			return false;
		}

		void ActionProcess::ResetSizeAndPos(int cur_width, int cur_height)
		{
			/*
			//set position

			Vec2 cur_pos = GetPosition();
			cur_pos.x = (cur_pos.x / cur_width) * GetGameSize().width;
			cur_pos.y = (cur_pos.y / cur_height) * GetGameSize().height;
			SetPosition(cur_pos);
			//set origin pos
			p_originPos.x = (p_originPos.x / cur_width) * GetGameSize().width;
			p_originPos.y = (p_originPos.y / cur_height) * GetGameSize().height;

			//set first pos
			p_firstPos.x = (p_firstPos.x / cur_width) * GetGameSize().width;
			p_firstPos.y = (p_firstPos.y / cur_height) * GetGameSize().height;

			//set size
			if (p_typeUI == UI_MENU_TYPE::MENU_TEXT)
			{
				Vec2 cur_scale = GetScaleResource();
				cur_scale.x = (cur_scale.x / cur_width) * GetGameSize().width;
				cur_scale.y = (cur_scale.y / cur_height) * GetGameSize().height;
				SetScaleResource(cur_scale);
			}
			else
			{
				Vec2 cur_size = GetSizeResource();
				cur_size.x = (cur_size.x / cur_width) * GetGameSize().width;
				cur_size.y = (cur_size.y / cur_height) * GetGameSize().height;
				SetSizeResource(cur_size);
			}

			//set first scale
			p_first_scale = (p_first_scale / cur_width) * GetGameSize().width;

			//set origin scale
			p_target_scale_to = (p_target_scale_to / cur_width) * GetGameSize().width;
			*/
		}

#pragma region xml_load
		void ActionProcess::SetValueFromXML(xml::UILayerWidgetDec * xml_value)
		{
			bool IsStickScreen = (xml_value->GeneralValue->GetDataInt("stick_screen") == 1);
			p_IsStickScreen = IsStickScreen;

			//set size value
			SetSizeValueXML(xml_value);

			//set pos value
			SetPosValueXML(xml_value);

			if (xml_value->FirstValue->HaveDataInt("opacity"))
			{
				SetFirstAlpha((RKUtils::BYTE)xml_value->FirstValue->GetDataInt("opacity"), true);
			}

			if (xml_value->FirstValue->HaveDatafloat("rotate"))
			{
				SetFirstAngle(xml_value->FirstValue->GetDatafloat("rotate"), true);
			}

			//
			Color4B color = xml_value->GeneralValue->GetDataColor("tint");
			SetOriginColor(Color3B(color.r, color.g, color.b), true);
			//
			InitSetFade(xml_value->actionFade, xml_value->TimeProcessFade);
			Visible(xml_value->visible == 1 ? true : false);
			SetIndexAppear(xml_value->index_appear);

			//
			SetCommandValueXML(xml_value);
		}

		void ActionProcess::SetSizeValueXML(xml::UILayerWidgetDec * xml_value)
		{

			if (!xml_value->FirstValue->HaveDataVector2("scale_ratio"))
			{
				return;
			}

			Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");
			Vec2 first_ScaleRatio = xml_value->FirstValue->GetDataVector2("scale_ratio");

			auto size_origin = Vec2(origin_ScaleRatio.x * GetGameSize().width, origin_ScaleRatio.y * GetGameSize().height);
			auto size_first = Vec2(first_ScaleRatio.x * GetGameSize().width, first_ScaleRatio.y * GetGameSize().height);

			bool IsStickScreen = (xml_value->GeneralValue->GetDataInt("stick_screen") == 1);

			p_IsStickScreen = IsStickScreen;

			if (IsStickScreen)
			{
				size_origin = Vec2(origin_ScaleRatio.x * GetGameSize().width, origin_ScaleRatio.y * GetGameSize().height);
				size_first = Vec2(first_ScaleRatio.x * GetGameSize().width, first_ScaleRatio.y * GetGameSize().height);
			}
			else
			{
				p_state_scale = (xml_value->GeneralValue->GetDataInt("state_scale"));

				float ratio = GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
				if (p_state_scale == 1)
				{
					ratio = GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
				}
				else if (p_state_scale == 2)
				{
					ratio = 1.f;
				}

				size_origin = Vec2(origin_ScaleRatio.x * ratio,
					origin_ScaleRatio.y * ratio);
				size_first = Vec2(first_ScaleRatio.x   * ratio,
					first_ScaleRatio.y   * ratio);

				if (size_first.x <= 2) { size_first.x = 2; }
				if (size_first.y <= 2) { size_first.y = 2; }
			}

			if (GetResource()->getContentSize().width == 0 || GetResource()->getContentSize().height == 0)
			{
				GetResource()->setContentSize(cocos2d::Size(size_origin.x, size_origin.y));			
			}

			SetFirstSize(size_first, xml_value->GeneralValue->GetDataInt("not_fade_size") == 0);
		}

		void ActionProcess::SetPosValueXML(xml::UILayerWidgetDec * xml_value)
		{
			if (!xml_value->FirstValue->HaveDataVector2("pos_ratio"))
			{
				return;
			}

			auto firstPosRatio = ConvertPos(xml_value->FirstValue->GetDataVector2("pos_ratio"));

			Vec2 posFirst = Vec2(0, 0);
			//if (p_parent_widget)
			//{
			//	//posFirst = 
			//}
			//else
			{
				posFirst = Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y);
			}

			SetFirstPos(posFirst, xml_value->GeneralValue->GetDataInt("not_fade_pos") == 0);
		}

		void ActionProcess::SetCommandValueXML(xml::UILayerWidgetDec * xml_value)
		{
			if (xml_value->GetCommandSize() <= 0)
				return;

			for (size_t i = 0; i < xml_value->GetCommandSize(); i++)
			{
				ActionCommand * p_command = new ActionCommand();

				auto cmd_pair = xml_value->GetIterCommandAtIdx(i);

				if (cmd_pair.first != "" && p_command->ReadJsonString(cmd_pair.second.c_str()))
				{
					p_list_command.insert(std::pair<RKString, ActionCommand*>(cmd_pair.first, p_command));
				}
			}
		}

		bool ActionProcess::SetActionCommandWithName(RKString name, CallFunc * func_callback)
		{
			if ((int)p_list_command.size() < 0 || p_list_command.find(name) == p_list_command.end())
				return false;

			//get the action command with name 

			ActionCommand* action_command = p_list_command.at(name);

			int flag = 0;
			auto action_squence = action_command->GetSequenceActionData(p_action_process, this->GetResource(),
				this->getUIWidgetType() == UI_TYPE::UI_LAYER ? this->GetParentLayer() : nullptr
				, flag, this->getUIWidgetType() == UI_TYPE::UI_LAYER);

			p_count_action_process = p_action_process;
			if (flag >= 0)
			{
				action_squence.pushBack(func_callback != nullptr ? func_callback : CallFunc::create(CC_CALLBACK_0(ActionProcess::ForceFinishAction, this)));
				//action_squence.pushBack(nullptr);
				auto action = Sequence::create(action_squence);
#if MANAGE_ACTION
				if (action)
				{
					PoolManager::getInstance()->getCurrentPool()->removeLastObject();
				}
				//if (func_callback)
				//{
				//	func_callback->retain();
				//}
				action->retain();
				p_action = action;
#endif
				action->setTag(p_action_process);
				GetResource()->runAction(action);
			}
			else
			{
				GetResource()->runAction(static_cast<RepeatForever*>(action_squence.at(0)));
			}


			return true;
		}

		bool ActionProcess::SetHookCommand(RKString name_command, RKString idx_act, int idx_squence, int idx_detail, xml::BasicDec  value, RKString name_value)
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

		bool ActionProcess::ClearHookCommand(RKString name_command, RKString idx_act, int idx_squence, int idx_detail)
		{
			if (p_list_command.size() < 0)
			{
				return false;
			}

			if (p_list_command.find(name_command) == p_list_command.end())
			{
				return false;
			}
			ActionCommand* action_command = p_list_command.at(name_command);

			if (idx_act == "")
			{
				auto list_act = action_command->GetNumberActInCmd();
				for (auto it : list_act)
				{
					ClearHookCommand(name_command, it, idx_squence, idx_detail);
				}
				return true;
			}

			if (idx_detail >= 0)
			{
				auto action_detail = action_command->GetActionDetail(idx_act, idx_squence, idx_detail);
				if (action_detail)
				{
					action_detail->ClearHookCommand();
					return true;
				}
			}
			else
			{
				if (idx_squence >= 0)
				{
					int number_idx_detail = action_command->GetNumberDetailActInSquence(idx_act, idx_squence);
					for (int i = 0; i < number_idx_detail; i++)
					{
						ClearHookCommand(name_command, idx_act, idx_squence, i);
					}
					return true;
				}
				else
				{
					int number_squence = action_command->GetNumberSquenceInAct(idx_act);
					for (int i = 0; i < number_squence; i++)
					{
						int number_idx_detail = action_command->GetNumberDetailActInSquence(idx_act, i);
						for (int j = 0; j < number_idx_detail; j++)
						{
							ClearHookCommand(name_command, idx_act, i, j);
						}
					}
					return true;
				}

			}

			return false;
		}

		void ActionProcess::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			p_Name = name;

			if (xml_value->GeneralValue->HaveDataVector2("anchor_point"))
			{
				Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
				GetResource()->setAnchorPoint(anchor_point);
			}
			else
			{
				GetResource()->setAnchorPoint(Vec2(0.5, 0.5));
			}

			SetInitSize(xml_value);

			SetInitPos(xml_value);

			SetInitRotate(xml_value);

			SetOriginAlpha((RKUtils::BYTE)xml_value->OriginValue->GetDataInt("opacity"), true);

			if (xml_value->GeneralValue->HaveDataInt("casca_color"))
			{
				GetResource()->setCascadeColorEnabled(xml_value->GeneralValue->GetDataInt("casca_color") == 1);
			}
			else
			{
				GetResource()->setCascadeColorEnabled(true);
			}

			if (xml_value->GeneralValue->HaveDataInt("casca_opacity"))
			{
				GetResource()->setCascadeOpacityEnabled(xml_value->GeneralValue->GetDataInt("casca_opacity") == 1);
			}
			else
			{
				GetResource()->setCascadeOpacityEnabled(true);
			}

			if (xml_value->GeneralValue->HaveDataInt("zorder"))
			{
				p_z_order = xml_value->GeneralValue->GetDataInt("zorder");
			}


		}

		void ActionProcess::SetInitPos(xml::UILayerWidgetDec * xml_value)
		{
			auto pos_ratio = xml_value->OriginValue->GetDataVector2("pos_ratio");
			auto pos = Vec2(GetGameSize().width * pos_ratio.x, GetGameSize().height * pos_ratio.y);

			SetPosition(pos);
			p_origin_value.ReplaceDataVector2("pos", pos);
		}

		void ActionProcess::SetInitSize(xml::UILayerWidgetDec * xml_value)
		{
			auto scale_ratio = xml_value->OriginValue->GetDataVector2("scale_ratio");
			auto size = Vec2(GetGameSize().width * scale_ratio.x, GetGameSize().height * scale_ratio.y);
            cocos2d::Size content_size = GetResource()->getContentSize();
			bool IsStickScreen = (xml_value->GeneralValue->GetDataInt("stick_screen") == 1);

			if (!IsStickScreen)
			{
				p_state_scale = xml_value->GeneralValue->GetDataInt("state_scale");

				float ratio = GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
				if (p_state_scale == 1)
				{
					ratio = GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
				}
				else if (p_state_scale == 2)
				{
					ratio = 1;
				}

				size = Vec2(scale_ratio.x * ratio,
					scale_ratio.y * ratio);
			}

			if (size.x <= 2) { size.x = 2; }
			if (size.y <= 2) { size.y = 2; }

			if (content_size.width == 0 || content_size.height == 0)
			{
				GetResource()->setContentSize(cocos2d::Size(size.x, size.y));
			}

			SetSize(size);
			p_origin_value.ReplaceDatafloat("scale", GetResource()->getScaleX());
		}

		void ActionProcess::SetInitRotate(xml::UILayerWidgetDec * xml_value)
		{
			auto rotate = xml_value->OriginValue->GetDatafloat("rotate");
			SetRotate(rotate);
			p_origin_value.ReplaceDatafloat("rotate", rotate);
		}

		void ActionProcess::FinishRunAction()
		{
			if (p_parent_layer)
				p_parent_layer->HaveChildFinishAction();
		}

		void ActionProcess::ReloadText() {}
#pragma endregion xml_load

#pragma region normal_function

		Vec2 ActionProcess::PositionWithVectorUnit(Vec2 vec_unit)
		{
			Vec2 current_position = GetPosition();
			Vec2 anchor_point = GetResource()->getAnchorPoint();
			Vec2 current_size = GetSize();
			//convert to bottom left
			current_position.x -= anchor_point.x * current_size.x;
			current_position.y -= anchor_point.y * current_size.y;
			//
			return Vec2(current_position.x + current_size.x * vec_unit.x, 
						current_position.y + current_size.y * vec_unit.y);
		}

		Vec2 ActionProcess::GetPosition_TopLeft()
		{
			return PositionWithVectorUnit(Vec2(0, 1));
		}
		
		Vec2 ActionProcess::GetPosition_TopRight()
		{
			return PositionWithVectorUnit(Vec2(1, 1));
		}
		
		Vec2 ActionProcess::GetPosition_BottomLeft()
		{
			return PositionWithVectorUnit(Vec2(0, 0));
		}
		
		Vec2 ActionProcess::GetPosition_BottomRight()
		{
			return PositionWithVectorUnit(Vec2(1, 0));
		}
		
		Vec2 ActionProcess::GetPosition_Middle()
		{
			return PositionWithVectorUnit(Vec2(0.5, 0.5));
		}

		void ActionProcess::AddActionFade(UI_STATE_FADE_ACTION fd)
		{
			p_action_process |= fd;
			p_count_action_process = p_action_process;
		}

		void ActionProcess::RemoveActionFade(UI_STATE_FADE_ACTION fd)
		{
			p_action_process ^= fd;
			p_count_action_process = p_action_process;
		}

		void ActionProcess::ResetActionFade()
		{
			p_action_process = 0;
			p_count_action_process = 0;
		}

		bool ActionProcess::ContainActionFade(UI_STATE_FADE_ACTION fd)
		{
			return ((p_action_process & fd) == fd);
		}

		void ActionProcess::SetActionFade(unsigned int fd)
		{
			p_action_process = fd;
			p_count_action_process = p_action_process;
		}

        void ActionProcess::InitSetFade(std::vector<UI_STATE_FADE_ACTION> l_fd, float time)
		{
			p_timeAction = time;
			p_l_fd = l_fd;
		}

		RKString ActionProcess::GetName()
		{
			return p_Name;
		}

		void ActionProcess::SetName(RKString name)
		{
			p_Name = name;
			if (GetResource())
			{
				GetResource()->setName(name.GetString());
			}
		}

		Vec2 ActionProcess::GetOriginPos() { return p_origin_value.GetDataVector2("pos"); }
		Vec2 ActionProcess::GetFirstPos() { return p_first_value.GetDataVector2("pos"); }
		void ActionProcess::SetFirstPos(Vec2 p, bool force) {
			p_first_value.ReplaceDataVector2("pos", p);
			if (force)
			{
				SetPosition(p);
			}
		}
		//
		float ActionProcess::GetOriginAngle() { return p_origin_value.GetDatafloat("rotate"); }
		float ActionProcess::GetFirstAngle() { return p_first_value.GetDatafloat("rotate"); }
		void  ActionProcess::SetFirstAngle(float p, bool force) {
			p_first_value.ReplaceDatafloat("rotate", p);
			if (force)
			{
				SetRotate(p);
			}
		}
		//
		RKUtils::BYTE ActionProcess::GetOriginAlpha() { return (RKUtils::BYTE)p_origin_value.GetDataInt("alpha"); }
		void ActionProcess::SetOriginAlpha(RKUtils::BYTE p, bool force) {
			p_origin_value.ReplaceDataInt("alpha", (int)p);
			if (force)
			{
				SetOpacity(p);
			}
		}
		RKUtils::BYTE ActionProcess::GetFirstAlpha() { return (RKUtils::BYTE)p_first_value.GetDataInt("alpha"); }
		void ActionProcess::SetFirstAlpha(RKUtils::BYTE p, bool force) {
			p_first_value.ReplaceDataInt("alpha", (int)p);
			if (force)
			{
				SetOpacity(p);
			}
		}
		//
		Color3B ActionProcess::GetOriginColor() {
			auto t = p_origin_value.GetDataColor("tint");
			return Color3B(t.r, t.g, t.b);
		}
		void ActionProcess::SetOriginColor(Color3B p, bool force) {
			p_origin_value.ReplaceDataColor("tint", Color4B(p.r, p.g, p.b, 255));
			if (force)
			{
				SetColor(p);
			}
		}
		Color3B ActionProcess::GetFirstColor() {
			auto t = p_first_value.GetDataColor("tint");
			return Color3B(t.r, t.g, t.b);
		}
		void ActionProcess::SetFirstColor(Color3B p, bool force ) {
			p_first_value.ReplaceDataColor("tint", Color4B(p.r, p.g, p.b, 255));
			if (force)
			{
				SetColor(p);
			}
		}
		//
		float ActionProcess::GetOriginScale() { return p_origin_value.GetDatafloat("scale"); }
		float ActionProcess::GetFirstScale() { return p_first_value.GetDatafloat("scale"); }
		void  ActionProcess::SetFirstSize(Vec2 p, bool force) {
			if (force)
			{
				SetSize(p);
			}
			p_first_value.ReplaceDatafloat("scale", GetResource()->getScaleX());
		}
		void ActionProcess::SetFirstScale(float p, bool force) {
			p_origin_value.ReplaceDatafloat("scale", p);
			if (force)
			{
				SetScale(Vec2(p, p));
			}
		}
		//
		LayerEntity *	ActionProcess::GetParentLayer() { return p_parent_layer; }
		WidgetEntity *	ActionProcess::GetParentWidget() { return p_parent_widget; }
		Screen *		ActionProcess::GetMenuParent() { return menu_point_to; }

		UI_TYPE ActionProcess::getUIWidgetType() { return p_typeUI; }

		void			ActionProcess::SetParentLayer(LayerEntity * parent){ p_parent_layer = parent; }
		void			ActionProcess::SetMenuParent(Screen * menu) { menu_point_to = menu; }
		void			ActionProcess::SetParentWidget(WidgetEntity * widget) { p_parent_widget = widget; }

		bool ActionProcess::IsAllChildFinishAction() { return true; }
		void ActionProcess::SetInteractive(bool value) 
		{ 
			if (p_disable)
				return;
			p_IsInteractive = value; 
		}
		bool ActionProcess::IsInteractive() { return p_IsInteractive; }

		void ActionProcess::SetDisable(bool value)
		{ 
			if (value == true)
			{
				p_disable = false;
				SetInteractive(false);

				p_disable = true;
			}
			else
			{
				p_disable = value;
				SetInteractive(true);
			}
		}

		bool ActionProcess::IsDisable()
		{
			return p_disable;
		}

		cocos2d::Rect ActionProcess::GetBound()
		{

			Vec2 size = GetSize();
			Vec2 pos_bot_left = GetPosition_BottomLeft();

			cocos2d::Rect rec(
				pos_bot_left.x,
				pos_bot_left.y,
				size.x,
				size.y
			);
			return rec;
		}
#pragma endregion normal_function

	}
}