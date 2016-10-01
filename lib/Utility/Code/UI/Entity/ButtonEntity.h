#ifndef __BUTTON_ENTITY_H__
#define __BUTTON_ENTITY_H__

#include "MenuResource.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI
	{
		class ButtonEntity : public MenuResource
		{
		private:
			bool		p_IsPress;
			Vec2		p_ScaleNormal;
			float		p_Scale;

			int			p_CurrentTypePress;
			bool		p_ShowHL;
			bool		p_IsDisable;

			Sprite	*	p_HL;
			Sprite  *   p_Disable;

			xml::sound_click_dec *	ID_SOUND_CLICK;
		public:
			ButtonEntity();
			~ButtonEntity();

			cocos2d::Size GetNormalSize()
			{
				return cocos2d::Size(p_Resource->getContentSize().width * p_ScaleNormal.x,
					p_Resource->getContentSize().height * p_ScaleNormal.y);
			}

			Vec2 GetSize();
			void SetSize(Vec2 p);

			void SetScale(Vec2 s);
			Vec2 GetScale();

			void SetPosition(Vec2 p);
			Vec2 GetPosition();

			bool IsPress();
			void IsPress(bool b);

			float GetRotate(){ return GetRotateResource(); }
			void  SetRotate(float r){ return SetRotateResource(r); }

			void Visible(bool b) { VisibleResource(b); }
			bool Visible() { return VisibleResource(); }

			void UpdatePress(float dt);

			virtual bool Update(float dt);
			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) ;
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) ;

			void Init(RKString name_button, RKString name_source, Vec2 pos, Vec2 scale,RKString id_sound_click,float delay = 0, int loop = 0, float rotate = 0.0f);
			void SetValuePress(xml::ResUIElement * res );

			void LoadButtonItem(RKString name_button);

			xml::sound_click_dec* GetIdSound() { return ID_SOUND_CLICK; }

			void SetScaleNormal(Vec2 value) { p_ScaleNormal = value; }

		};

	}
}

#endif //__BUTTON_ENTITY_H__

