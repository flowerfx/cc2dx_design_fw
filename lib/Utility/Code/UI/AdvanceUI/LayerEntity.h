#ifndef __LAYER_ENTITY_H__
#define __LAYER_ENTITY_H__

#include "WidgetEntity.h"
#include "../../Factory.h"
//#include "wTimeProcessWidget.h"
//#include "wListViewTable.h"
using namespace RKUtils;
using namespace ui;
namespace Utility
{
	namespace UI_Widget
	{

		enum class EventType_SetCell;
		enum class EventType_Process;
		enum class EventType_Digital;
		enum class EventType_Anim;
        enum class EventType_TextField;

		class LayerEntity : public Layer, public ActionProcess
		{
		private:

			std::vector<WidgetEntity* > p_listWidgetChild;
			std::vector<LayerEntity* >	p_listLayerChild;
			Factory::FactoryWithIndex<WidgetEntity>	p_factory_ui_widget;
			bool p_isRootLayer;

		protected:

			int p_current_idx_appear;
			int p_number_child_have_action;

			void RegisterUIWidget();
			void ReleaseChildWidget();

		protected:

			std::function<void(Screen&, RKString, UI_TYPE , WidgetEntity*)> func_call_widget;

			void AddWidgetWithName(Widget * element, RKString name);

			virtual void OnTouchEvent(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type, WidgetEntity* _widget);
			virtual void OnSelectListItem(cocos2d::Ref* sender, cocos2d::ui::ListView::EventType type, WidgetEntity* _widget);
			virtual void OnTextFieldEvent(cocos2d::Ref* sender, EventType_TextField type, WidgetEntity* _widget);
            
			virtual void OnSliderEvent(cocos2d::Ref* sender, cocos2d::ui::Slider::EventType type, WidgetEntity* _widget);
			virtual void OnPageViewEvent(cocos2d::Ref* sender, cocos2d::ui::PageView::EventType type, WidgetEntity* _widget);
			virtual void OnProgressTimer(cocos2d::Ref* sender, EventType_Process type, WidgetEntity* _widget);
			virtual void OnScrollWidgetEvent(cocos2d::Ref* sender, cocos2d::ui::ScrollView::EventType type, WidgetEntity* _widget);
			virtual void SetInitPos(xml::UILayerWidgetDec * xml_value);
			virtual void OnMovePageEvent(cocos2d::Ref* sender, cocos2d::ui::ListView::EventMovePage type, WidgetEntity* _widget);
			virtual void OnSetCellTable(cocos2d::Ref* sender, EventType_SetCell type, WidgetEntity* _widget);
			virtual void OnDigitalTimer(cocos2d::Ref* sender, EventType_Digital type, WidgetEntity* _widget);
			virtual void OnCheckBoxEvent(cocos2d::Ref* sender, cocos2d::ui::CheckBox::EventType type, WidgetEntity* _widget);
			virtual void OnFinishAnimate(cocos2d::Ref* sender, EventType_Anim type, WidgetEntity* _widget);
		public:

			Widget * GetWidgetElementByName(RKString name);
			WidgetEntity * GetWidgetEntityByName(RKString name);
		

			void AddLayerEntity(LayerEntity * layer);
			void AddWidgetEntity(WidgetEntity * layer);
			bool RemoveWidgetEntity(WidgetEntity * layer);
			bool RemoveWigetEnityByName(RKString name);

			LayerEntity();
			virtual ~LayerEntity();

			virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;
			virtual void visit(Renderer *renderer, const Mat4& transform, uint32_t flags)	override;

			virtual bool Update(float dt)													override;
			virtual void SetPosValueXML(xml::UILayerWidgetDec * xml_value)					override;
			virtual bool IsAllChildFinishAction()											override;
			virtual void SetInteractive(bool value)											override;
			virtual void SetDisable(bool value)												override;
			virtual void ReloadText()														override;
			virtual void ClearChild(int count)												override;
			virtual cocos2d::Node * GetResource()											override;
			virtual bool SetNewResource(cocos2d::Node * resource)							override;

			virtual WidgetEntity * GetWidgetChildAtIdx(int idx);
			virtual WidgetEntity * GetWidgetChildByName(RKString name_path);
			virtual LayerEntity	 * GetLayerChildByName(RKString name_str);

			WidgetEntity * InitWidgetEntity(xml::UILayerWidgetDec * widget, LayerEntity * parent_layer, WidgetEntity * parent_widget);
			void SetCallBack(std::function<void(Screen&, RKString, UI_TYPE, WidgetEntity*)> func);
			void SetActionProcessForAllChild(STATUS_MENU fade_state, unsigned int current_idx);
			void SetIdxProcessGame(int idx);
			std::vector<LayerEntity* > GetListLayerChild();
			void HaveChildFinishAction();
			bool UpdateActionProcessChild(float dt, int current_idx);
			void AddEventType(xml::TYPE_UI_WIDGET type, WidgetEntity * widget, xml::UILayerWidgetDec* _widget = nullptr);
			bool HaveChildWithIdxAppear(int checked_idx_appear);
			bool IsRootLayer();
			void SetRootLayer(bool value);
		};
	}
}

#endif //__LAYER_ENTITY_H__

