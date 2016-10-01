#include "ObjectBGEntity.h"

#if USE_DYNAMIC_BG

#include "../ObjectTexture.h"
#include "../InputManager.h"
namespace Utility
{
	namespace Background
	{
		BGEntity::BGEntity()
		{
			p_PointToMoveTo = Vec2(0, 0);
			p_VectorMove = Vec2(0, 0);

			p_name_object_bg = "";

			p_HaveFadeZoom = false;
			p_HaveFadeOpacity = false;

			p_isFinishAction = false;
			p_haveActionDone = false;
		}
		BGEntity::BGEntity(RKString name)
		{
			p_PointToMoveTo = Vec2(0, 0);
			p_VectorMove = Vec2(0, 0);

			p_name_object_bg = name;

			p_action_dec.clear();
		}

		BGEntity::~BGEntity()
		{
			FreeObject(); 

			p_action_dec.clear();
		}

		void BGEntity::SetScaleBG(Vec2 s)
		{
			SetScale(s);
		}

		Vec2 BGEntity::GetScaleBG()
		{
			return GetScale();
		}

		bool BGEntity::VisibleBG()
		{
			return IsVisible();
		}

		void BGEntity::VisibleBG(bool b)
		{
			SetVisible(b);
		}

		Vec2 BGEntity::GetSizeBG()
		{
			return GetSize();
		}

		void BGEntity::SetSizeBG(Vec2 s)
		{
			SetSize(s);
		}

		void BGEntity::SetPositionBG(Vec2 p)
		{
			SetPosition(p);
		}

		Vec2 BGEntity::GetPositionBG()
		{
			return GetPosition();
		}

		void BGEntity::InitWithName(RKString name)
		{
			bool res = OnInitFirstValueBasicObject(name);
		}

		void BGEntity::Init()
		{
			InitWithName(p_name_object_bg);
		}

		void BGEntity::SetTheThreadHold(Vec2 value, Vec2 vectormove)
		{
			vectormove.normalize();
			p_VectorMove = vectormove;
			p_PosThreadHoldRemove = value;
		}

		void BGEntity::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (!p_isFinishAction)
			{
				VisitObject(renderer, transform, flags);
			}
		}

		void BGEntity::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
		{
			if (!p_isFinishAction)
			{
				DrawObject(renderer, transform, flags);
			}
		}

		void BGEntity::Update(float dt)
		{
			if (!p_isFinishAction)
			{
				UpdateObject(dt);
				//check is out of threadhold
				bool IsOut = IsOutOfTHreadHold();
				if (IsOut)
				{
					p_isFinishAction;
				}

			}
		}

		void BGEntity::MoveToPoint(Vec2 p)
		{
			SetToMoveToPoint(p, time_move_to);
		}

		bool BGEntity::IsOutOfTHreadHold()
		{
			bool isOut = false;
			Vec2 PosBG = GetPositionBG();
			if (abs(p_VectorMove.x) > abs(p_VectorMove.y))
			{
				if (p_VectorMove.x > 0)
				{
					isOut = PosBG.x > p_PosThreadHoldRemove.x;
				}
				else
				{
					isOut = PosBG.x < p_PosThreadHoldRemove.x;
				}
			}
			else if (abs(p_VectorMove.x) < abs(p_VectorMove.y))
			{
				if (p_VectorMove.y > 0)
				{
					isOut = PosBG.y > p_PosThreadHoldRemove.y;
				}
				else
				{
					isOut = PosBG.y < p_PosThreadHoldRemove.y;
				}
			}

			return isOut;
		}

		bool BGEntity::OnFinishAction(Object::BASIC_ACTION ba)
		{
			p_haveActionDone = true;
			//if (ba == Object::BASIC_ACTION::BA_MOVE_TO 
			//	|| ba == Object::BASIC_ACTION::BA_MOVE_BY
			//	|| ba == Object::BASIC_ACTION::BA_MOVE_DELAY_TO 
			//	|| ba == Object::BASIC_ACTION::BA_MOVE_DELAY_BY
			//	)
			//{
			//	float time_fade_ = 0;
			//	//after move complete so process action its
			//	if (p_HaveFadeOpacity)
			//	{
			//		auto __action = p_action_dec.at("Opacity");
			//		time_fade_ = __action->GetDatafloat("time");
			//		OnSetProcessFadeActionByDec(__action, __action->GetDataInt("origin"), __action->GetDataInt("first"), 0, 0, false, false);
			//	}
			//	if (p_HaveFadeZoom)
			//	{
			//		auto __action = p_action_dec.at("Size");
			//		if (__action->GetDatafloat("time") > time_fade_)
			//			time_fade_ = __action->GetDatafloat("time");
			//		OnSetProcessScaleActionByDec(__action, -__action->GetDatafloat("scale") * GetGameSize().width,0,0,false,false);
			//	}
			//	//
			//	if (time_fade_ > 0)
			//	{
			//		auto __data_layer = p_action_dec.at("Pos");
			//		//
			//		Vec2 vector_move = __data_layer->GetDataVector2("vector");
			//		Vec2 _cur_pos = GetPositionBG();
			//		//
			//		float time_move_origin = __data_layer->GetDatafloat("time");
			//		//
			//		if (ba == Object::BASIC_ACTION::BA_MOVE_TO)
			//		{
			//			SetToMoveToPoint(vector_move, time_fade_);
			//		}
			//		else if (ba == Object::BASIC_ACTION::BA_MOVE_BY)
			//		{
			//			vector_move.x = (vector_move.x * time_fade_ / time_move_origin) + _cur_pos.x;
			//			vector_move.y = (vector_move.y * time_fade_ / time_move_origin) + _cur_pos.y;
			//			//
			//			SetToMoveByVector(vector_move, time_fade_);
			//		}
			//		p_HaveFadeOpacity = false;
			//		p_HaveFadeZoom = false;

			//		return false;
			//	}
			//}		

			return true;
		}

		void BGEntity::SetActionDec(std::map<RKString, xml::BasicDec *> action)
		{
			p_action_dec = action;
		}

		void BGEntity::EventActionFinish()
		{
			//happen when all action list == 0
			if (p_haveActionDone)
				p_isFinishAction = true;
		}
	}
}

#endif//USE_DYNAMIC_BG