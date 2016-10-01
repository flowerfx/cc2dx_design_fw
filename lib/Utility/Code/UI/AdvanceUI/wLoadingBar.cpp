#include "wLoadingBar.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		LoadingBarWidget::LoadingBarWidget()
		{
			p_typeUI = UI_TYPE::UI_LOADING_BAR;
			p_IsInteractive = true;
		}
		LoadingBarWidget::~LoadingBarWidget()
		{
		}

		void LoadingBarWidget::CopyValue(WidgetEntity * value)
		{
			WidgetEntity::CopyValue(value);
		}

		WidgetEntity * LoadingBarWidget::Clone()
		{
			LoadingBarWidget * p_clone = new LoadingBarWidget();
			p_clone->CopyValue(this);
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;

		}

		/////////////////////////////////////////////////////////////
		void LoadingBarWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void LoadingBarWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);
			bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);
			int direction = xml_value->GeneralValue->GetDataInt("direction") ;
			float percent = xml_value->GeneralValue->GetDatafloat("percent");
			if (percent < 1)
				percent = 1;
			RKString bar_texture = xml_value->GeneralValue->GetDataChar("source");

			LoadingBar* _widget = LoadingBar::create(bar_texture.GetString(), IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL , percent);
			_widget->setDirection(direction == 0 ? ui::LoadingBar::Direction::LEFT : ui::LoadingBar::Direction::RIGHT);
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

					_widget->setCapInsets(cap_insert);
				}
				
			}
			p_isScale9Enable = IsScale9Enable;

			p_Resource = _widget;
		}


		void LoadingBarWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<LoadingBar*>(p_Resource)->setTouchEnabled(value);
		}

		float LoadingBarWidget::GetPercent()
		{
			return static_cast<LoadingBar*>(p_Resource)->getPercent();
		}

		void LoadingBarWidget::SetPercent(float percent)
		{
			static_cast<LoadingBar*>(p_Resource)->setPercent(percent);
		}

	}
}