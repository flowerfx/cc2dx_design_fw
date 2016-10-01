#include "MenuResource.h"
#include "../../Effect.h"
namespace Utility
{
	int tag_glow = 0xfff;
	namespace UI
	{
		MenuResource::MenuResource()
		{
			p_action_fade = 0;

			p_originPos = Vec2(0, 0);
			p_firstPos = p_originPos;

			p_originAngle = 0;
			p_firstAngle = p_originAngle;

			p_target_fade_alpha = 255;
			p_first_fade_alpha = p_first_fade_alpha;

			p_target_scale_to = 1.f;
			p_first_scale = p_target_scale_to;

			p_origin_tint = Color3B::WHITE;
			p_first_tint = p_origin_tint;

			p_IsVisible = true;
			p_Resource = NULL;
			p_textLabel = NULL;
			p_listView = NULL;


			p_currentStatusMenu = STATUS_MENU::M_IDLE;

			p_typeUI = UI_MENU_TYPE::MENU_UI_NONE;
		}
		MenuResource::~MenuResource()
		{
			CC_SAFE_RELEASE_NULL(p_Resource);
			CC_SAFE_RELEASE_NULL(p_textLabel);
		}

		bool MenuResource::VisibleResource()
		{
			return p_IsVisible;
		}

		void MenuResource::VisibleResource(bool b)
		{
			p_IsVisible = b;
		}

		unsigned int MenuResource::GetIndexAppear()
		{
			return p_index_appear;
		}

		void MenuResource::SetIndexAppear(unsigned int idx)
		{
			p_index_appear = idx;
		}

		void MenuResource::SetScaleResource(cocos2d::Vec2 s)
		{
			cocos2d::Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			p_scale.x = GetMulMul(s.x, mipmap);
			p_scale.y = GetMulMul(s.y, mipmap);

			GetResource()->setScaleX(p_scale.x);
			GetResource()->setScaleY(p_scale.y);
			
		}

		Vec2 MenuResource::GetScaleResource()
		{
			cocos2d::Vec2 p_scale(1, 1);
			int mipmap = TextureMgr->GetLevelMipMap();

			p_scale.x = GetMulDiv(GetResource()->getScaleX(), mipmap);
			p_scale.y = GetMulDiv(GetResource()->getScaleY(), mipmap);


			return p_scale;
		}

		Vec2 MenuResource::GetSizeResource()
		{
			cocos2d::Vec2 p_Size(1, 1);
			p_Size.x = GetResource()->getContentSize().width * GetResource()->getScaleX();
			p_Size.y = GetResource()->getContentSize().height * GetResource()->getScaleY();

			return p_Size;
		}

		void MenuResource::SetSizeResource(cocos2d::Vec2 s)
		{
			cocos2d::Vec2 p_Size(1, 1);

			p_Size.x = s.x / GetResource()->getContentSize().width;
			p_Size.y = s.y / GetResource()->getContentSize().height;
			GetResource()->setScaleX(p_Size.x);
			GetResource()->setScaleY(p_Size.y);


		}

		void MenuResource::SetRotateResource(float r)
		{
			GetResource()->setRotation(r);
		}

		float MenuResource::GetRotateResource()
		{
			return GetResource()->getRotation();
		}

		void MenuResource::SetPositionResource(Vec2 p)
		{
			GetResource()->setPosition(p);
		}

		Vec2 MenuResource::GetPositionResource()
		{
			return GetResource()->getPosition();		
		}

        void MenuResource::SetOpacityResource(RKUtils::BYTE p)
		{
			GetResource()->setOpacity(p);
		}

		RKUtils::BYTE MenuResource::GetOpacityResource()
		{
				return GetResource()->getOpacity();
		}

		void MenuResource::SetColorResource(Color3B p)
		{

			return GetResource()->setColor(p);			
		}

		Color3B MenuResource::GetColorResource()
		{
			return GetResource()->getColor();
		}

		void MenuResource::VisitResource(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (p_IsVisible)
			{
				GetResource()->visit(renderer, transform, flags);
			}
		}

