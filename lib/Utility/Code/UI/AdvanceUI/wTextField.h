#ifndef __TEXT_FIELD_WIDGET_H__
#define __TEXT_FIELD_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
        
        enum class EventType_TextField
        {
            ON_INSERT_TEXT = 0,
            ON_DELETE_TEXT,
            ON_ATTACH_WITH_IME,
            ON_DETACH_WITH_IME
        };
        
		class TextFieldWidget : public WidgetEntity
		{
		protected:
			RKString	p_current_text;
			RKString	p_default_place_holder;
		public:
			TextFieldWidget();
			virtual ~TextFieldWidget();
            
            virtual void CopyValue(WidgetEntity * value)								override;
            virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
            
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value) {};
            
            virtual RKString GetText();
            virtual void ClearText() {};
            virtual void SetText(RKString text) {};
            virtual RKString GetDefaultPlaceHolder();

            virtual void SetTextArea(Vec2 size) {};
            virtual void SetTouchZone(Vec2 size){};
            
            typedef std::function<void(Ref*, EventType_TextField)> wEditBoxEventHandler;
            void AddEventHandler(const wEditBoxEventHandler & event);
        protected:
            wEditBoxEventHandler _event_callback;
		};
        
        class TextFieldHandleWidget : public TextFieldWidget
        {
        private:
            int			p_border_width;
            Color4B		p_border_color;
            
            bool		p_size_use_scale;
            int			p_use_origin_size;
        public:
            TextFieldHandleWidget();
            virtual ~TextFieldHandleWidget();
            
            virtual void SetSize(Vec2 s)												override;
            virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value)		override;
            virtual void SetColor(Color3B p)											override;
            
            virtual RKString GetText()                                                  override;
            virtual void ClearText()                                                    override;
            virtual void SetText(RKString text)                                         override;
            
            virtual WidgetEntity * Clone()												override;
            virtual void CopyValue(WidgetEntity * value)								override;
            virtual void ReloadText()													override;
            virtual void SetInteractive(bool value)										override;
            
            virtual void SetTextArea(Vec2 size)                                         override;
            virtual void SetTouchZone(Vec2 size)                                        override;
            
            void OnHappenEvent(Ref * ref , ui::TextField::EventType event_type);
            
        };
        
        class EditBoxWidget : public TextFieldWidget , public cocos2d::ui::EditBoxDelegate
        {
        public:
            EditBoxWidget();
            virtual ~EditBoxWidget();
            
            virtual void SetSize(Vec2 s)												override;
            virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value)		override;
            virtual void SetColor(Color3B p)											override;
            virtual WidgetEntity * Clone()												override;
            virtual void CopyValue(WidgetEntity * value)								override;
            
            RKString GetText();
            void SetText(RKString text);
            
            virtual void ReloadText()                                                   override;
            
            //override from cocos to handle event
            virtual void editBoxReturn(EditBox* editBox)                                override;
            virtual void editBoxEditingDidBegin(EditBox* editBox)                       override;
            virtual void editBoxEditingDidEnd(EditBox* editBox)                         override;
            virtual void editBoxTextChanged(EditBox* editBox, const std::string& text)  override;
            
            virtual void SetInteractive(bool value)										override;
            
        };

	}
}

#endif //__TEXT_FIELD_WIDGET_H__

