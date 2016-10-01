#include "TextEntity.h"
namespace Utility
{
	namespace UI
	{
		TextEntity::TextEntity()
		{
			p_IsUnicode = false;
			p_IsVisible = true;
			p_borderSize = 0;
			p_originSize = 0;

			p_fontIdx = 0;
			p_curStr = "";
			p_textLabel = NULL;

			p_typeUI = UI_MENU_TYPE::MENU_TEXT;
		}

		TextEntity::~TextEntity()
		{
			CC_SAFE_RELEASE(p_textLabel);
		}
		//
		float TextEntity::GetRotate()
		{
			return GetRotateResource();
		}
		void TextEntity::SetRotate(float r)
		{
			SetRotateResource(r);
		}
		//
		Vec2 TextEntity::GetScale()
		{
			return GetScaleResource();
		}
		void TextEntity::SetScale(Vec2 s)
		{
			SetScaleResource(s);
		}
		//
		bool TextEntity::IsVisible()
		{
			return VisibleResource();
		}
		void TextEntity::IsVisible(bool b)
		{
			VisibleResource(b);
		}
		//
		void TextEntity::SetPosition(Vec2 p)
		{
			SetPositionResource(p);
		}
		Vec2 TextEntity::GetPosition()
		{
			return GetPositionResource();
		}
		//
		void TextEntity::SetTint(Color4B tint)
		{
			p_tint = tint;
			p_textLabel->setColor(Color3B(p_tint.r, p_tint.g, p_tint.b));
		}
		void TextEntity::SetTintBorder(Color4B tintBorder, int bordersize)
		{
			if (XMLMgr->GetHarwardProfile() == xml::HARDWARE_LOW)
			{
				//not set border when memory low
				return;
			}
			p_tintBorder = tintBorder;
			p_borderSize = bordersize;
			if (TextureMgr->GetLevelMipMap() == 0 && p_borderSize > 0)
			{
				p_textLabel->enableOutline(p_tintBorder, bordersize);
			}
		}
		//

		void TextEntity::SetTextLabel(RKString name_text, Label * p, float size_text, xml::ResUIElement * res, bool IsUnicode)
		{
			CCASSERT(p, "string invalid!");
			auto posRatio = ConvertPos(res->origin_PosRatio);
            auto pos = cocos2d::Point(GetGameSize().width * posRatio.x, GetGameSize().height * posRatio.y);
			auto scale = res->origin_ScaleRatio;
#ifdef OS_IOS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif
			p_fontIdx = res->fontIdx;
#ifdef OS_IOS
#pragma clang diagnostic pop   
#endif
			p_IsUnicode = IsUnicode;
			p_originSize = scale.x * GetScaleFactor();
#ifdef USE_TEXT
			p_textLabel = Text::create(res->source.GetString(), p->getTTFConfig().fontFilePath.c_str(), size_text);
#else
			TextHAlignment align = (TextHAlignment)res->align;
			TTFConfig config(p->getTTFConfig().fontFilePath.c_str(), size_text, IsUnicode ? GlyphCollection::ASCII : GlyphCollection::DYNAMIC);
			p_textLabel = p->createWithTTF(config, res->source.GetString(), align);
#endif
			p_curStr = res->source;
			p_textLabel->retain();

			SetPosition(pos);
			p_originPos = pos;
			//
			SetTint(Color4B(res->Tint[0], res->Tint[1], res->Tint[2], (res->origin_opacity / 100) * 255));
			SetTintBorder(Color4B(res->TintBorder[0], res->TintBorder[1], res->TintBorder[2], res->TintBorder[3]), (int)(res->BorderWidth));

			SetRotate(res->origin_Rotate);
			float scale_i = p_originSize / size_text;
			SetScale(Vec2(scale_i, scale_i));

			p_target_scale_to = GetResource()->getScaleX();

			p_originAngle = res->origin_Rotate;

			p_Name = name_text;

			p_typeUI = UI_MENU_TYPE::MENU_TEXT;
		}

		void TextEntity::ReloadText()
		{

		}
		//
		void TextEntity::SetText(RKString text)
		{
			p_textLabel->setString(text.GetString());
			p_curStr = text;
		}
		RKString TextEntity::GetText()
		{
			return p_curStr;
		}
		//
		bool TextEntity::Update(float dt) 
		{
			return UpdateActionFade(dt);
		}
		void TextEntity::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			VisitResource(renderer, transform, flags);
		}
		void TextEntity::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			DrawResource(renderer, transform, flags);
		}
	}
}