		void MenuResource::DrawResource(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (p_IsVisible)
			{
				GetResource()->draw(renderer, transform, flags);
			}
		}

		void MenuResource::LoadResourceFromSingleImage(RKString name)
		{
			p_Resource = Sprite::create(name.GetString());
			p_Resource->retain();
		}

		void MenuResource::LoadResourceFromSingleTexture(Texture2D * tx)
		{
			p_Resource = Sprite::createWithTexture(tx);
			if (p_Resource)
			{
				p_Resource->retain();
			}
		}

		void MenuResource::LoadResourceFromData(RKString str)
		{
			auto tx = TextureMgr->GetSpriteByName(str);
			if (tx)
			{
				parseSprite(tx);
			}

		}

		void MenuResource::parseSprite(Sprite * tx)
		{
#ifdef USE_SPRITE_FRAME
			p_Resource = tx;
#else
			p_Resource = tx->createWithTexture(tx->getTexture(), tx->getTextureRect());
#endif
#if USE_ANTIALIAS
			p_Resource->getTexture()->generateMipmap();
			Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
			p_Resource->getTexture()->setTexParameters(&texParams);
#endif
			p_Resource->retain();
		}

		void MenuResource::SetActionToFade(unsigned int fd)
		{
			p_action_fade = fd;
			p_count_action_fade = p_action_fade;
		}

		void MenuResource::AddActionFade(UI_STATE_FADE_ACTION fd)
		{
			p_action_fade |= fd;
			p_count_action_fade = p_action_fade;
		}

		void MenuResource::RemoveActionFade(UI_STATE_FADE_ACTION fd)
		{
			p_action_fade ^= fd;
			p_count_action_fade = p_action_fade;
		}
		
		void MenuResource::ResetActionFade()
		{
			p_action_fade = 0;
			p_count_action_fade = 0;
		}

		bool MenuResource::ContainActionFade(UI_STATE_FADE_ACTION fd)
		{
			return ((p_action_fade & fd) == fd);
		}

		void MenuResource::InitSetFade(RKList<UI_STATE_FADE_ACTION> l_fd, float time)
		{
			p_timeAction = time;
			p_l_fd = l_fd;
		}

		void MenuResource::SetActionFade(STATUS_MENU stateFade, bool force_only)
		{
			//check
			if (force_only)
			{
				p_currentStatusMenu = stateFade;
				return;
			}
			//this time we support 4 action s
			if (p_l_fd.Size() >= UI_STATE_FADE_ACTION::FADE_COUNT)
			{
				CCASSERT(false, "check size again"); // check size again
				return;
			}
			p_currentStatusMenu = stateFade;
			Sequence * action = NULL;
			action = GetSequenceActionFromList(stateFade, p_l_fd, p_timeAction);
			if (!action)
			{
				p_currentStatusMenu = STATUS_MENU::M_IDLE;
				return;
			}

			GetResource()->runAction(action);
			
		}

		void MenuResource::FadeFinish(UI_STATE_FADE_ACTION current_fd)
		{
			p_count_action_fade ^= current_fd;
			if (p_count_action_fade <= 0)
			{
				//reset all action tag
				RemoveActionFadeByTag();
			}
		}

		void MenuResource::FadeFinish1(UI_STATE_FADE_ACTION current_fd)
		{
			p_count_action_fade ^= current_fd;
			if (p_count_action_fade <= 0)
			{
				//reset all action tag
				RemoveActionFadeByTag();
			}
		}

		void MenuResource::RemoveActionFadeByTag(bool IsAll)
		{
			if (GetResource())
			{
					if (IsAll)
					{
						GetResource()->getActionManager()->removeAllActionsFromTarget(GetResource());
					}
					else
					{
						GetResource()->getActionManager()->removeActionByTag(p_action_fade, GetResource());
					}
			}
			ResetActionFade();
			p_currentStatusMenu = STATUS_MENU::M_IDLE;
		}

