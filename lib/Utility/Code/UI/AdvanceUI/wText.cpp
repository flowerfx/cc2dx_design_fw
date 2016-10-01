#include "wText.h"
#include "../../FileManager.h"
namespace Utility
{
	namespace UI_Widget
	{

		NumberToEffect::NumberToEffect() 
		{
			_function_convert = nullptr;
		}

		NumberToEffect::~NumberToEffect() 
		{
			if (_function_convert)
			{
				_function_convert = nullptr;
			}
		}

		NumberToEffect* NumberToEffect::create(float duration, long long numberTo)
		{
			NumberToEffect *effect = new (std::nothrow) NumberToEffect();
			effect->initWithDuration(duration, numberTo);
			effect->autorelease();
			return effect;
		}

		NumberToEffect* NumberToEffect::clone() const
		{
			// no copy constructor
			auto a = new (std::nothrow) NumberToEffect();
			a->SetStartNumber(_from);
			a->initWithDuration(_duration, _to);
			a->autorelease();
			return a;
		}
		NumberToEffect* NumberToEffect::reverse(void) const
		{
			auto a = new (std::nothrow) NumberToEffect();
			a->SetStartNumber(_to);
			a->initWithDuration(_duration, _from);
			a->autorelease();
			return a;
		}
		/**
		* @param time In seconds.
		*/
		void NumberToEffect::update(float time)
		{
			long long delta = _to - _from;
			float deltatime = delta / _duration;

			_current_number = _from + delta * time;
			if (_function_convert)
			{
				if(_target->getDescription() == "Label")
					static_cast<Text*>(_target)->setString(_function_convert(_current_number));
				else 
					static_cast<TextBMFont*>(_target)->setString(_function_convert(_current_number));
			}
			else
			{
				if(_target->getDescription() == "Label")
					static_cast<Text*>(_target)->setString(std::to_string(_current_number));
				else
					static_cast<TextBMFont*>(_target)->setString(std::to_string(_current_number));
			}
		}

		/** initializes the action with duration and color */
		bool NumberToEffect::initWithDuration(float duration, long long numberTo)
		{
			if (ActionInterval::initWithDuration(duration))
			{
				_to = numberTo;
				return true;
			}

			return false;
		}

		void NumberToEffect::SetFunctionConvert(const std::function<std::string(long long)> & func)
		{
			_function_convert = func;
		}

		void NumberToEffect::SetStartNumber(long long number)
		{
			_from = _current_number = number;
		}
		/////////////////////////////////////////////////////////////
		TextWidget::TextWidget()
		{
			p_typeUI = UI_TYPE::UI_TEXT;
			p_border_width = 0;
			p_border_color = Color4B::WHITE;
			p_current_text = "";
			p_size_use_scale = false;

			p_use_bmp_text = false;
			p_size_font_bmp = 1;

			p_size_area = Vec2(0, 0);

			p_use_origin_size = 0;

			p_isLinebreak = false;

		}
		TextWidget::~TextWidget()
		{
			p_current_text = "";
		}
		void TextWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TextWidget*>(value);
			WidgetEntity::CopyValue(value);
			//
			p_border_width = val->p_border_width;
			p_border_color = val->p_border_color;
			p_size_use_scale = val->p_size_use_scale;

			p_use_bmp_text = val->p_use_bmp_text;
			p_size_font_bmp = val->p_size_font_bmp;
			p_use_origin_size = val->p_use_origin_size;
			p_size_area = val->p_size_area;

			p_current_text = val->p_current_text;
			p_isLinebreak = val->p_isLinebreak;
			if (p_border_width > 0 && !p_use_bmp_text)
			{
				static_cast<Text*>(p_Resource)->enableOutline(p_border_color, p_border_width);
			}

