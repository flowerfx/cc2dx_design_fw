#include "PanelEntity.h"
namespace Utility
{
	namespace UI
	{
		PanelEntity::PanelEntity()
		{
			p_typeUI = UI_MENU_TYPE::MENU_PANEL;
		}
		PanelEntity::~PanelEntity()
		{
		}

		Vec2 PanelEntity::GetSize() { return GetSizeResource(); }
		void PanelEntity::SetSize(Vec2 p) { SetSizeResource(p); }
		void PanelEntity::Visible(bool b) { VisibleResource(b); }
		bool PanelEntity::Visible() { return VisibleResource(); }

		void PanelEntity::SetScale(Vec2 s) { SetScaleResource(s); }
		Vec2 PanelEntity::GetScale() { return GetScaleResource(); }
		void PanelEntity::SetPosition(Vec2 p) { SetPositionResource(p); }
		cocos2d::Point PanelEntity::GetPosition() { return GetPositionResource(); }

		float PanelEntity::GetRotate(){ return GetRotateResource(); }
		void  PanelEntity::SetRotate(float r){ return SetRotateResource(r); }

		bool PanelEntity::Update(float dt)
		{
			return UpdateActionFade(dt);
		}
		void PanelEntity::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			VisitResource(renderer, transform, flags);
		}
		void PanelEntity::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			DrawResource(renderer, transform, flags);
		}

		void PanelEntity::Init(RKString name_background, RKString name_source, Vec2 pos, Vec2 size, float rotate )
		{
			LoadPanelItem(name_source);
			p_Name = name_background;
			SetPosition(pos);
			p_originPos = pos;
			SetSize(size);
			SetRotate(rotate);
			p_target_scale_to = p_Resource->getScaleX();
			p_originAngle = rotate;
			p_typeUI = UI_MENU_TYPE::MENU_PANEL;
		}

		void PanelEntity::LoadPanelItem(RKString name_panel)
		{
			auto te = TextureMgr->GetSpriteByName(name_panel);
			parseSprite(te);
		}
	}
}