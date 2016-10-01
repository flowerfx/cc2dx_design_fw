#include "MenuEntity.h"
#include "../../InputManager.h"
#include "../../SoundManager.h"

namespace Utility
{
	namespace UI
	{

		MenuEntity::MenuEntity()
		{
			p_IsCanPressButton = false;

			p_DeltaMoveDir = cocos2d::Point(0, 0);
			p_DeltaMoveTime = 0;
			p_DistanceDelta = 0;

			p_DeltaPosWithBGMain = cocos2d::Point(0, 0);

			p_IsPlaySoundPress = false;

			p_IdxMenuData = -1;
			p_IdxUITagAppear = 0;

			p_time_process = 0;
			p_max_idx_process_first = 0;

			name_ui = "";
		}


		MenuEntity::~MenuEntity()
		{
			p_ListStatus.clear();

			p_ListMemberInGame.Clear();
		}

		void MenuEntity::onDrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				p_ListMemberInGame.GetAt(i)->Draw(renderer, transform, flags);
			}

		}
		void MenuEntity::onVisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				p_ListMemberInGame.GetAt(i)->Visit(renderer, transform, flags);
			}

		}
		void MenuEntity::onUpdateMenu(float dt)
		{
			onCheckPress();
			//if (GetCurrentStatus(0) == STATUS_MENU::M_IDLE)
			{
				for (int i = 0; i < p_ListMemberInGame.Size(); i++)
				{
					if (p_ListMemberInGame.GetAt(i)->GetResource()->HaveActionNotFade())
					{
						p_ListMemberInGame.GetAt(i)->GetResource()->UpdateActionFade(dt);

					}


					if (p_ListMemberInGame.GetAt(i)->GetType() != UI_MENU_TYPE::MENU_BUTTON)
						continue;
					auto btn = (ButtonEntity*)(p_ListMemberInGame.GetAt(i)->GetResource());
					btn->UpdatePress(dt);

				}
			}

			if ((p_IdxUITagAppear >= 0 && GetCurrentStatus(p_IdxUITagAppear) == STATUS_MENU::M_FADEIN) ||
				(p_IdxUITagAppear == 0 && GetCurrentStatus(p_IdxUITagAppear) == STATUS_MENU::M_FADEOUT))
			{
				bool res = OnUpdateFadeProcessChildByTagIdx(p_IdxUITagAppear,
					XMLMgr->GetUIByName(name_ui)->GetTimeProcess(p_IdxUITagAppear).time_process,
					dt,
					GetCurrentStatus(p_IdxUITagAppear));
			}
			else if (p_IdxUITagAppear > 0)
			{
				if (GetCurrentStatus(0) == STATUS_MENU::M_FADEOUT)
				{
					for (int i = 1; i < p_ListStatus.size(); i++)
					{
						if (p_ListStatus.at(i)->p_IsActive)
						{
							bool res = OnUpdateFadeProcessChildByTagIdx(i,
								XMLMgr->GetUIByName(name_ui)->GetTimeProcess(i).time_process,
								dt,
								GetCurrentStatus(i));
						}
					}
				}
				else if (GetCurrentStatus(0) == STATUS_MENU::M_IDLE)
				{
					bool res = OnUpdateFadeProcessChildByTagIdx(p_IdxUITagAppear,
						XMLMgr->GetUIByName(name_ui)->GetTimeProcess(p_IdxUITagAppear).time_process,
						dt,
						GetCurrentStatus(p_IdxUITagAppear));
				}

			}
		}

		void MenuEntity::onCheckPress()
		{
			//when fade in or out do not process touch button
			if (GetCurrentStatus() != STATUS_MENU::M_IDLE)
				return;

			//only can touch menu if it lays on the top of the menu ui stack 
			if (p_IdxMenuData != 0)
				return;

			//in some case the idxtagAppear have 1 , 2... so wait all the menu appear then can press button
			if (!p_IsCanPressButton)
				return;
            
            

			if (InputMgr->GetNumberTouch() > 0)
			{
				auto p = InputMgr->GetPosAtId(0);
				for (int i = 0; i < p_ListMemberInGame.Size(); i++)
				{
					if (p_ListMemberInGame.GetAt(i)->GetType() != UI_MENU_TYPE::MENU_BUTTON)
						continue;
					auto btn = (ButtonEntity*)(p_ListMemberInGame.GetAt(i)->GetResource());

					Vec2 p = btn->GetPosition();
					Vec2 s = btn->GetSize();
					//
					cocos2d::Rect rec(p.x - s.x / 2, p.y - s.y / 2, s.x, s.y);
					//
					int idx_appear = btn->GetIndexAppear();

					if (btn->Visible() && InputMgr->IsPressOnBound(rec) && GetActiveStatusAtidx(idx_appear) == true)
					{
						btn->IsPress(true);
						InputMgr->EndTouchOnScreen();
#if USE_CC_AUDIO
						if (p_IsPlaySoundPress)
						{
							GetSound->PlaySoundEffect(btn->GetIdSound()->id_click, btn->GetIdSound()->delayTime, btn->GetIdSound()->loop);
							p_IsPlaySoundPress = false;
						}
#endif
					}
					else
					{
						btn->IsPress(false);
					}
				}
			}
			else
			{
				for (int i = 0; i < p_ListMemberInGame.Size(); i++)
				{
					if (p_ListMemberInGame.GetAt(i)->GetType() != UI_MENU_TYPE::MENU_BUTTON)
						continue;
					auto btn = (ButtonEntity*)(p_ListMemberInGame.GetAt(i)->GetResource());
					if (btn->Visible() && btn->IsPress() == true)
					{
						auto p = btn->GetPosition();
						auto s = btn->GetSize();
						cocos2d::Rect rec(p.x - s.x / 2, p.y - s.y / 2, s.x, s.y);
						if (InputMgr->IsReleaseOnbutton(rec))
						{
							this->OnProcessButton(btn->GetName());
						}
						InputMgr->OnClearTouchReleaseList();
					}
					btn->IsPress(false);
				}
				p_IsPlaySoundPress = true;
			}

		}

		void  MenuEntity::InitTheChildElement(RKString str, xml::ResUIElement * re)
		{
			//
			RKString name_res = str;
			auto layout = re;
			auto posRatio = ConvertPos(layout->origin_PosRatio);
			auto firstPosRatio = ConvertPos(layout->first_PosRatio);
			auto pos = Vec2(GetGameSize().width * posRatio.x, GetGameSize().height * posRatio.y);

			auto size_origin = Vec2(layout->origin_ScaleRatio.x * GetGameSize().width, layout->origin_ScaleRatio.y * GetGameSize().width);
			auto size_first = Vec2(layout->first_ScaleRatio.x * GetGameSize().width, layout->first_ScaleRatio.y * GetGameSize().width);

			if (layout->stick_screen == 1)
			{
				size_origin = Vec2(layout->origin_ScaleRatio.x * GetGameSize().width, layout->origin_ScaleRatio.y * GetGameSize().height);
				size_first = Vec2(layout->first_ScaleRatio.x * GetGameSize().width, layout->first_ScaleRatio.y * GetGameSize().height);

			}
			//
			MenuUIMember * mbm = NULL;
			//
			if (layout->type == "Button")
			{
				ButtonEntity * btn = new ButtonEntity();
				btn->Init(name_res, layout->source, pos, size_origin, layout->sound_click->id_click, layout->sound_click->delayTime, layout->sound_click->loop, layout->origin_Rotate);
				//
				btn->SetFirstPos(Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y), !layout->not_fade_pos);
				btn->SetFirstSize(size_first, !layout->not_fade_size);

				btn->SetFirstAlpha((RKUtils::BYTE)layout->first_opacity, !layout->not_fade_opacity);
				btn->SetOriginAlpha((RKUtils::BYTE)layout->origin_opacity);

				btn->SetFirstAngle(layout->first_Rotate, !layout->not_fade_rotate);

				btn->InitSetFade(layout->actionFade, layout->TimeProcessFade);
				btn->SetValuePress(layout);
				//
				btn->SetOriginColor(Color3B(layout->Tint[0], layout->Tint[1], layout->Tint[2]), true);
				//
				btn->Visible(layout->visible == 1 ? true : false);
				btn->SetIndexAppear(layout->index_appear);
				//
				btn->retain();
				//
				mbm = new MenuUIMember(UI_MENU_TYPE::MENU_BUTTON, btn);
				//
				p_ListMemberInGame.Insert(mbm, 0);
				//
				CC_SAFE_RELEASE_NULL(btn);
			}
			else if (layout->type == "Panel")
			{
				PanelEntity * bg = new PanelEntity();
				bg->Init(name_res, layout->source, pos, size_origin, layout->origin_Rotate);
				//
				bg->SetFirstPos(Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y), !layout->not_fade_pos);
				bg->SetFirstSize(size_first, !layout->not_fade_size);
				bg->SetFirstAlpha((RKUtils::BYTE)layout->first_opacity, !layout->not_fade_opacity);
				bg->SetOriginAlpha((RKUtils::BYTE)layout->origin_opacity);
				bg->SetFirstAngle(layout->first_Rotate, !layout->not_fade_rotate);
				bg->InitSetFade(layout->actionFade, layout->TimeProcessFade);
				//
				bg->SetOriginColor(Color3B(layout->Tint[0], layout->Tint[1], layout->Tint[2]), true);
				//
				bg->Visible(layout->visible == 1 ? true : false);
				bg->SetIndexAppear(layout->index_appear);
				//
				bg->retain();
				//
				mbm = new MenuUIMember(UI_MENU_TYPE::MENU_PANEL, bg);
				//
				p_ListMemberInGame.Insert(mbm, 0);
				//
				CC_SAFE_RELEASE_NULL(bg);
			}
			else if (layout->type == "Text")
			{
				int fontIdx = layout->fontIdx;
				auto StrEntity = new TextEntity();
				bool IsUnicode = false;
				float size_text = XMLMgr->GetFontDecByIdx(fontIdx)->d_size;
				StrEntity->SetTextLabel(name_res, TextureMgr->GetFontByIdx(fontIdx), size_text, layout, IsUnicode);
				//
				StrEntity->SetFirstPos(Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y), !layout->not_fade_pos);
				auto value_text_size = (layout->first_ScaleRatio.x / size_text) * (GetGameSize().width / GetGameDesignSize().x);
				StrEntity->SetFirstScale(value_text_size, !layout->not_fade_size);
				StrEntity->SetFirstAlpha((RKUtils::BYTE)layout->first_opacity, !layout->not_fade_opacity);
				StrEntity->SetOriginAlpha((RKUtils::BYTE)layout->origin_opacity);
				StrEntity->SetFirstAngle(layout->first_Rotate, !layout->not_fade_rotate);
				StrEntity->InitSetFade(layout->actionFade, layout->TimeProcessFade);
				//
				StrEntity->IsVisible(layout->visible == 1 ? true : false);
				StrEntity->SetIndexAppear(layout->index_appear);
				//
				StrEntity->retain();
				//
				mbm = new MenuUIMember(UI_MENU_TYPE::MENU_TEXT, StrEntity);
				//
				p_ListMemberInGame.Insert(mbm, 0);
				//

				CC_SAFE_RELEASE_NULL(StrEntity);
			}

		}

		void MenuEntity::InitMenuEntity(RKString str)
		{
			name_ui = str;
			auto UIDec = XMLMgr->GetUIByName(name_ui);
			auto listElementName = UIDec->GetListChildName();
			p_time_process = UIDec->GetTimeProcess(0).time_process;
			p_max_idx_process_first = UIDec->max_idx_first;
			//
			for (int i = 0; i < listElementName.Size(); i++)
			{
				InitTheChildElement(listElementName[i], UIDec->GetElementChildByName(listElementName[i]));
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

		void MenuEntity::ResetSizeAndPosMenuEntity(float cur_width, float cur_height)
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				p_ListMemberInGame.GetAt(i)->GetResource()->ResetSizeAndPosResource(cur_width, cur_height);
				if (p_ListMemberInGame.GetAt(i)->GetType() == UI_MENU_TYPE::MENU_BUTTON)
				{
					((ButtonEntity*)p_ListMemberInGame.GetAt(i)->GetResource())->SetScaleNormal(
						Vec2(p_ListMemberInGame.GetAt(i)->GetResource()->GetResource()->getScaleX(),
						p_ListMemberInGame.GetAt(i)->GetResource()->GetResource()->getScaleY())
						);
				}				
			}

			onSpecificSizeChange(cur_width,cur_height);
		}

		
		void MenuEntity::OnReloadString()
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				if (p_ListMemberInGame.GetAt(i)->GetType() != UI_MENU_TYPE::MENU_TEXT)
					continue;
				auto txt = (TextEntity*)(p_ListMemberInGame.GetAt(i)->GetResource());
				txt->ReloadText();
			}
		}

		bool MenuEntity::OnSetActionFadeAllResource(STATUS_MENU stateFade, unsigned int idx_tag_appear)
		{
			bool have_value = false;
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				if (p_ListMemberInGame.GetAt(i)->GetResource()->GetIndexAppear() != idx_tag_appear)
				{
					continue;
				}
				have_value = true;
				p_ListMemberInGame.GetAt(i)->GetResource()->SetActionFade(stateFade);
			}
			SetStatusAtIdx(stateFade, idx_tag_appear);
			if (have_value)
				p_DeltaMoveTime = XMLMgr->GetUIByName(name_ui)->GetTimeProcess(idx_tag_appear).time_process;
			return have_value;
		}

		bool MenuEntity::OnUpdateFadeProcessChildByTagIdx(unsigned int idx_tag_appear, float process_time, float dt, STATUS_MENU stateFade)
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
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				if (p_ListMemberInGame.GetAt(i)->GetResource()->GetIndexAppear() != idx_tag_appear)
					continue;
				AllFinish &= p_ListMemberInGame.GetAt(i)->Update(dt);
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
						OnDeactiveCurrentMenu();
						//clear all the fade process tag when deactive an ui menu
						RemoveActionFadeAllChild(idx_tag_appear);
						p_DeltaMoveTime = 0;
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
			}

			return AllFinish;
		}

		void MenuEntity::RemoveActionFadeAllChild(int idx)
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				if (p_ListMemberInGame.GetAt(i)->GetResource()->GetIndexAppear() != idx)
					continue;
				p_ListMemberInGame.GetAt(i)->GetResource()->RemoveActionFadeByTag(true);
			}
		}

		STATUS_MENU MenuEntity::GetCurrentStatus(int idx)
		{
			if (idx >= 0 && idx < p_ListStatus.size())
				return p_ListStatus.at(idx)->p_curStatus;
			return STATUS_MENU::M_IDLE;
		}

		void MenuEntity::SetStatusAtIdx(STATUS_MENU status, int idx)
		{
			if (idx >= 0 && idx < p_ListStatus.size())
			{
				auto status__ = p_ListStatus.at(idx);
				p_ListStatus.erase(p_ListStatus.begin() + idx);
				status__->p_curStatus = status;
				p_ListStatus.insert(p_ListStatus.begin() + idx, status__);
			}
		}

		bool MenuEntity::GetActiveStatusAtidx(int idx)
		{
			if (idx >= 0 && idx < p_ListStatus.size())
				return p_ListStatus.at(idx)->p_IsActive;
			return STATUS_MENU::M_IDLE;
		}
		void MenuEntity::SetActiveStatusAtIdx(bool active, int idx)
		{
			if (idx >= 0 && idx < p_ListStatus.size())
			{
				auto status__ = p_ListStatus.at(idx);
				if (status__->p_IsActive == active)
					return;
				p_ListStatus.erase(p_ListStatus.begin() + idx);
				status__->p_IsActive = active;
				p_ListStatus.insert(p_ListStatus.begin() + idx, status__);
			}
		}

		void MenuEntity::OnFadeIn()
		{
			SetStatusAtIdx(STATUS_MENU::M_FADEIN, 0);
			OnSetActionFadeAllResource(GetCurrentStatus());

			OnBeginFadeIn();
		}

		void MenuEntity::OnFadeOut()
		{
			int maximum_idx_appear = 0;
			for (int i = 0; i < p_ListStatus.size(); i++)
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

		void MenuEntity::OnShow()
		{
			OnFadeIn();

		}

		void MenuEntity::OnHide()
		{
			//
			OnFadeOut();
		}

		MenuResource* MenuEntity::GetResourceByName(RKString name)
		{
			for (int i = 0; i < p_ListMemberInGame.Size(); i++)
			{
				MenuResource* res = p_ListMemberInGame.GetAt(i)->GetResource();
				if (res->GetName() == name)
				{
					return res;
				}
			}
			return 0;
		}

		void MenuEntity::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
		{
			//the abstrast class may be not implement, which class use, that class implement ex: SelectionMode UI
			if (status == STATUS_MENU::M_FADEIN)
				p_IsCanPressButton = true;
			else
				p_IsCanPressButton = false;
		}

		void MenuEntity::OnProcessWhenEndOrBegin(STATUS_MENU cur_status)
		{
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

		void MenuEntity::OnProcessWhenTagIdxMulti(int max_idx, int idx, STATUS_MENU cur_status, STATUS_MENU status)
		{
			if (status == STATUS_MENU::M_FADEIN) //fade in
			{
				if (idx < max_idx)
				{
					p_IdxUITagAppear++;
					bool res = OnSetActionFadeAllResource(cur_status, p_IdxUITagAppear);
					if (!res)
					{
						p_IdxUITagAppear = 0;
					}
				}
				else if (idx >= max_idx)
				{
					p_IsCanPressButton = true;
					p_IdxUITagAppear = 0;
				}

				SetActiveStatusAtIdx(true, idx);
			}
			else
			{
				SetActiveStatusAtIdx(false, idx);
				if (p_ListStatus.at(0)->p_curStatus != STATUS_MENU::M_IDLE)
					p_IsCanPressButton = false;

				bool IsAllDeActive = true;
				if (p_ListStatus.size() > 1)
				{
					for (int i = max_idx < 1 ? 0 : 1; i < p_ListStatus.size(); i++)
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

		void MenuEntity::OnAllProcessFinishFadeIdx(int idx, int max_idx, STATUS_MENU status)
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

		void MenuEntity::OnShowOrHideIdxTarget(int idx_target)
		{
			if (idx_target >= p_ListStatus.size() || GetCurrentStatus(idx_target) != STATUS_MENU::M_IDLE)
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

		void MenuEntity::OnBeginFadeIn()
		{
			//abstrast class not impelemt this 
			return;
		}

		void MenuEntity::OnFadeInComplete()
		{
			//abstrast class not impelemt this 
			return;
		}

		void MenuEntity::OnBeginFadeOut()
		{
			//abstrast class not impelemt this 
			return;
		}

		void MenuEntity::OnFadeOutComplete()
		{
			//abstrast class not impelemt this 
			return;
		}

		void MenuEntity::onSpecificSizeChange(float cur_width, float cur_height)
		{
			//abstrast class not impelemt this 
			return;
		}

	}
}