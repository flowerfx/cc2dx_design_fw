
#include "Screens/ScreenManager.h"
#include "Screens/Object/Card.h"
#include "AnimUtils.h"
USING_NS_CC;
namespace AnimUtils
{
	void RunEffectFlopCardReturn(Card * c, Vec2 point, cocos2d::CallFunc* call_back)
	{
		//xml::BasicDec dec;
		//dec.InsertDataVector2("POSITION", point);
		//c->SetHookCommand("RETURN_FLOP_CARD", "act0", 0, 0, dec, "POSITION");
		//c->SetActionCommandWithName("RETURN_FLOP_CARD", call_back);
		//c->ClearHookCommand("RETURN_FLOP_CARD", "act0", 0, 0);

		NODE(c)->runAction(Sequence::create(MoveTo::create(0.2f, point),call_back,nullptr));
	}

	void RunEffectChiaBaiCao(Card* c, Vec2 point, float delay_time, bool isMyInfo, cocos2d::CallFunc* call_back)
	{
		xml::BasicDec dec;
		dec.InsertDatafloat("DELAY", delay_time);
		dec.InsertDataVector2("POSITION", point);

		if (isMyInfo)
		{
			c->SetHookCommand("CHIA_BAI_CAO", "act0", 0, 2, dec, "DELAY");
			c->SetHookCommand("CHIA_BAI_CAO", "act1", 0, 0, dec, "POSITION");

			c->SetActionCommandWithName("CHIA_BAI_CAO", call_back);

			c->ClearHookCommand("CHIA_BAI_CAO", "act0", 0, 2);
			c->ClearHookCommand("CHIA_BAI_CAO", "act1", 0, 0);
		}
		else
		{
			c->SetHookCommand("CHIA_BAI_CAO_1", "act0", 0, 2, dec, "DELAY");
			c->SetHookCommand("CHIA_BAI_CAO_1", "act1", 0, 0, dec, "POSITION");

			c->SetActionCommandWithName("CHIA_BAI_CAO_1", call_back);

			c->ClearHookCommand("CHIA_BAI_CAO_1", "act0", 0, 2);
			c->ClearHookCommand("CHIA_BAI_CAO_1", "act1", 0, 0);
		}
	}

	void RunEffectFlipBaiCao(Card* c, int id, cocos2d::CallFunc* call_back)
	{
		CCLOG("BAI CAO Id flip: %d", id);
		//c->EventActionFinish();
		c->SetActionCommandWithName("FLIP_BAI_CAO", call_back);
	}

	void RunEffectChiaBai(Card* c, Vec2 point, float delay_time, cocos2d::CallFunc* call_back)
	{
		xml::BasicDec dec;
		dec.InsertDatafloat("DELAY", delay_time);
		dec.InsertDataVector2("POSITION", point);

		c->SetHookCommand("CHIA_BAI", "act0", 0, 4, dec, "DELAY");
		c->SetHookCommand("CHIA_BAI", "act0", 0, 5, dec, "POSITION");

		c->SetActionCommandWithName("CHIA_BAI", call_back);

		c->ClearHookCommand("CHIA_BAI", "act0", 0, 4);
		c->ClearHookCommand("CHIA_BAI", "act0", 0, 5);
	}

	void RunEffectChiaBai1(Card* c, Vec2 point1, float delay_time1, Vec2 point2, float delay_time2, cocos2d::CallFunc* call_back)
	{
		xml::BasicDec decOther;
		decOther.InsertDatafloat("DELAY", delay_time1);
		decOther.InsertDataVector2("POSITION", point1);
		xml::BasicDec decOther1;
		decOther1.InsertDatafloat("DELAY", delay_time2);
		decOther1.InsertDataVector2("POSITION", point2);

		c->SetHookCommand("CHIA_BAI_OTHER_1", "act0", 0, 2, decOther, "DELAY");
		c->SetHookCommand("CHIA_BAI_OTHER_1", "act1", 1, 2, decOther, "POSITION");
		c->SetHookCommand("CHIA_BAI_OTHER_1", "act1", 2, 0, decOther1, "DELAY");
		c->SetHookCommand("CHIA_BAI_OTHER_1", "act2", 2, 0, decOther1, "POSITION");

		c->SetActionCommandWithName("CHIA_BAI_OTHER_1", call_back);

		c->ClearHookCommand("CHIA_BAI_OTHER_1", "act0", 0, 2);
		c->ClearHookCommand("CHIA_BAI_OTHER_1", "act1", 1, 2);
		c->ClearHookCommand("CHIA_BAI_OTHER_1", "act1", 2, 0);
		c->ClearHookCommand("CHIA_BAI_OTHER_1", "act2", 2, 0);
	}

	void RunEffectLayout(Card* c, Vec2 point, cocos2d::CallFunc* call_back)
	{
		if (c->GetPosition().x > point.x)
		{
			xml::BasicDec dec;
			dec.InsertDataVector2("POSITION", point);

			c->SetHookCommand("LAYOUT_EFFECT_2", "act0", 0, 2, dec, "POSITION");
			c->SetActionCommandWithName("LAYOUT_EFFECT_2", call_back, false);
			c->ClearHookCommand("LAYOUT_EFFECT_2", "act0", 0, 2);
		}
		else
		{
			xml::BasicDec dec;
			dec.InsertDataVector2("POSITION", point);
			c->SetHookCommand("LAYOUT_EFFECT_1", "act0", 0, 2, dec, "POSITION");
			c->SetActionCommandWithName("LAYOUT_EFFECT_1", call_back, false);
			c->ClearHookCommand("LAYOUT_EFFECT_1", "act0", 0, 2);
		}
	}

	void RunEffectMoveToFire(Card* c, Vec2 from_pos, Vec2 to_pos, cocos2d::CallFunc* call_back)
	{
		xml::BasicDec dec;
		dec.InsertDataVector2("POSITION", to_pos);
		dec.InsertDataVector2("SET_POSITION", from_pos);

		c->SetHookCommand("MOVE_CENTER_FRONT", "act1", 0, 0, dec, "POSITION");
		c->SetHookCommand("MOVE_CENTER_FRONT", "act0", 0, 1, dec, "SET_POSITION");

		c->SetActionCommandWithName("MOVE_CENTER_FRONT", call_back);

		c->ClearHookCommand("MOVE_CENTER_FRONT", "act0", 0, 1);
		c->ClearHookCommand("MOVE_CENTER_FRONT", "act1", 0, 0);
	}
	
	void EffectSelectCard(Card* c , cocos2d::CallFunc* call_back)
	{
		c->SetActionCommandWithName("SELECT_CARD" , call_back);
	}

	void EffectDeselectCard(Card* c, cocos2d::CallFunc* call_back)
	{
		c->SetActionCommandWithName("DESELECT_CARD", call_back);
	}

}