			if (p_size_area != Vec2(0, 0))
			{
				if (p_use_bmp_text)
				{
					static_cast<TextBMFont*>(p_Resource)->ignoreContentAdaptWithSize(true);
					static_cast<TextBMFont*>(p_Resource)->setTextAreaSize(cocos2d::Size(p_size_area.x, p_size_area.y));
					static_cast<Label*>(static_cast<TextBMFont*>(p_Resource)->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
				}
				else
				{
					bool ignore_content_size = false;
					if (p_size_area.x == 0 || p_size_area.y == 0)
					{
						ignore_content_size = true;
					}
					static_cast<Text*>(p_Resource)->ignoreContentAdaptWithSize(ignore_content_size);
					static_cast<Text*>(p_Resource)->setTextAreaSize(cocos2d::Size(p_size_area.x, p_size_area.y));
					static_cast<Label*>(static_cast<Text*>(p_Resource)->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
				}
			}
		}
		WidgetEntity * TextWidget::Clone()
		{
			TextWidget * p_clone = new TextWidget();
			p_clone->CopyValue(this);
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void TextWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);

		}

		void TextWidget::SetSize(cocos2d::Vec2 s)
		{
			cocos2d::Vec2 p_Size(1, 1);
			if (HaveAligneRes())
			{
				SetSize(Vec2(
					p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x, 
					p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
				return;
			}

			Vec2 p_size_font = Vec2(0, 0);
				if (p_use_bmp_text)
				{
					p_size_font.x = p_size_font_bmp;
					p_size_font.y = p_size_font_bmp;
				}
				else
				{
					p_size_font.x = (static_cast<Text*>(p_Resource))->getFontSize();
					p_size_font.y = (static_cast<Text*>(p_Resource))->getFontSize();
				}

			float add_ = 1;

			if (GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0)
			{
				add_ *= (p_min_size_scale / GetGameSize().width);
			}

			p_Size.x = (s.x * add_ )/ p_size_font.x;
			p_Size.y = (s.y * add_ )/ p_size_font.y;

			if (p_size_use_scale)
			{
				GetResource()->setScaleX(p_Size.x);
				GetResource()->setScaleY(p_Size.y);
			}
			else
			{
				if (p_use_bmp_text)
				{
					if(GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0 )
					{
						if (p_use_origin_size == 0 || p_Size.x > 1)
						{
							p_Size.x = 1;
						}
						//auto label_render = static_cast<TextBMFont*>(GetResource())->getVirtualRenderer();
						//static_cast<Label*>(label_render)->scaleFontSizeDown(p_Size.x * p_size_font_bmp);
						static_cast<TextBMFont*>(GetResource())->setScale(p_Size.x);
					}
					else
					{
						static_cast<TextBMFont*>(GetResource())->setScale(p_Size.x);
					}
				}
				else
				{
					int size_target = (int)(s.x * add_);

					if (GetGameSize().width <= p_min_size_scale && 
						p_min_size_scale > 0)
					{
						size_target = static_cast<Text*>(GetResource())->getFontSize();
					}

					static_cast<Text*>(GetResource())->setFontSize(size_target);


				}
			}


		}

		void TextWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			p_current_text = xml_value->GeneralValue->GetDataChar("source");
			int font_idx = xml_value->GeneralValue->GetDataInt("font_idx");
			//
			if (xml_value->GeneralValue->HaveDataInt("use_origin_size"))
			{
				p_use_origin_size = xml_value->GeneralValue->GetDataInt("use_origin_size");
			}
			//
			p_use_bmp_text = (xml_value->GeneralValue->GetDataInt("bitmap_font") == 1);

			if (!p_use_bmp_text)
			{
			
				Label * label = TextureMgr->GetFontByIdx(font_idx);
				float size_text = XMLMgr->GetFontDecByIdx(font_idx)->d_size;

				Text* text = Text::create(LANG_STR(p_current_text).GetString(), label->getTTFConfig().fontFilePath.c_str(), size_text);

				text->setTextColor(xml_value->GeneralValue->GetDataColor("tint"));
				p_size_use_scale = (xml_value->GeneralValue->GetDataInt("size_use_scale") == 1);

				Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
				text->setAnchorPoint(anchor_point);

				if (xml_value->GeneralValue->HaveDataInt("aligneH"))
				{
					text->setTextHorizontalAlignment((TextHAlignment)xml_value->GeneralValue->GetDataInt("aligneH"));
				}
				if (xml_value->GeneralValue->HaveDataInt("aligneV"))
				{
					text->setTextVerticalAlignment((TextVAlignment)xml_value->GeneralValue->GetDataInt("aligneV"));
				}

				int border_width = xml_value->GeneralValue->GetDataInt("border_width");
				if (border_width > 0)
				{
					Vec2 design_size = xml_value->p_parent_layer->p_menu_widget->DesignSize;
					border_width = (int)(((float)border_width) * GetGameSize().width / design_size.x);
					text->enableOutline(xml_value->GeneralValue->GetDataColor("border_corlor"), border_width);

					p_border_width = border_width;
					p_border_color = xml_value->GeneralValue->GetDataColor("border_corlor");
				}

				if (xml_value->GeneralValue->HaveDataVector2("AreaSize"))
				{
					Vec2 size_area = xml_value->GeneralValue->GetDataVector2("AreaSize");
					p_isLinebreak = xml_value->GeneralValue->GetDataInt("line_break") == 1;
					p_size_area.x = (size_area.x * GetGameSize().width) / xml_value->p_menu_widget->DesignSize.x;
					p_size_area.y = (size_area.y * GetGameSize().height) / xml_value->p_menu_widget->DesignSize.y;
					bool ignore_content_size = false;
					if (p_size_area.x == 0 || p_size_area.y == 0)
					{
						ignore_content_size = true;
					}
					text->ignoreContentAdaptWithSize(ignore_content_size);
					text->setTextAreaSize(cocos2d::Size(p_size_area.x, p_size_area.y));
					static_cast<Label*>(text->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
				}

				p_Resource = text;
			}
			else
			{
			
				auto text_bmp = TextureMgr->GetFontBmpByIdx(font_idx);
				p_size_font_bmp = text_bmp->size_font;
			
				TextBMFont * text = TextBMFont::create(LANG_STR(p_current_text).GetString(), File::ResolveNamePath(text_bmp->path_fnt).GetString());

				//text->setColor(xml_value->GeneralValue->GetDataColor("tint"));
				p_size_use_scale = (xml_value->GeneralValue->GetDataInt("size_use_scale") == 1);

				Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
				text->setAnchorPoint(anchor_point);

				if (xml_value->GeneralValue->HaveDataInt("aligneH"))
				{
					text->setTextHorizontalAlignment((TextHAlignment)xml_value->GeneralValue->GetDataInt("aligneH"));
				}
				if (xml_value->GeneralValue->HaveDataInt("aligneV"))
				{
					text->setTextVerticalAlignment((TextVAlignment)xml_value->GeneralValue->GetDataInt("aligneV"));
				}

				if (xml_value->GeneralValue->HaveDataVector2("AreaSize"))
				{
					Vec2 size_area = xml_value->GeneralValue->GetDataVector2("AreaSize");
					p_isLinebreak = xml_value->GeneralValue->GetDataInt("line_break") == 1;
					p_size_area.x = (size_area.x * GetGameSize().width) / xml_value->p_menu_widget->DesignSize.x;
					p_size_area.y = (size_area.y * GetGameSize().height) / xml_value->p_menu_widget->DesignSize.y;

					text->ignoreContentAdaptWithSize(true);
					text->setTextAreaSize(cocos2d::Size(p_size_area.x, p_size_area.y));
					static_cast<Label*>(text->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
				}

				p_Resource = text;
			}
		}

		RKString TextWidget::GetText()
		{
			if (p_use_bmp_text)
			{
				return static_cast<TextBMFont*>(p_Resource)->getString();
			}
			else
			{
				return static_cast<Text*>(p_Resource)->getString();
			}
		}

		void TextWidget::SetText(RKString text, bool force)
		{
			if (force)
			{
				p_current_text = text;
			}
			else
			{
				p_current_text = LANG_STR(text);
			}
			SetText();
		}

		void TextWidget::SetText()
		{
			if (p_use_bmp_text)
			{
				static_cast<TextBMFont*>(p_Resource)->setString(p_current_text.GetString());
			}
			else
			{
				static_cast<Text*>(p_Resource)->setString(p_current_text.GetString());
			}
		}

		void TextWidget::SetColor(Color3B p)
		{
			if (p_use_bmp_text)
			{
				static_cast<TextBMFont*>(p_Resource)->setColor(Color3B(p.r, p.g, p.b));
			}
			else
			{
				static_cast<Text*>(p_Resource)->setTextColor(Color4B(p.r, p.g, p.b, 255));
			}
		}

		void TextWidget::ReloadText()
		{
			WidgetEntity::ReloadText();
			if (p_use_bmp_text)
			{
				static_cast<TextBMFont*>(p_Resource)->setString(LANG_STR(p_current_text).GetString());
			}
			else
			{
				static_cast<Text*>(p_Resource)->setString(LANG_STR(p_current_text).GetString());
			}
		}

		void TextWidget::TextRunEffect(
			long long to_number, 
			long long current_number, 
			float time, 
			std::function<std::string(long long)> func , 
			CallFunc * call_back)
		{
			//if (p_use_bmp_text)
			//	return;

			NumberToEffect * effect = NumberToEffect::create(time, to_number);
			effect->SetStartNumber(current_number);
			effect->SetFunctionConvert(func);
			if (call_back)
			{
				auto squence = Sequence::createWithTwoActions(effect, call_back);
				this->GetResource()->runAction(squence);
			}
			else
			{
				this->GetResource()->runAction(effect);
			}
		}

		Vec2 TextWidget::GetTextAreaSize()
		{
			if (p_use_bmp_text)
			{
				//return Vec2(-1, -1);
				auto size = static_cast<TextBMFont*>(GetResource())->getTextAreaSize();
				return Vec2(size.width, size.height);
			}
			else
			{
				auto size = static_cast<Text*>(GetResource())->getTextAreaSize();
				return Vec2(size.width, size.height);
			}
		}
		void TextWidget::SetTextAreaSize(Vec2 size)
		{
			if (p_use_bmp_text)
			{
				static_cast<TextBMFont*>(p_Resource)->setTextAreaSize(cocos2d::Size(size.x, size.y));
			}
			else
			{
				static_cast<Text*>(p_Resource)->setTextAreaSize(cocos2d::Size(size.x, size.y));
			}
		}

		void TextWidget::SetLineBreakWithoutSpace(bool value)
		{
			p_isLinebreak = value;
			if (p_use_bmp_text)
			{
				static_cast<Label*>(static_cast<TextBMFont*>(p_Resource)->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
			}
			else
			{
				static_cast<Label*>(static_cast<Text*>(p_Resource)->getVirtualRenderer())->setLineBreakWithoutSpace(p_isLinebreak);
			}
		}

		int	 TextWidget::GetNumberOfLine()
		{
			int value = 1;
			if (p_use_bmp_text)
			{
				value = static_cast<Label*>(static_cast<TextBMFont*>(p_Resource)->getVirtualRenderer())->getStringNumLines();
			}
			else
			{
				value = static_cast<Label*>(static_cast<Text*>(p_Resource)->getVirtualRenderer())->getStringNumLines();
			}
			return value;
		}

		float TextWidget::GetFontSize()
		{
			float value = 20;

			if (p_use_bmp_text)
			{
				value = p_size_font_bmp * p_Resource->getScaleX();
			}
			else
			{
				value = static_cast<Text*>(p_Resource)->getFontSize();
			}
			return value;
		}

		Vec2  TextWidget::GetActualContainSize()
		{
			Vec2 value = Vec2(0, 0);
			if (p_size_area == Vec2(0, 0))
			{
				return GetResource()->getContentSize();
			}
			else
			{
				int num_line = GetNumberOfLine();
				if (num_line > 1)
				{
					Vec2 size_text = GetResource()->getContentSize();
					float size_y_each_line = size_text.y / num_line;
					float font_size = GetFontSize();
					value = Vec2(size_text.x, (font_size*num_line) + abs(size_y_each_line - font_size));
				}
				else
				{
					Vec2 size_text = GetResource()->getContentSize();
					float width_text = static_cast<Label*>(static_cast<Text*>(p_Resource)->getVirtualRenderer())->getLineWidthAtIdx(0);
					if (width_text > size_text.x)
					{
						width_text = size_text.x;
					}
					value = Vec2(width_text, size_text.y);
				}
			}
			return value;
		}

	}
}