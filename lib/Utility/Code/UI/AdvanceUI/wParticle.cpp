#include "wParticle.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ParticleWidget::ParticleWidget()
		{
			p_typeUI = UI_TYPE::UI_PARTICLE;

		}
		ParticleWidget::~ParticleWidget()
		{

		}
		void ParticleWidget::CopyValue(WidgetEntity * value)
		{
			WidgetEntity::CopyValue(value);
		}
		WidgetEntity * ParticleWidget::Clone()
		{
			ParticleWidget * p_clone = new ParticleWidget();
			p_clone->CopyValue(this);
			//copy resource
			p_clone->_particle = ParticleSystemQuad::create(_path_plist.GetString());
			p_clone->_path_plist = _path_plist;

			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void ParticleWidget::InitParam(RKString name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void ParticleWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			auto source_texture = xml_value->GeneralValue->GetDataChar("source");
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);
			_path_plist = xml_value->GeneralValue->GetDataChar("plist");
			_particle = ParticleSystemQuad::create(_path_plist.GetString());

			if (RKString(source_texture) != "")
			{
				if (IsUseAtlas)
				{
					SpriteFrame * sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(source_texture);
					if (sp)
					{
						_particle->setTextureWithRect(sp->getTexture(), sp->getRect());
					}
				}
				else
				{
					Texture2D * tex = TextureMgr->GetTextureByName(source_texture);
					if (tex)
					{
						_particle->setTexture(tex);
					}
					else
					{
						RKString full_path_text = File::ResolveNamePath(source_texture);
						Texture2D * tex = TextureMgr->CreateTextureFromNamePath(full_path_text, false, 0);
						if (tex)
						{
							_particle->setTexture(tex);
						}
					}
				}
			}

			float delta_size = GetGameSize().width / GetMenuParent()->GetDesignSize().x;;
			_particle->setStartSize(_particle->getStartSize() * delta_size);
			_particle->setEndSize(_particle->getEndSize()* delta_size);
			_particle->setStartSizeVar(_particle->getStartSizeVar()* delta_size);
			_particle->setEndSizeVar(_particle->getEndSizeVar() * delta_size);
			

			p_Resource = nullptr;
		}
		
		cocos2d::Node * ParticleWidget::GetResource()
		{
			return _particle;
		}

		bool ParticleWidget::SetNewResource(cocos2d::Node * resource)
		{
			if (_particle)
			{
				_particle = static_cast<ParticleSystemQuad*>(resource);
			}
			else
			{
				return false;
			}

			return true;
		}

		void ParticleWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			//WidgetEntity::FindAndInsertChildWidget(xml_value);
			//particle system not have child
			return;
		}

		void ParticleWidget::CloneChild(WidgetEntity * p_clone)
		{
			return;
		}

		void ParticleWidget::SetSize(Vec2 s)
		{
			cocos2d::Vec2 p_Size(1, 1);

			/*p_Size.x = s.x / _particle->getTexture()->getContentSize().width;
			p_Size.y = s.y / _particle->getTexture()->getContentSize().height;
			GetResource()->setScaleX(p_Size.x);
			GetResource()->setScaleY(p_Size.y);*/
		}

		Vec2 ParticleWidget::GetSize()
		{
			return Vec2(1, 1);
		}
	}
}