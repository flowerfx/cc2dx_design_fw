#ifndef __TEXT_ENTITY_H__
#define __TEXT_ENTITY_H__
#include "MenuResource.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI
	{
		class TextEntity : public MenuResource
		{
		private:
			RKString	p_curStr;

			Color4B p_tint;
			Color4B p_tintBorder;

			bool	p_IsUnicode;
			int		p_borderSize;
			float	p_originSize;

			int		p_fontIdx;
		public:
			TextEntity();
			~TextEntity();
			//
			float GetRotate();
			void SetRotate(float r);
			//
			Vec2 GetScale();
			void SetScale(Vec2 s);
			//
			bool IsVisible();
			void IsVisible(bool b);
			//
			void SetPosition(Vec2 p);
			Vec2 GetPosition();
			//
			void SetTint(Color4B tint);
			void SetTintBorder(Color4B tintBorder, int bordersize);
			//
			void SetTextLabel(RKString name_text, Label * p, float size_text , xml::ResUIElement * res = NULL,  bool IsUnicode = false);

			void ReloadText();
			//
			void SetText(RKString text);
			RKString GetText();
			//
			virtual bool Update(float dt);
			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);

		};
	}
}


#endif //__TEXT_ENTITY_H__

