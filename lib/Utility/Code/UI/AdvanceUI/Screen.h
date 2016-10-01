#ifndef __MENU_WIDGET_H__
#define __MENU_WIDGET_H__

#pragma once

#include "LayerEntity.h"
//#include "cocos2d.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class Screen;
        class WidgetEntity;
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
			UIDecs();	
			UIDecs(RKString sou_1, RKString sou_2, bool IsActive);
			UIDecs(RKString sou_1, RKString sou_2, bool IsActive, float delayTime, int loop);
		};

		class Screen : public Layer
		{
		protected:

			std::vector<LayerEntity*>		p_listLayerWidget;
			std::vector<UIDecs *>		p_ListStatus;

			RKString name_ui;
			float p_DeltaMoveTime;
			int p_IdxMenuData;
			int p_IdxUITagAppear;

			LayerEntity * InitTheChildElement(RKString str, LayerEntity * parent_layer, xml::UILayerWidgetDec* re);
			void InitMenuWidgetEntity(RKString str);
			void InitMenuWidgetEntityWithParam(RKString name , Vec2 design_size);

			float p_time_process;
			int p_max_idx_process_first;

			//stop update of screen
			bool p_IsStopUpdate;
			//can handle touch to screen
			bool p_IsMenuInteractive;

			Vec2 p_design_size;
            
            // 0 is none, 1 is shown 2 is hide
            int  p_stage_show_keyboard;            
            WidgetEntity * p_textfield_infocus;

		protected:

			void onDrawMenuWidget(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void onVisitMenuWidget(Renderer *renderer, const Mat4& transform, uint32_t flags);
			void onUpdateMenuWidget(float dt);
			//
			void RemoveActionFadeAllChild(int idx);
			//
			virtual void OnProcessWhenEndOrBegin(STATUS_MENU cur_status);
			virtual void OnProcessWhenTagIdxMulti(int max_idx, int idx, STATUS_MENU cur_status, STATUS_MENU status);
		public:
			RKString GetNameUI();
			void SetIdxMenuData(int idx);

			virtual void AddChildWidget(WidgetEntity * widget);
			virtual void RemoveChildWidget(WidgetEntity * widget);
			virtual void RemoveChildWidgetByName(RKString name);
			virtual LayerEntity* GetRoot();

			Screen();
			virtual ~Screen();
			//
			virtual int	Init() = 0;
			virtual int	Update(float dt) = 0;
			virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags) = 0;
			virtual void	RegisterKeyTouch();
			//
			virtual void OnFadeIn() ;
			virtual void OnFadeOut() ;
			//
			virtual void OnShow() ;
			virtual void OnHide() ;
			//
			virtual void OnProcessWidgetChild(RKString name , UI_TYPE type_widget, WidgetEntity * _widget) = 0;
			void OnCheckTouchEvent(RKString name, UI_TYPE type_widget, WidgetEntity * _widget);

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
			bool OnSetActionFadeAllResource(STATUS_MENU stateFade, unsigned int idx_tag_appear = 0);
			//
			// return the fade process finish or not
			bool OnUpdateFadeProcessChildByTagIdx(unsigned int idx_tag_appear, float process_time, float dt, STATUS_MENU stateFade);

			virtual WidgetEntity * GetWidgetChildByName(RKString name_path);
			virtual LayerEntity	 * GetLayerChildByName(RKString name_str);

			//
			virtual bool OnCallProcessOfChild(RKString name_child_path, RKString command_str, int type = 0);

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

			//happend when have touch on the menu
			virtual void OnUpdateTouchMenu();
			virtual void OnTouchMenuBegin(cocos2d::Point p);
			virtual void OnTouchMenuEnd(cocos2d::Point p);

			virtual void onSpecificSizeChange(float cur_width, float cur_height);
			void ResetSizeAndPosMenuEntity(float cur_width, float cur_height);

			int GetMaxIdxFirst();

			//method with cocos2dx
			virtual void push_layer_to_main_scene(int order = 0 );

			virtual void remove_layer_from_main_scene();

			//
			void SetLayerInteractive(bool value);

			void SetChildInteractive(RKString name_path, bool value);

			bool IsInteractive();

			Vec2 GetDesignSize();

			//
			virtual void onLanguageChanged(LANGUAGE_TYPE lang);
			//handle key and touch
			virtual void onKeyHold(float interval);
			virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
			virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

			virtual void onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			virtual void onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			virtual void onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event);
			virtual void onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event  *event);
            
			void ReloadUI(RKString path);
			
            bool MoveScreenUpWhenKeyboardAppear(cocos2d::Rect rect_text_field);
            
            bool ResetScreenWhenKeyboardHide();
            
            void SetStageShowKeyboard(int stage , WidgetEntity * text_field = nullptr);
            
            void HandleMoveKeyboard();

			void SetStopUpdate(bool value);
			
		};

	}
}

#endif //__MENU_WIDGET_H__

