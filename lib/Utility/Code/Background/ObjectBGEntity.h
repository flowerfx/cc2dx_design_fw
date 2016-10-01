#ifndef __OBJECT_BG_ENTITY_H__
#define __OBJECT_BG_ENTITY_H__

#include "../DisplayView.h"
#if USE_DYNAMIC_BG
#include "../Object/BasicObject.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
USING_NS_CC;
namespace Utility
{
	namespace Background
	{
		class BGEntity : public Object::BasicObject
		{
		protected:
			Vec2		p_PointToMoveTo;
			Vec2		p_VectorMove;		

			RKString	p_name_object_bg;

			bool p_HaveFadeZoom;
			bool p_HaveFadeOpacity;

			void InitWithName(RKString name);

			std::map<RKString, xml::BasicDec *> p_action_dec;

			bool p_isFinishAction;
			bool p_haveActionDone;

			Vec2 p_PosThreadHoldRemove;

			bool IsOutOfTHreadHold();

			virtual bool OnFinishAction(Object::BASIC_ACTION ba);

			virtual void EventActionFinish();
		public:

			BGEntity();
			BGEntity(RKString name);
			virtual ~BGEntity();

			void SetScaleBG(Vec2 s);
			Vec2 GetScaleBG();

			bool VisibleBG();
			void VisibleBG(bool b);

			Vec2 GetSizeBG();
			void SetSizeBG(Vec2 s);

			void SetPositionBG(Vec2 p);
			Vec2 GetPositionBG();

			bool HaveFadeZoom() { return p_HaveFadeZoom; }
			void SetHaveFadeZoom(bool val) { p_HaveFadeZoom = val; }

			bool HaveFadeOpacity() { return p_HaveFadeOpacity; }
			void SetHaveFadeOpacity(bool val) { p_HaveFadeOpacity = val; }

			void SetActionDec(std::map<RKString, xml::BasicDec *> action);

			virtual void Init() ;
			virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) ;
			virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) ;
			virtual void Update(float dt) ;

			void MoveToPoint(Vec2 p);

			void TimeMove(float s) { time_move_to = s; }
			void SetTheThreadHold(Vec2 value, Vec2 vectormove);

			bool IsFinishAction() { return p_isFinishAction; }
		};
	}
}
#endif
#endif //__OBJECT_BG_ENTITY_H__

