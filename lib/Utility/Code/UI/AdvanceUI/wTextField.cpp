#include "wTextField.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		TextFieldWidget::TextFieldWidget()
		{
			p_current_text = "";
			p_default_place_holder = "";
            _event_callback = nullptr;

		}
		TextFieldWidget::~TextFieldWidget()
		{
			p_current_text = "";
			p_default_place_holder = "";
            _event_callback = nullptr;
		}

		void TextFieldWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TextFieldWidget*>(value);
			WidgetEntity::CopyValue(value);
			//
			p_current_text = val->p_current_text;
			p_default_place_holder = val->p_default_place_holder;
            _event_callback = val->_event_callback;
		}

        void TextFieldWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
        {
            InitTextureTextField(xml_value);
        }
        
        RKString TextFieldWidget::GetDefaultPlaceHolder()
        {
            return p_default_place_holder;
        }
        
        RKString TextFieldWidget::GetText()
        {
            return "";
        }
    
        void TextFieldWidget::AddEventHandler(const wEditBoxEventHandler & event)
        {
            _event_callback = event;
        }
        
        /////////////////////////////////////////////////////////////
        TextFieldHandleWidget::TextFieldHandleWidget()
        {
            p_typeUI = UI_TYPE::UI_TEXT_FIELD;
            p_border_width = 0;
            p_border_color = Color4B::WHITE;
            p_size_use_scale = false;
            
        }
        
        TextFieldHandleWidget::~TextFieldHandleWidget()
        {
        }
        
        void TextFieldHandleWidget::CopyValue(WidgetEntity * value)
        {
            auto val = static_cast<TextFieldHandleWidget*>(value);
            TextFieldWidget::CopyValue(value);
            //
            p_border_width = val->p_border_width;
            p_border_color = val->p_border_color;
            p_size_use_scale = val->p_size_use_scale;
            p_use_origin_size = val->p_use_origin_size;
            if (p_border_width > 0)
            {
                static_cast<Text*>(p_Resource)->enableOutline(p_border_color, p_border_width);
            }
        }
        
        WidgetEntity * TextFieldHandleWidget::Clone()
        {
            TextFieldHandleWidget * p_clone = new TextFieldHandleWidget();
            p_clone->CopyValue(this);
            //remove child remain
            p_clone->p_Resource->removeAllChildrenWithCleanup(true);
            
            
            //copy child
            CloneChild(p_clone);
            CloneThis(p_clone);
            
            return p_clone;
        }
        
        /////////////////////////////////////////////////////////////
        void TextFieldHandleWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
        {
            InitTexture(xml_value);
            WidgetEntity::InitParam(name, xml_value);
            
            if (xml_value->GeneralValue->HaveDataVector2("touch_zone"))
            {
                auto value_touch_zone = xml_value->GeneralValue->GetDataVector2("touch_zone");
                value_touch_zone.x = value_touch_zone.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
                value_touch_zone.y = value_touch_zone.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
                static_cast<TextField*>(GetResource())->setTouchAreaEnabled(true);
                static_cast<TextField*>(GetResource())->setTouchSize(cocos2d::Size(value_touch_zone.x, value_touch_zone.y));
            }
            
            if (xml_value->GeneralValue->HaveDataVector2("text_area"))
            {
                auto value_text_area = xml_value->GeneralValue->GetDataVector2("text_area");
                value_text_area.x = value_text_area.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
                value_text_area.y = value_text_area.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
                static_cast<TextField*>(GetResource())->SetTextArea(Vec2(value_text_area.x, value_text_area.y));
            }
            
            if (xml_value->GeneralValue->HaveDataInt("type_text"))
            {
                static_cast<TextField*>(GetResource())->SetTypeTextField((ui::TYPE_TEXT_FIELD)xml_value->GeneralValue->GetDataInt("type_text"));
            }
        }
        
        void TextFieldHandleWidget::SetSize(cocos2d::Vec2 s)
        {
            cocos2d::Vec2 p_Size(1, 1);
            
            if (HaveAligneRes())
            {
                SetSize(Vec2(
                             p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x,
                             p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
                return;
            }
            
            float p_size_font = (static_cast<TextField*>(p_Resource))->getFontSize();
            
            float add_ = 1;
            if (GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0)
            {
                add_ *= (p_min_size_scale / GetGameSize().width);
            }
            
            p_Size.x = (s.x * add_) / p_size_font;
            p_Size.y = (s.y * add_) / p_size_font;
            
            if (p_size_use_scale)
            {
                
                
                GetResource()->setScaleX(p_Size.x);
                GetResource()->setScaleY(p_Size.y);
            }
            else
            {
                int size_target = (int)(s.x * add_);
                
                if (GetGameSize().width <= p_min_size_scale &&
                    p_min_size_scale > 0)
                {
                    size_target = static_cast<TextField*>(GetResource())->getFontSize();
                }
                
                static_cast<TextField*>(GetResource())->setFontSize(size_target);
            }
            
            
        }
        
        void TextFieldHandleWidget::InitTextureTextField(xml::UILayerWidgetDec * xml_value)
        {
            p_current_text = xml_value->GeneralValue->GetDataChar("source");
            p_default_place_holder = LANG_STR(p_current_text);
            
            Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");
            
            Vec2 size_origin = Vec2(origin_ScaleRatio.x * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x),
                                    origin_ScaleRatio.y * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x));
            
            int font_idx = xml_value->GeneralValue->GetDataInt("font_index");
            
            Label * label = TextureMgr->GetFontByIdx(font_idx);
            float size_text = XMLMgr->GetFontDecByIdx(font_idx)->d_size;
            
            TextField* text = TextField::create(LANG_STR(p_current_text).GetString(), label->getTTFConfig().fontFilePath.c_str(), size_text);
            
            text->setPlaceHolderColor(xml_value->GeneralValue->GetDataColor("tint_place_holder"));
            text->setPasswordEnabled(xml_value->GeneralValue->GetDataInt("pass_input") == 1);
            
            p_size_use_scale = (xml_value->GeneralValue->GetDataInt("size_use_scale") == 1);
            
            Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
            text->setAnchorPoint(anchor_point);
            text->setTextColor(xml_value->GeneralValue->GetDataColor("tint"));
            
            if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
            {
                text->setMaxLengthEnabled(true);
                text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
            }
            
            
            text->setCursorEnabled(true);
            text->addEventListener(CC_CALLBACK_2(TextFieldHandleWidget::OnHappenEvent, this));
            
            p_Resource = text;
        }
        
        RKString TextFieldHandleWidget::GetText()
        {
            return static_cast<TextField*>(p_Resource)->getString();
        }
        
        void TextFieldHandleWidget::SetText(RKString text)
        {
            p_current_text = LANG_STR(text);
            static_cast<TextField*>(p_Resource)->setString(LANG_STR(text).GetString());
        }
        
        void TextFieldHandleWidget::SetColor(Color3B p)
        {
            static_cast<TextField*>(p_Resource)->setTextColor(Color4B(p.r, p.g, p.b, 255));
        }
        
        
        void TextFieldHandleWidget::ReloadText()
        {
            WidgetEntity::ReloadText();
            static_cast<TextField*>(p_Resource)->setString(LANG_STR(p_current_text).GetString());
        }
        
        void TextFieldHandleWidget::SetInteractive(bool value)
        {
            if (p_disable)
                return;
            WidgetEntity::SetInteractive(value);
            static_cast<TextField*>(p_Resource)->setTouchEnabled(value);
        }
        
        void TextFieldHandleWidget::ClearText()
        {
            SetText("" );
            static_cast<TextField*>(p_Resource)->SetStoreText("");
            static_cast<TextField*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
            static_cast<TextField*>(p_Resource)->didNotSelectSelf();
        }
        
        void TextFieldHandleWidget::SetTextArea(Vec2 size)
        {
            if (size != Vec2(0, 0))
            {
                static_cast<TextField*>(GetResource())->SetTextArea(size);
            }
        }
        
        void TextFieldHandleWidget::SetTouchZone(Vec2 size)
        {
            if (size != Vec2(0, 0))
            {
                static_cast<TextField*>(GetResource())->setTouchAreaEnabled(true);
                static_cast<TextField*>(GetResource())->setTouchSize(cocos2d::Size(size.x, size.y));
            }
        }
        
        void TextFieldHandleWidget::OnHappenEvent(Ref * ref , ui::TextField::EventType event_type)
        {
            if (_event_callback)
            {
                if(event_type == ui::TextField::EventType::ATTACH_WITH_IME)
                {
                    _event_callback(this, EventType_TextField::ON_ATTACH_WITH_IME);
                }
                else if (event_type == ui::TextField::EventType::DETACH_WITH_IME)
                {
                    _event_callback(this, EventType_TextField::ON_DETACH_WITH_IME);
                }
                else if (event_type == ui::TextField::EventType::INSERT_TEXT)
                {
                    _event_callback(this, EventType_TextField::ON_INSERT_TEXT);
                }
                else if (event_type == ui::TextField::EventType::DELETE_BACKWARD)
                {
                    _event_callback(this, EventType_TextField::ON_DELETE_TEXT);
                }
            }
        }

        
        //////////////////////
        
        /////////////////////////////////////////////////////////////
        EditBoxWidget::EditBoxWidget()
        {
            p_typeUI = UI_TYPE::UI_TEXT_FIELD;
            
        }
        EditBoxWidget::~EditBoxWidget()
        {
            
        }
        
        void EditBoxWidget::CopyValue(WidgetEntity * value)
        {
            auto val = static_cast<EditBoxWidget*>(value);
            TextFieldWidget::CopyValue(value);
            _event_callback = val->_event_callback;
        }
        
        WidgetEntity * EditBoxWidget::Clone()
        {
            EditBoxWidget * p_clone = new EditBoxWidget();
            p_clone->CopyValue(this);
            //remove child remain
            p_clone->p_Resource->removeAllChildrenWithCleanup(true);
            //copy child
            CloneChild(p_clone);
            CloneThis(p_clone);
            
            return p_clone;
        }
        
        /////////////////////////////////////////////////////////////
        void EditBoxWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
        {
            InitTexture(xml_value);
            WidgetEntity::InitParam(name, xml_value);
        }
        
        void EditBoxWidget::SetSize(cocos2d::Vec2 s)
        {
            
            if (HaveAligneRes())
            {
                SetSize(Vec2(
                             p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x,
                             p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
                return;
            }
            
            
            float add_ = 1;
            if (GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0)
            {
                add_ *= (p_min_size_scale / GetGameSize().width);
            }
            
            static_cast<EditBox*>(GetResource())->setFontSize((int)(s.x * add_));
            static_cast<EditBox*>(GetResource())->setPlaceholderFontSize((int)(s.x * add_));
            
            
        }
        
        void EditBoxWidget::InitTextureTextField(xml::UILayerWidgetDec * xml_value)
        {
            p_current_text = xml_value->GeneralValue->GetDataChar("source");
            p_default_place_holder = LANG_STR(p_current_text);
            
            auto panel = xml_value->GeneralValue->GetDataChar("panel");
            
            if(RKString(panel) == "")
            {
                auto xml_parent = xml_value->p_parent_layer;
                panel = xml_parent->GeneralValue->GetDataChar("source");
                if(RKString(panel) == "")
                {
                    PASSERT2(false , "wrong design");
                }
            }
            
            bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);
            
            Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");
            float delta_scale = (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x);
            Vec2 size_origin = Vec2(origin_ScaleRatio.x * delta_scale, origin_ScaleRatio.y * delta_scale);
            
            Vec2 touch_zone = Vec2(0, 0);
                
            if (xml_value->GeneralValue->HaveDataVector2("text_area"))
            {
                touch_zone = xml_value->GeneralValue->GetDataVector2("text_area");
                touch_zone.x =touch_zone.x * delta_scale;
                touch_zone.y =touch_zone.y * delta_scale;
            }
            else
            {
                if (xml_value->GeneralValue->HaveDataVector2("touch_zone"))
                {
                    touch_zone = xml_value->GeneralValue->GetDataVector2("touch_zone");
                    touch_zone.x =touch_zone.x * delta_scale;
                    touch_zone.y =touch_zone.y * delta_scale;
                }
                else
                {
                    PASSERT2(false ,"edit box must have this value");
                }
            }
            
            EditBox* text = EditBox::create(cocos2d::Size(touch_zone.x, touch_zone.y), panel,
                                            IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL
                                            );
            text->setPlaceHolder(p_default_place_holder.GetString());
            text->setPlaceholderFontColor(xml_value->GeneralValue->GetDataColor("tint_place_holder"));
            text->setPlaceholderFontSize(size_origin.x);
            text->setContentSize(cocos2d::Size(touch_zone.x, touch_zone.y));
            if (xml_value->GeneralValue->GetDataInt("pass_input") == 1)
            {
                text->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
            }
            else
            {
                text->setInputFlag(ui::EditBox::InputFlag::INITIAL_CAPS_WORD);
            }
            
            if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
            {
                text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
            }
            
            Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
            text->setAnchorPoint(anchor_point);
            text->setFontColor(xml_value->GeneralValue->GetDataColor("tint"));
            
            if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
            {
                text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
            }
            
            if (xml_value->GeneralValue->HaveDataInt("type_text"))
            {
                int current_type_text = xml_value->GeneralValue->GetDataInt("type_text");
                if (current_type_text == ui::TYPE_TEXT_FIELD::NUMBER_ONLY)
                {
                    text->setInputMode(ui::EditBox::InputMode::PHONE_NUMBER);
                }
                else if (current_type_text == ui::TYPE_TEXT_FIELD::TEXT_AND_NUMBER)
                {
                    text->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
                }
                else if (current_type_text == ui::TYPE_TEXT_FIELD::TEXT_ONLY)
                {
                    text->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
                }
            }
            auto bg_sprite = text->getBackgroundSprite();
            bg_sprite->setVisible(false);
            
            text->setDelegate(this);
            text->setReturnType(EditBox::KeyboardReturnType::DONE);
            
            p_Resource = text;
        }
        
        RKString EditBoxWidget::GetText()
        {
            return static_cast<EditBox*>(p_Resource)->getText();
        }
        
        void EditBoxWidget::SetText(RKString text)
        {
            p_current_text = text;
            static_cast<EditBox*>(p_Resource)->setText(LANG_STR(text).GetString());
        }
        
        void EditBoxWidget::SetColor(Color3B p)
        {
            static_cast<EditBox*>(p_Resource)->setFontColor(Color4B(p.r, p.g, p.b, 255));
        }
        
        void EditBoxWidget::ReloadText()
        {
            WidgetEntity::ReloadText();
            static_cast<EditBox*>(p_Resource)->setText(LANG_STR(p_current_text).GetString());
        }
        
        
        void EditBoxWidget::editBoxReturn(EditBox* editBox)
        {
            if (_event_callback)
            {
                _event_callback(this, EventType_TextField::ON_DETACH_WITH_IME);
            }
        }
        
        void EditBoxWidget::editBoxEditingDidBegin(EditBox* editBox)
        {
            if (_event_callback)
            {
                _event_callback(this, EventType_TextField::ON_ATTACH_WITH_IME);
            }
        }
        void EditBoxWidget::editBoxEditingDidEnd(EditBox* editBox)
        {
            if (_event_callback)
            {
                _event_callback(this, EventType_TextField::ON_DETACH_WITH_IME);
            }
        }
        void EditBoxWidget::editBoxTextChanged(EditBox* editBox, const std::string& text)
        {
            if(p_current_text.Length() < text.length())
            {
                if (_event_callback)
                {
                    _event_callback(this, EventType_TextField::ON_INSERT_TEXT);
                }
            }
            else if (p_current_text.Length() > text.length())
            {
                if (_event_callback)
                {
                    _event_callback(this, EventType_TextField::ON_DELETE_TEXT);
                }
            }
            
            
            p_current_text = text;
            
        }
        
        void EditBoxWidget::SetInteractive(bool value)
        {
            if (p_disable)
                return;
            WidgetEntity::SetInteractive(value);
            static_cast<EditBox*>(p_Resource)->setTouchEnabled(value);
        }
	}
}