#ifndef __LIST_VIEW_ENTITY_H__
#define __LIST_VIEW_ENTITY_H__
#include "MenuResource.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI
	{
		class ListViewEntity : public MenuResource
		{
		public:
			ListViewEntity();
			~ListViewEntity();

			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags);
			virtual bool Update(float dt);

			void Init(RKString name_background, RKString name_source, Vec2 pos, Vec2 scale, float rotate = 0.0f);

			void InitListView(RKString name_panel);

			void PushDataToListView(xml::ResUIElement * res);
		};

	}
}


#endif //__LIST_VIEW_ENTITY_H__

