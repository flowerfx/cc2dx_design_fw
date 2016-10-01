#include "wSlider.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		SliderWidget::SliderWidget()
		{
			p_typeUI = UI_TYPE::UI_SLIDER;
			p_IsInteractive = true;
		}
		SliderWidget::~SliderWidget()
		{
		}

		void SliderWidget::CopyValue(WidgetEntity * value)
		{
			WidgetEntity::CopyValue(value);
		}

		WidgetEntity * SliderWidget::Clone()
		{
			SliderWidget * p_clone = new SliderWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;

		}

		/////////////////////////////////////////////////////////////
		void SliderWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void SliderWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);
			bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);

			RKString bar_texture = xml_value->GeneralValue->GetDataChar("source");

			RKString normal_ball = xml_value->GeneralValue->GetDataChar("normal_ball");
			RKString press_ball = xml_value->GeneralValue->GetDataChar("press_ball");
			RKString disable_ball = xml_value->GeneralValue->GetDataChar("disable_ball");

			RKString bar_progress = xml_value->GeneralValue->GetDataChar("bar_progress");


			Slider* _widget = Slider::create();
			_widget->loadBarTexture(bar_texture.GetString(), IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);

			_widget->loadSlidBallTextures(normal_ball.GetString(), press_ball.GetString(), disable_ball.GetString(),
				IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);

			if (xml_value->GeneralValue->HaveDataVector2("size_ball"))
			{
				Vec2 size_ball = xml_value->GeneralValue->GetDataVector2("size_ball");
				size_ball.x = size_ball.x * GetGameSize().width / menu_point_to->GetDesignSize().x;
				size_ball.y = size_ball.y * GetGameSize().width / menu_point_to->GetDesignSize().x;
				Vec2 origin_size_ball = xml_value->GeneralValue->GetDataVector2("origin_size_ball");

				Vec2 scale_ball = Vec2(size_ball.x / origin_size_ball.x, size_ball.y / origin_size_ball.y);

				if (GetGameSize().width < p_min_size_scale && p_min_size_scale > 0)
				{
					scale_ball = Vec2(1, 1);
				}

				_widget->setScaleSlideBall(scale_ball);
			}

			if (xml_value->GeneralValue->HaveDatafloat("rotate_ball"))
			{
				float rotate = xml_value->GeneralValue->GetDatafloat("rotate_ball");
				_widget->setRotateSlideBall(rotate);
			}

			_widget->loadProgressBarTexture(bar_progress.GetString(), IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);

			_widget->setMaxPercent(100);

			_widget->setScale9Enabled(IsScale9Enable);
			if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
			{
				if (bar_texture != "")
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(bar_texture.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}


					_widget->setCapInsetsBarRenderer(cap_insert);
				}
				if (bar_progress != "")
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(bar_progress.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					_widget->setCapInsetProgressBarRenderer(cap_insert);
				}

			}
			p_isScale9Enable = IsScale9Enable;

			if (xml_value->GeneralValue->HaveDataInt("number_part"))
			{
				int p_number_part = xml_value->GeneralValue->GetDataInt("number_part");
				_widget->SetNumberPart(p_number_part);
			}

			p_Resource = _widget;
		}

		void SliderWidget::SetSize(Vec2 s)
		{
			s.x = (int)(s.x + 0.5f);
			s.y = (int)(s.y + 0.5f);
			p_Resource->setContentSize(cocos2d::Size(s.x, s.y));
		}

		Vec2 SliderWidget::GetSize()
		{
			return Vec2(p_Resource->getContentSize().width, p_Resource->getContentSize().height);
		}

		void SliderWidget::SetInteractive(bool value)
		{
			WidgetEntity::SetInteractive(value);
			static_cast<Slider*>(p_Resource)->setTouchEnabled(value);
		}

		float SliderWidget::GetPercent()
		{
			return static_cast<Slider*>(p_Resource)->getPercent();
		}

		void SliderWidget::SetPercent(float percent)
		{
			static_cast<Slider*>(p_Resource)->setPercent(percent);
		}

		int SliderWidget::GetPart()
		{
			return static_cast<Slider*>(p_Resource)->GetNumberPart();
		}

		void SliderWidget::SetPart(int value)
		{
			static_cast<Slider*>(p_Resource)->SetNumberPart(value);
		}

	}
}