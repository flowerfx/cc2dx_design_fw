#include "wEditBox.h"
namespace Utility
{
	namespace UI_Widget
	{
#ifdef USE_EDIT_BOX_SEP
		/////////////////////////////////////////////////////////////
		EditBoxWidget::EditBoxWidget()
		{
			p_typeUI = UI_TYPE::UI_EDIT_BOX;
			p_current_text = "";
			p_default_place_holder = "";

            _event_callback= nullptr;
		}
		EditBoxWidget::~EditBoxWidget()
		{
			p_current_text = "";
			p_default_place_holder = "";

            _event_callback = nullptr;
		}

		void EditBoxWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<EditBoxWidget*>(value);
			WidgetEntity::CopyValue(value);
			p_current_text = val->p_current_text;
			p_default_place_holder = val->p_default_place_holder;

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

		void EditBoxWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			p_current_text = xml_value->GeneralValue->GetDataChar("source");
			p_default_place_holder = LANG_STR(p_current_text);

			auto panel = xml_value->GeneralValue->GetDataChar("panel");
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");
            float delta_scale = (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x);
			Vec2 size_origin = Vec2(origin_ScaleRatio.x * delta_scale, origin_ScaleRatio.y * delta_scale);
         
			Vec2 touch_zone = Vec2(0, 0);
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

			EditBox* text = EditBox::create(cocos2d::Size(touch_zone.x, touch_zone.y), panel,
				IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL
			);
			text->setPlaceHolder(p_current_text.GetString());
			text->setPlaceholderFontColor(xml_value->GeneralValue->GetDataColor("tint_place_holder"));
            text->setPlaceholderFontSize(size_origin.x);
            text->setContentSize(cocos2d::Size(touch_zone.x, size_origin.y));
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

		RKString EditBoxWidget::GetDefaultPlaceHolder()
		{
			return LANG_STR(p_default_place_holder);
		}

		void EditBoxWidget::ReloadText()
		{
			WidgetEntity::ReloadText();
			static_cast<EditBox*>(p_Resource)->setText(LANG_STR(p_current_text).GetString());
		}

        void EditBoxWidget::AddEventHandler(const wEditBoxEventHandler & event)
        {
            _event_callback = event;
        }
        
		void EditBoxWidget::editBoxReturn(EditBox* editBox)
		{
            if (_event_callback)
            {
                _event_callback(this, EventType_EditBox::ON_DETACH_WITH_IME);
            }
		}
        
        void EditBoxWidget::editBoxEditingDidBegin(EditBox* editBox)
        {
            if (_event_callback)
            {
                _event_callback(this, EventType_EditBox::ON_ATTACH_WITH_IME);
            }
        }
        void EditBoxWidget::editBoxEditingDidEnd(EditBox* editBox)
        {
            if (_event_callback)
            {
                _event_callback(this, EventType_EditBox::ON_DETACH_WITH_IME);
            }
        }
        void EditBoxWidget::editBoxTextChanged(EditBox* editBox, const std::string& text)
        {
            if(p_current_text.Length() < text.length())
            {
                if (_event_callback)
                {
                    _event_callback(this, EventType_EditBox::ON_INSERT_TEXT);
                }
            }
            else if (p_current_text.Length() < text.length())
            {
                if (_event_callback)
                {
                    _event_callback(this, EventType_EditBox::ON_DELETE_TEXT);
                }
            }
            
            
            p_current_text = text;

        }
#endif
	}
}