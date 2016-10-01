#ifndef __ANIM_UTILS_H__
#define __ANIM_UTILS_H__

#include <vector>
#include "cocos2d.h"
USING_NS_CC;
class Card;
namespace AnimUtils
{
	void RunEffectFlopCardReturn(Card * c, Vec2 point, cocos2d::CallFunc* call_back);
	void RunEffectChiaBaiCao(Card* c, Vec2 point, float delay_time, bool isMyInfo , cocos2d::CallFunc* call_back);
	void RunEffectFlipBaiCao(Card* c, int id, cocos2d::CallFunc* call_back);

	void RunEffectMoveToFire(Card* c, Vec2 from_pos, Vec2 to_pos, cocos2d::CallFunc* call_back);
	void RunEffectChiaBai1(Card* c, Vec2 point1, float delay_time1 , Vec2 point2, float delay_time2, cocos2d::CallFunc* call_back);
	void RunEffectChiaBai(Card* c, Vec2 point, float delay_time, cocos2d::CallFunc* call_back);
	void RunEffectLayout(Card* c, Vec2 point, cocos2d::CallFunc* call_back);

	void EffectSelectCard(Card* c, cocos2d::CallFunc* call_back);
	void EffectDeselectCard(Card* c ,cocos2d::CallFunc* call_back);
};

#endif // __ANIM_UTILS_H__