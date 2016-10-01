
#include "ObjectTexture.h"
#include "FlyMoney.h"
#include "UI/AdvanceUI/wText.h"

using namespace Utility;
using namespace UI_Widget;
FlyMoney* FlyMoney::s_instance = nullptr;


FlyMoney* FlyMoney::getInstance()
{
	if (!s_instance)
	{
		s_instance = new FlyMoney();
	}
	return s_instance;
}

TextBMFont* FlyMoney::getFreeLabel(s64 money)
{
	TextBMFont* label;
	if (money > 0)
	{
		for (auto f : _free_label_green)
		{
			if (f->getNumberOfRunningActions() <= 0)
			{
				return f;
			}
		}
		label = TextBMFont::create("", File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(3)->path_fnt).GetString());
		label->retain();
		_free_label_green.push_back(label);
	}
	else {
		for (auto f : _free_label_orange)
		{
			if (f->getNumberOfRunningActions() <= 0)
			{
				return f;
			}
		}
		label = TextBMFont::create("", File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(12)->path_fnt).GetString());
		label->retain();
		_free_label_orange.push_back(label);
	}


	return label;
}

void FlyMoney::fly(s64 money, Vec2 pos, cocos2d::Node* target)
{
	TextBMFont* label = getFreeLabel(money);
	if (label->getParent() != target)
	{
		label->removeFromParent();
		target->addChild(label);
	}
	label->setText(StringUtils::format("%lld", money));
	label->setPosition(pos);
	label->setOpacity(0);
	NumberToEffect* number_action = NumberToEffect::create(2.0f, money);
	number_action->SetStartNumber(0);
	label->runAction(Sequence::create(Spawn::create(MoveBy::create(2.0f, Vec2(0, 30.0f * ((money > 0)?1:-1))), FadeIn::create(2.0f), number_action, nullptr),
		DelayTime::create(1.0f),
		CallFunc::create([label]() {
		label->removeFromParent();
	}), nullptr));
}