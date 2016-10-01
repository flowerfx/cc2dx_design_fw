#include "wPanel.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		PanelWidget::PanelWidget()
		{
			p_typeUI = UI_TYPE::UI_PANEL;

		}
		PanelWidget::~PanelWidget()
		{

		}
		
		void PanelWidget::CopyValue(WidgetEntity * value)
		{		
			WidgetEntity::CopyValue(value);
		}

		WidgetEntity * PanelWidget::Clone()
		{
			PanelWidget * p_clone = new PanelWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void PanelWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void PanelWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			auto source_texture = xml_value->GeneralValue->GetDataChar("source");
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			ImageView* panel = nullptr;
			if (IsUseAtlas)
			{
				panel = ImageView::create(source_texture,Widget::TextureResType::PLIST);
			}
			else
			{
				panel = ImageView::create(source_texture);
			}
			bool IsScale9Enable = (xml_value->GeneralValue->GetDataInt("scale_9") == 1);
            cocos2d::Rect cap_insert = cocos2d::Rect(0, 0, 0, 0);
			if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
			{
				cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(source_texture)->GetCapInsert();

				RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
				if (cap != "")
				{
					cap_insert = XMLMgr->ConvertToRect(cap);
				}

			}
			p_isScale9Enable = IsScale9Enable;
			if (p_isScale9Enable)
			{
				panel->setScale9Enabled(IsScale9Enable);
				panel->setCapInsets(cap_insert);
			}

			p_Resource = panel;
		}
		

	}
}