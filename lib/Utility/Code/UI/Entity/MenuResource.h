#ifndef __MENU_RESOURCE_H__
#define __MENU_RESOURCE_H__

#include "RKTool_Code/RKList.h"
#include "cocos2d.h"


#include "../../DisplayView.h"
#include "../../ObjectTexture.h"
#include "../../LoadDataXML.h"

#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"


using namespace RKUtils;
using namespace ui;
namespace Utility
{

	namespace UI
	{
		
		enum UI_MENU_TYPE
		{
			MENU_UI_NONE = 0,
			MENU_BUTTON,
			MENU_PANEL,
			MENU_TEXT,
			MENU_LISTVIEW,
			MENU_SCROLLVIEW,
			MENU_PAGEVIEW,
			MENU_UI_COUNT
		};
		enum STATUS_MENU
		{
			M_IDLE = 0,
			M_FADEIN,
			M_FADEOUT
		};
		class MenuResource : public Ref
		{
		protected:
			//
			Sprite*		p_Resource;
			LABEL *		p_textLabel;
			ListView*	p_listView;
			//
			unsigned int p_action_fade;
			unsigned int p_count_action_fade; //use to know wherever all the action fade finish

			float		 p_timeAction;
			//
			bool p_IsVisible;
			//
			Vec2 p_originPos;
			Vec2 p_firstPos;
			//
			float p_originAngle;
			float p_firstAngle;
			//
			RKUtils::BYTE p_first_fade_alpha;
			RKUtils::BYTE p_target_fade_alpha;
			//
			float p_first_scale;
			float p_target_scale_to;
			//
			Color3B p_origin_tint;
			Color3B p_first_tint;

			//
			UI_MENU_TYPE p_typeUI;
			//
			RKString	p_Name;
			//
			STATUS_MENU p_currentStatusMenu;
			//
			RKList<UI_STATE_FADE_ACTION> p_l_fd;

			void VisitResource(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void DrawResource(Renderer *renderer, const Mat4& transform, uint32_t flags);

			//
			void AddActionFade(UI_STATE_FADE_ACTION fd);
			void RemoveActionFade(UI_STATE_FADE_ACTION fd);
			void ResetActionFade();
			bool ContainActionFade(UI_STATE_FADE_ACTION fd);

			Sequence * GetSequenceActionFromList(STATUS_MENU stateFade, RKList<UI_STATE_FADE_ACTION> l_fd, float time);

			void LoadResourceFromSingleImage(RKString name);
			void LoadResourceFromSingleTexture(Texture2D * tx);
			void LoadResourceFromData(RKString str);
			void parseSprite(Sprite * tx);
			//
			unsigned int p_index_appear;

		public:

			cocos2d::Node * GetResource() {
				if (p_typeUI == UI_MENU_TYPE::MENU_BUTTON || p_typeUI == UI_MENU_TYPE::MENU_PANEL)
				{
					return p_Resource;
				}
				else if (p_typeUI == UI_MENU_TYPE::MENU_TEXT)
				{
					return p_textLabel;
				}
				else if (p_typeUI == UI_MENU_TYPE::MENU_LISTVIEW)
				{
					return p_listView;
				}
				return nullptr;
			}

			MenuResource();		
			~MenuResource();			
			
			bool VisibleResource();
			void VisibleResource(bool b);

			void SetScaleResource(Vec2 s);
			Vec2 GetScaleResource();

			Vec2 GetSizeResource();
			void SetSizeResource(Vec2 s);

			void SetRotateResource(float r);
			float GetRotateResource();

			void SetPositionResource(Vec2 p);
			Vec2 GetPositionResource();

			void SetOpacityResource(RKUtils::BYTE p);
			RKUtils::BYTE GetOpacityResource();

			void SetColorResource(Color3B p);
			Color3B GetColorResource();

			unsigned int GetIndexAppear();
			void SetIndexAppear(unsigned int idx);

			Vec2 GetOriginPos(){ return p_originPos; }
			Vec2 GetFirstPos() { return p_firstPos; }
			void SetFirstPos(Vec2 p, bool force = true) { 
				p_firstPos = p; 
				if (force)
				{
					SetPositionResource(p);
				}
			}
			//
			float GetOriginAngle(){ return p_originAngle; }
			float GetFirstAngle() { return p_firstAngle; }
			void SetFirstAngle(float p, bool force = true) { 
				p_firstAngle = p; 
				if (force)
				{
					SetRotateResource(p);
				}
			}
			//
			RKUtils::BYTE GetOriginAlpha(){ return p_target_fade_alpha; }
			void SetOriginAlpha(RKUtils::BYTE p, bool force = false) {
				p_target_fade_alpha = p;
				if (force)
				{
					SetOpacityResource(p);
				}
			}
			RKUtils::BYTE GetFirstAlpha() { return p_first_fade_alpha; }
			void SetFirstAlpha(RKUtils::BYTE p, bool force = true) {
				p_first_fade_alpha = p;
				if (force)
				{
					SetOpacityResource(p);
				}
			}
			//
			Color3B GetOriginColor(){ return p_origin_tint; }
			void SetOriginColor(Color3B p, bool force = false) {
				p_origin_tint = p;
				if (force)
				{
					SetColorResource(p);
				}
			}
			Color3B GetFirstColor() { return p_first_tint; }
			void SetFirstColor(Color3B p, bool force = true) {
				p_first_tint = p;
				if (force)
				{
					SetColorResource(p);
				}
			}
			//
			float GetOriginScale(){ return p_target_scale_to; }
			float GetFirstScale() { return p_first_scale; }
			void SetFirstSize(Vec2 p, bool force = true) {
				if (force)
				{
					SetSizeResource(p);
				}
				p_first_scale = p_Resource->getScaleX();
			}
			void SetFirstScale(float p, bool force = true) {	
					p_first_scale = p;
					if (force)
					{
						SetScaleResource(Vec2(p, p));
					}
			}

			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual bool Update(float dt) = 0;

			void SetActionToFade(unsigned int fd);

			void SetActionFade(STATUS_MENU stateFade, bool force_only= false);
			void FadeFinish(UI_STATE_FADE_ACTION current_fd);
			void FadeFinish1(UI_STATE_FADE_ACTION current_fd);

			void InitSetFade(RKList<UI_STATE_FADE_ACTION> l_fd, float time);

			void SetResourceRunAction(UI_STATE_FADE_ACTION action, int loop, bool reserve, float time = 0.f);

			void SetResourceRunManyAction(std::vector<Vector<FiniteTimeAction*>> list_action_);

			Vector<FiniteTimeAction*> CreateListActionFromSingleAction(UI_STATE_FADE_ACTION action, int loop, bool reserve, float time = 0.f);

			Sequence * CreateActionFromList(Vector<ActionInterval*> list_action);

			bool HaveActionNotFade();

			bool UpdateActionFade(float dt);

			void RemoveActionFadeByTag(bool IsAll = false);

			RKString GetName(){ return p_Name; }

			void ResetSizeAndPosResource(int cur_width, int cur_height);
		};
	}
}

#endif //__MENU_RESOURCE_H__

