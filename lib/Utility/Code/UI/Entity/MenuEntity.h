#ifndef __MENU_ENTITY_H__
#define __MENU_ENTITY_H__


#include "MenuResource.h"
#include "ButtonEntity.h"
#include "PanelEntity.h"
#include "TextEntity.h"

#include "cocos2d.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI
	{
		class MenuEntity;
		//
		class MenuUIMember
		{
		private:
			UI_MENU_TYPE	p_type;
			MenuResource *	p_res;
		public:
			MenuUIMember()
			{
				p_type = UI_MENU_TYPE::MENU_UI_NONE;
				p_res = NULL;
			}
			MenuUIMember(UI_MENU_TYPE type, MenuResource * data)
			{
				p_type = type;
				p_res = data;
			}
			~MenuUIMember()
			{
				if (p_res)
				{
					delete p_res;
					p_res = NULL;
				}
			}
			void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
			{			
				p_res->Draw(renderer, transform, flags);
			}

			void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
			{
				p_res->Visit(renderer, transform, flags);
			}

			bool Update(float dt)
			{
				return p_res->Update(dt);
			}

			UI_MENU_TYPE GetType() { return p_type; }
			MenuResource * GetResource() { return p_res; }

		};
		//
		struct UIDecs
		{
			STATUS_MENU p_curStatus;
			bool		p_IsActive;
			RKString	p_SoundFadeIn;
			RKString	p_SoundFadeOut;
			float		p_delayTime;
			int			p_loop;
		public:
			UIDecs()
			{
				p_curStatus = STATUS_MENU::M_IDLE;
				p_IsActive = false;
				p_SoundFadeIn = "";
				p_SoundFadeOut = "";

				p_delayTime = 0;
				p_loop = 0;

			}

			UIDecs(RKString sou_1, RKString sou_2, bool IsActive)
			{
				p_curStatus = STATUS_MENU::M_IDLE;
				p_IsActive = IsActive;
				p_SoundFadeIn = sou_1;
				p_SoundFadeOut = sou_2;

				p_delayTime = 0;
				p_loop = 0;
			}

			UIDecs(RKString sou_1, RKString sou_2, bool IsActive, float delayTime, int loop)
			{
				p_curStatus = STATUS_MENU::M_IDLE;
				p_IsActive = IsActive;
				p_SoundFadeIn = sou_1;
				p_SoundFadeOut = sou_2;

				p_delayTime = delayTime;
				p_loop = loop;
			}
		};


		class MenuEntity : public Ref
		{
		protected:
			RKList<MenuUIMember*>	    p_ListMemberInGame;

			std::vector<UIDecs *>		p_ListStatus;

			float p_DeltaMoveTime;
            cocos2d::Point p_DeltaMoveDir;
			float p_DistanceDelta;

			cocos2d::Point p_DeltaPosWithBGMain;

			bool p_IsPlaySoundPress;

			int p_IdxMenuData;
			int p_IdxUITagAppear;

			void InitTheChildElement(RKString str, xml::ResUIElement* re);
			void InitMenuEntity(RKString str);

			float p_time_process;

			int p_max_idx_process_first;

			RKString name_ui;

			bool p_IsCanPressButton;
		protected:

			void onDrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void onVisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void onUpdateMenu(float dt);
			//
			void RemoveActionFadeAllChild(int idx);
			//
			void onCheckPress();
			//
			virtual void OnProcessWhenEndOrBegin(STATUS_MENU cur_status);
			virtual void OnProcessWhenTagIdxMulti(int max_idx, int idx, STATUS_MENU cur_status, STATUS_MENU status);
		public:
			RKString GetNameUI() { return name_ui; }
			void SetIdxMenuData(int idx) { p_IdxMenuData = idx; }

			MenuEntity();
			virtual ~MenuEntity();
			//
			virtual int	Init() = 0;
			virtual void	Update(float dt) = 0;
			virtual void	Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void	Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			//
			virtual void OnFadeIn() ;
			virtual void OnFadeOut() ;
			//
			virtual void OnShow() ;
			virtual void OnHide() ;
			//
			virtual void OnProcessButton(RKString name) = 0;
			virtual void OnDeactiveCurrentMenu() = 0 ;

			virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status);

			STATUS_MENU GetCurrentStatus(int idx = 0);
			void SetStatusAtIdx(STATUS_MENU status, int idx);
			//
			bool GetActiveStatusAtidx(int idx);
			void SetActiveStatusAtIdx(bool active, int idx);
			//
			virtual void OnReloadString();
			//
			MenuResource* GetResourceByName(RKString name_str);
			//
			bool OnSetActionFadeAllResource(STATUS_MENU stateFade, unsigned int idx_tag_appear = 0);
			//
			// return the fade process finish or not
			bool OnUpdateFadeProcessChildByTagIdx(unsigned int idx_tag_appear, float process_time, float dt, STATUS_MENU stateFade);
			
			//use this to call any idx target to appear or disappear except at 0
			virtual void OnShowOrHideIdxTarget(int idx_target);

			//if UI have more 1 idx target call this function
			virtual void OnAllProcessFinishFadeIdx(int idx, int max_idx, STATUS_MENU status);

			//happen when begin fade in 
			virtual void OnBeginFadeIn();

			//happen when fade in complete
			virtual void OnFadeInComplete();

			//happen when begin fade out 
			virtual void OnBeginFadeOut();

			//happen when fade out complete
			virtual void OnFadeOutComplete();

			virtual void onSpecificSizeChange(float cur_width, float cur_height);
			void ResetSizeAndPosMenuEntity(float cur_width, float cur_height);

		};

	}
}

#endif //__MENUENTITY_H__

