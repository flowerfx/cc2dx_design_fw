#ifndef __PANEL_ENTITY_H__
#define __PANEL_ENTITY_H__
#include "MenuResource.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI
	{
		class PanelEntity : public MenuResource
		{
		public:
			PanelEntity();
			~PanelEntity();

			Vec2 GetSize(); 
			void SetSize(Vec2 p); 
			void Visible(bool b);
			bool Visible(); 

			void SetScale(Vec2 s);
			Vec2 GetScale();

			void SetPosition(Vec2 p);
			Vec2 GetPosition();

			float GetRotate();
			void  SetRotate(float r);

			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);
			virtual bool Update(float dt);

			void Init(RKString name_background, RKString name_source, Vec2 pos, Vec2 scale, float rotate = 0.0f);

			void LoadPanelItem(RKString name_panel);
		};

	}
}


#endif //__PANEL_ENTITY_H__