		Vector<FiniteTimeAction*> MenuResource::CreateListActionFromSingleAction(UI_STATE_FADE_ACTION action_state, int loop, bool reserve, float time)
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
				ac1 = ScaleTo::create(time, p_first_scale);
				if (reserve)
				{
					ac_reserve = ScaleTo::create(time, p_target_scale_to);
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_TRANS)
			{
				ac1 = FadeTo::create(time, p_first_fade_alpha);
				if (reserve)
				{
					ac_reserve = FadeTo::create(time, p_target_fade_alpha);
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_ROTATE)
			{
				ac1 = RotateTo::create(time, abs(p_firstAngle - p_originAngle));
				if (reserve)
				{
					ac_reserve = RotateTo::create(time, abs(p_originAngle - p_firstAngle));
				}
			}
			else if (action_state == UI_STATE_FADE_ACTION::FADE_MOVE)
			{
				ac1 = MoveTo::create(time, p_firstPos);
				if (reserve)
				{
					ac_reserve = MoveTo::create(time, p_originPos);
				}
			}

			p_action_fade |= (action_state * loop);

			if (ac_reserve)
			{
				p_action_fade |= ((action_state << 1) * loop);
			}



			if (loop < 0)
			{
				list_action.insert(list_action.size() , RepeatForever::create(
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
				RemoveActionFadeByTag();
				///
			});

			list_action.insert(list_action.size(), action_complete);
			//list_action.insert(list_action.size(), nullptr);

			return list_action;
		}

		Sequence * MenuResource::CreateActionFromList(Vector<ActionInterval*> list_spawn_action)
		{
			Sequence * action = NULL;

			int size_list = list_spawn_action.size();

			if (size_list <= 0)
			{
				return NULL;
			}

			if (size_list == 1){
				action = Sequence::create(list_spawn_action.at(0), nullptr);
			}
			else if (size_list == 2){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), nullptr);
			}
			else if (size_list == 3){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), nullptr);
			}
			else if (size_list == 4){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), nullptr);
			}
			else if (size_list == 5){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), nullptr);
			}
			else if (size_list == 6){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), nullptr);
			}
			else if (size_list == 7){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), nullptr);
			}
			else if (size_list == 8){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), nullptr);
			}
			else if (size_list == 9){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), list_spawn_action.at(8), nullptr);
			}
			else if (size_list == 10){
				action = Sequence::create(list_spawn_action.at(0), list_spawn_action.at(1), list_spawn_action.at(2), list_spawn_action.at(3), list_spawn_action.at(4), list_spawn_action.at(5), list_spawn_action.at(6), list_spawn_action.at(7), list_spawn_action.at(8), list_spawn_action.at(9), nullptr);
			}

			return action;
		}

		void MenuResource::SetResourceRunManyAction(std::vector<Vector<FiniteTimeAction*>> list_action_)
		{
			Action * action = NULL;

			if (list_action_.size() <= 1)
			{
				CCASSERT(false, "use this function if the list have 2 or more action");
				p_action_fade = 0;
				p_count_action_fade = 0;
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
					p_action_fade = 0;
					p_count_action_fade = 0;
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
					p_action_fade = 0;
					p_count_action_fade = 0;
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
					p_action_fade = 0;
					p_count_action_fade = 0;
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
				return ;
			}

			p_count_action_fade = p_action_fade;
			action->setTag(p_action_fade);
			action->retain();

			GetResource()->runAction(action);
		}

		void MenuResource::SetResourceRunAction(UI_STATE_FADE_ACTION action_state, int loop, bool reserve, float time)
		{
			Action * action = NULL;

			Vector<FiniteTimeAction*> list_action = CreateListActionFromSingleAction(action_state, loop, reserve, time);

			action = Sequence::create(list_action);

			p_count_action_fade = p_action_fade;
			action->setTag(p_action_fade);
			action->retain();

			GetResource()->runAction(action);		

		}


		bool MenuResource::HaveActionNotFade()
		{
			return (p_currentStatusMenu == STATUS_MENU::M_IDLE && p_action_fade != 0);
		}

		Sequence * MenuResource::GetSequenceActionFromList(STATUS_MENU stateFade , RKList<UI_STATE_FADE_ACTION> l_fd, float time)
		{
			//
			RKList<FiniteTimeAction *> listActions;
			RKList<FiniteTimeAction *> listActionDones;
			int state_fd_insert[UI_STATE_FADE_ACTION::FADE_COUNT] = { 0 };
			//
			for (int i = 0; i < UI_STATE_FADE_ACTION::FADE_COUNT; i++)
			{
				if (i >= l_fd.Size())
				{
					break;
				}
				if (l_fd.GetAt(i) == UI_STATE_FADE_ACTION::FADE_MOVE)
				{
					if (state_fd_insert[0] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					//
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(MoveTo::create(p_timeAction, p_originPos));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(MoveTo::create(p_timeAction, p_firstPos));
					//
					state_fd_insert[0] = 1;
				}
				else if (l_fd.GetAt(i) == UI_STATE_FADE_ACTION::FADE_TRANS)
				{
					if (state_fd_insert[1] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(FadeTo::create(p_timeAction, p_target_fade_alpha));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(FadeTo::create(p_timeAction, p_first_fade_alpha));
					state_fd_insert[1] = 1;
				}
				else if (l_fd.GetAt(i) == UI_STATE_FADE_ACTION::FADE_ROTATE)
				{
					if (state_fd_insert[2] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(RotateTo::create(p_timeAction, abs(p_originAngle - p_firstAngle)));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(RotateTo::create(p_timeAction, abs(p_firstAngle - p_originAngle)));
					state_fd_insert[2] = 1;
				}
				else if (l_fd.GetAt(i) == UI_STATE_FADE_ACTION::FADE_ZOOM)
				{
					if (state_fd_insert[3] == 1)
					{
						//already insert
						CC_ASSERT(false);
						continue;
					}
					if (stateFade == STATUS_MENU::M_FADEIN)
						listActions.Append(ScaleTo::create(p_timeAction, p_target_scale_to));
					else if (stateFade == STATUS_MENU::M_FADEOUT)
						listActions.Append(ScaleTo::create(p_timeAction, p_first_scale));
					state_fd_insert[3] = 1;
				}
				AddActionFade(l_fd.GetAt(i));
				listActionDones.Append(CallFunc::create(CC_CALLBACK_0(MenuResource::FadeFinish, this, l_fd.GetAt(i))));
				
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
						listActions[0],		listActions[1],		nullptr),
						listActionDones[0], listActionDones[1],
					nullptr);
			}
			else if (listActions.Size() == 3)
			{
				action = Sequence::create(
					Spawn::create(
						listActions[0],		listActions[1],		listActions[2],		nullptr),
						listActionDones[0],	listActionDones[1], listActionDones[2],
					nullptr);
			}
			else if (listActions.Size() == 4)
			{
				action = Sequence::create(
					Spawn::create(
						listActions[0],		listActions[1],		listActions[2],		listActions[3],		nullptr),
						listActionDones[0], listActionDones[1], listActionDones[2], listActionDones[3],
					nullptr);
			}
			else
			{
				//CCASSERT(false,"no thing in list action");
				return 0;
			}
			action->setTag(p_action_fade);

			return action;
		}

		bool MenuResource::UpdateActionFade(float dt)
		{
			if (p_currentStatusMenu == STATUS_MENU::M_IDLE && (p_action_fade == 0))
			{
				return true;
			}
			if (p_action_fade == 0)
				return true;
			if (GetResource())
			{
				auto actionTag = GetResource()->getActionByTag(p_action_fade);
				if (actionTag)
					actionTag->step(dt);
			}
			
			return false;
		}

		void MenuResource::ResetSizeAndPosResource(int cur_width, int cur_height)
		{
			//set position

			Vec2 cur_pos = GetPositionResource();
			cur_pos.x = (cur_pos.x / cur_width) * GetGameSize().width;
			cur_pos.y = (cur_pos.y / cur_height) * GetGameSize().height;
			SetPositionResource(cur_pos);
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

		}
	
	}
}