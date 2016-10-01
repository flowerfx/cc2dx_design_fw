#ifndef __FLY_MONEY_H__
#define __FLY_MONEY_H__

#include "cocos2d.h"
#include "ui/UITextBMFont.h"
#include "Common/Common.h"
USING_NS_CC;
using namespace ui;

class FlyMoney
{
private:
	static FlyMoney*		s_instance;
	std::vector<TextBMFont*> _free_label_green;
	std::vector<TextBMFont*> _free_label_orange;

	TextBMFont*	getFreeLabel(s64 money);
public:
	static FlyMoney* getInstance();

	void fly(s64 money, Vec2 pos, cocos2d::Node* target);

};

#define GetFlyMoney() FlyMoney::getInstance()

#endif // __FLY_MONEY_H__