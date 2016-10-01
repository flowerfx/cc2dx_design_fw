#include "ButtonEntity.h"
namespace Utility
{
	namespace UI
	{
		/////////////////////////////////////////////////////////////
		ButtonEntity::ButtonEntity()
		{
			p_IsPress = false;
            p_ScaleNormal = cocos2d::Point(0, 0);
			p_Scale = 0;
			p_CurrentTypePress = 0;

			p_ShowHL = false;
			p_IsDisable = false;

			p_HL = NULL;
			p_Disable = NULL;

			ID_SOUND_CLICK = NULL;

			p_typeUI = UI_MENU_TYPE::MENU_BUTTON;
		}
		ButtonEntity::~ButtonEntity()
		{
			CC_SAFE_RELEASE_NULL(p_HL);
			CC_SAFE_RELEASE_NULL(p_Disable);
			if (ID_SOUND_CLICK)
			{
				delete ID_SOUND_CLICK;
				ID_SOUND_CLICK = NULL;
			}
		}
		
		/////////////////////////////////////////////////////////////
		void ButtonEntity::Init(RKString name_button, RKString name_source, Vec2 pos, Vec2 size, RKString id_sound_click, float delay , int looping, float rotate)
		{
			LoadButtonItem(name_source);

			p_Name = name_button;
			SetPosition(pos);
			p_originPos = pos;
			SetSize(size);
			p_target_scale_to = p_Resource->getScaleX();
			p_ScaleNormal = Vec2(p_Resource->getScaleX(), p_Resource->getScaleY());
			SetRotate(rotate);
			p_originAngle = rotate;
			ID_SOUND_CLICK = new xml::sound_click_dec();
			ID_SOUND_CLICK->id_click = id_sound_click;
			ID_SOUND_CLICK->delayTime = delay;
			ID_SOUND_CLICK->loop = looping;

			p_typeUI = UI_MENU_TYPE::MENU_BUTTON;
		}
		void ButtonEntity::LoadButtonItem(RKString name_button)
		{
			auto te = TextureMgr->GetSpriteByName(name_button);
			parseSprite(te);
		
		}
		
		/////////////////////////////////////////////////////////////
		Vec2 ButtonEntity::GetSize() 
		{ 
			return GetSizeResource(); 
		}
		void ButtonEntity::SetSize(Vec2 p) 
		{ 
			SetSizeResource(p); 
		}
		
		/////////////////////////////////////////////////////////////
		void ButtonEntity::SetScale(cocos2d::Point s)
		{
			SetScaleResource(s);
		}
		cocos2d::Point ButtonEntity::GetScale()
		{
			return GetScaleResource();
		}
		
		/////////////////////////////////////////////////////////////
		void ButtonEntity::SetPosition(cocos2d::Point p)
		{
			SetPositionResource(p);
		}
		cocos2d::Point ButtonEntity::GetPosition()
		{
			return GetPositionResource();
		}
		
		/////////////////////////////////////////////////////////////
		bool ButtonEntity::IsPress()
		{
			return p_IsPress;
		}		
		void ButtonEntity::IsPress(bool b)
		{
			p_IsPress = b;
		}
		
		void ButtonEntity::UpdatePress(float dt)
		{
			CCASSERT(p_Resource, "button invaid");
			if (p_CurrentTypePress == 0)
			{
				cocos2d::Point s = p_ScaleNormal;
				if (p_IsPress)
				{
					s = p_ScaleNormal * p_Scale;
				}
				SetScale(s);
			}
			else
			{
				p_ShowHL = p_IsPress;
			}
		}

		void ButtonEntity::SetValuePress(xml::ResUIElement * res)
		{
			p_Scale = res->scale_press_down;
			p_CurrentTypePress = res->type_press_button;
			if (res->source_hl != "")
			{
				auto sprite = TextureMgr->GetSpriteByName(res->source_hl);
				if (sprite)
				{
					p_HL = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
					p_HL->retain();

					p_HL->setPosition(p_originPos);
					p_HL->setRotation(p_originAngle);
					p_HL->setOpacity(p_target_fade_alpha);

					p_HL->setScaleX(p_ScaleNormal.x);
					p_HL->setScaleY(p_ScaleNormal.y);
				}
			}
			//
			if (res->source_disable != "")
			{
				auto sprite = TextureMgr->GetSpriteByName(res->source_disable);
				if (sprite)
				{
					p_Disable = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
					p_Disable->retain();

					p_Disable->setPosition(p_originPos);
					p_Disable->setRotation(p_originAngle);
					p_Disable->setOpacity(p_target_fade_alpha);

					p_Disable->setScaleX(p_ScaleNormal.x);
					p_Disable->setScaleY(p_ScaleNormal.y);
				}
			}
		}
		/////////////////////////////////////////////////////////////
		bool ButtonEntity::Update(float dt)
		{
			return UpdateActionFade(dt);
		}

		void ButtonEntity::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			VisitResource(renderer, transform, flags);

			if (p_ShowHL)
			{
				p_HL->visit(renderer, transform, flags);
			}

			if (p_IsDisable)
			{
				p_Disable->visit(renderer, transform, flags);
			}
		}
		void ButtonEntity::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			DrawResource(renderer, transform, flags);

			if (p_ShowHL)
			{
				p_HL->draw(renderer, transform, flags);
			}

			if (p_IsDisable)
			{
				p_Disable->draw(renderer, transform, flags);
			}
		}

	}
}