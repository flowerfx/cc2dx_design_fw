#ifndef __EDIT_BOX_WIDGET_H__
#define __EDIT_BOX_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
#ifdef USE_EDIT_BOX_SEP
        enum class EventType_EditBox
        {
            ON_INSERT_TEXT = 0,
            ON_DELETE_TEXT,
            ON_ATTACH_WITH_IME,
            ON_DETACH_WITH_IME
        };
        
		class EditBoxWidget : public WidgetEntity , public cocos2d::ui::EditBoxDelegate
		{
		private:
			RKString	p_current_text;
			RKString	p_default_place_holder;
            
            
		public:
			EditBoxWidget();
			virtual ~EditBoxWidget();

			virtual void SetSize(Vec2 s)												override;
			virtual void InitParam(RKString name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetColor(Color3B p)											override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;

			RKString GetText();

			void SetText(RKString text);
			RKString GetDefaultPlaceHolder();


			virtual void ReloadText();
            
            //override from cocos to handle event
			virtual void editBoxReturn(EditBox* editBox)                                override;
            virtual void editBoxEditingDidBegin(EditBox* editBox)                       override;
            virtual void editBoxEditingDidEnd(EditBox* editBox)                         override;
            virtual void editBoxTextChanged(EditBox* editBox, const std::string& text)  override;
            
            typedef std::function<void(Ref*, EventType_EditBox)> wEditBoxEventHandler;
            void AddEventHandler(const wEditBoxEventHandler & event);
        private:
            wEditBoxEventHandler _event_callback;

		};
#endif

	}
}

#endif //__EDIT_BOX_WIDGET_H__

