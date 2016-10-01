#include "wButton.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ButtonWidget::ButtonWidget()
		{
			p_typeUI = UI_TYPE::UI_BUTTON;
			p_IsInteractive = true;
			p_is_only_text = false;
			p_color_text = Color3B::WHITE;
			p_opacity_text = 255;
			p_title_text = "";
		}
		ButtonWidget::~ButtonWidget()
		{
			p_title_text = "";
		}

		void ButtonWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<ButtonWidget*>(value);
			
			WidgetEntity::CopyValue(value);
			p_is_only_text = val->p_is_only_text;
			p_color_text = val->p_color_text;
			p_opacity_text = val->p_opacity_text;
			p_title_text = val->p_title_text;
		}

		WidgetEntity * ButtonWidget::Clone()
		{
			ButtonWidget * p_clone = new ButtonWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;

		}

		/////////////////////////////////////////////////////////////
		void ButtonWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void ButtonWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			RKString normal = xml_value->GeneralValue->GetDataChar("source");
			RKString press = xml_value->GeneralValue->GetDataChar("press");

			RKString disable = xml_value->GeneralValue->GetDataChar("disable");
			p_title_text = xml_value->GeneralValue->GetDataChar("title");
			float scale = xml_value->GeneralValue->GetDatafloat("scale_press");

			bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);

			ui::Button * btn_widget = nullptr;

			if (normal != "" || press != "" || disable != "")
			{
				btn_widget = Button::create(normal.GetString(), press.GetString(), disable.GetString(),
					IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);

			}
			else
			{
				btn_widget = Button::create();
				if (p_title_text != "")
				{
					p_is_only_text = true;
				}
			}

			if (p_title_text != "")
			{
				btn_widget->setTitleText(LANG_STR(p_title_text).GetString());
				Color4B title_color = xml_value->GeneralValue->GetDataColor("title_color");
				int		title_alige = xml_value->GeneralValue->GetDataInt("title_alige");
				int		font_idx = xml_value->GeneralValue->GetDataInt("title_font_idx");
				

				Label * label = TextureMgr->GetFontByIdx(font_idx);

				Vec2 title_size = xml_value->GeneralValue->GetDataVector2("title_size");

				Vec2 size_origin = Vec2(
					title_size.x * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x),
					title_size.y * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x));

				int		min_size_scale = xml_value->GeneralValue->GetDataInt("min_size_scale");
				if (GetGameSize().width <= min_size_scale)
				{
					size_origin.x *= (min_size_scale / GetGameSize().width);
				}
				p_color_text = Color3B(title_color.r, title_color.g, title_color.b);
				btn_widget->setTitleFontSize((int)(size_origin.x));
				btn_widget->setTitleColor(p_color_text);
				btn_widget->setTitleAlignment((cocos2d::TextHAlignment)title_alige);
				btn_widget->setTitleFontName(label->getTTFConfig().fontFilePath.c_str());
			}

			if (scale != 0)
			{
				btn_widget->setPressedActionEnabled(true);
				btn_widget->setZoomScale(scale);
			}

			btn_widget->setScale9Enabled(IsScale9Enable);
			if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
			{

				if (normal != "")
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(normal.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}


					btn_widget->setCapInsetsNormalRenderer(cap_insert);
				}
				if (press != "")
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(press.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					btn_widget->setCapInsetsPressedRenderer(cap_insert);
				}
				if (disable != "")
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(disable.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					btn_widget->setCapInsetsDisabledRenderer(cap_insert);
				}
			}
			p_isScale9Enable = IsScale9Enable;

			p_Resource = btn_widget;
		}

		void ButtonWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<Button*>(p_Resource)->setTouchEnabled(value);
		}

		void ButtonWidget::ResizeAndPosChildWidget(WidgetEntity * child)
		{
			WidgetEntity::ResizeAndPosChildWidget(child);
			return;
			//set new size for widget child
			float scale_parent_widget_x = this->GetSize().x / this->GetResource()->getContentSize().width;
			float scale_parent_widget_y = this->GetSize().y / this->GetResource()->getContentSize().height;

			Vec2 design_size = child->GetMenuParent()->GetDesignSize();

			Vec2 first_pos = Vec2((child->GetPosition().x / GetGameSize().width) * design_size.x, (child->GetPosition().y / GetGameSize().height) * design_size.y);
			//child->SetPosition(first_pos);

			Vec2 first_size = Vec2(this->GetSize().x * design_size.x / GetGameSize().width, this->GetSize().y * design_size.x / GetGameSize().width);


			Vec2 pos_ratio = Vec2(first_pos.x / first_size.x, first_pos.y / first_size.y);
			//if (scale_parent_widget_x != 1.0f)
			{
				child->GetResource()->setScaleX((1 / scale_parent_widget_x) * child->GetResource()->getScaleX());
				child->GetResource()->setScaleY((1 / scale_parent_widget_x) * child->GetResource()->getScaleY());

				child->SetPosition(Vec2((1 / scale_parent_widget_x) * this->GetSize().x *pos_ratio.x,
					(1 / scale_parent_widget_y) * this->GetSize().y *pos_ratio.y));
			}
		}

		void ButtonWidget::SetColor(Color3B p)
		{
			ActionProcess::SetColor(p);
			auto title_renderer = static_cast<ui::Button *>(p_Resource)->getTitleRenderer();
			if (title_renderer && !p_Resource->isCascadeColorEnabled())
			{
				title_renderer->updateDisplayedColor(p_color_text);
			}	
		}

		void ButtonWidget::SetSize(Vec2 s)
		{
			if (!p_is_only_text)
			{
				WidgetEntity::SetSize(s);
			}
		}

		void ButtonWidget::ReloadText()
		{
			WidgetEntity::ReloadText();
			if (static_cast<ui::Button *>(p_Resource)->getTitleText() != "")
			{
				static_cast<ui::Button *>(p_Resource)->setTitleText(LANG_STR(p_title_text).GetString());
			}
		}

		void ButtonWidget::SetTitle(RKString title)
		{
			p_title_text = title;
			static_cast<Button*>(GetResource())->setTitleText(LANG_STR(title).GetString());
		}

		void ButtonWidget::SetCallBack(const ui::Widget::ccWidgetTouchCallback& callback)
		{
			if (callback)
			{
				static_cast<Button*>(this->GetResource())->addTouchEventListener(callback);
			}
		}
	}
}