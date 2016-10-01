#include "SpecialCardsBound.h"
#include "Utils.h"


bool SpecialCardsBound::init()
{
	Node::init();

	setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_spr_rect = Scale9Sprite::createWithSpriteFrameName("gamescreen-tienlen-screen/highlight_quan_bai.9.png");
	_spr_rect->setPosition(Utils::s_bounding_offset * -1);
	_spr_rect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	addChild(_spr_rect);
	_spr_glow_top = Sprite::createWithSpriteFrameName("gamescreen-tienlen-screen/vachsang.png");
	addChild(_spr_glow_top);
	_spr_glow_bot = Sprite::createWithSpriteFrameName("gamescreen-tienlen-screen/vachsang.png");
	addChild(_spr_glow_bot);
	return true;
}

void SpecialCardsBound::setBound(cocos2d::Rect bound)
{
	_cur_rect = bound;
	updateBound();
}

void SpecialCardsBound::setUpBot()
{
	_spr_glow_bot->setPosition(Vec2::ZERO - Utils::s_bounding_offset / 2);
	_spr_glow_bot->setScale(.0f);
}

void SpecialCardsBound::setUpTop()
{
	_spr_glow_top->setPosition(Vec2(_cur_rect.size.width + Utils::s_bounding_offset.x / 2, _cur_rect.size.height + Utils::s_bounding_offset.y / 2));
	_spr_glow_top->setScale(.0f);
}

void SpecialCardsBound::updateBound()
{
	cocos2d::Size real_size = cocos2d::Size(_cur_rect.size.width + Utils::s_bounding_offset.x * 2, _cur_rect.size.height + Utils::s_bounding_offset.y * 2);
	_spr_rect->setContentSize(real_size);
	_spr_glow_bot->stopAllActions();
	_spr_glow_bot->runAction(RepeatForever::create(Sequence::create( CallFunc::create(CC_CALLBACK_0(SpecialCardsBound::setUpBot, this) ),
		Spawn::create(FadeIn::create(1.0f), MoveBy::create(1.0f, Vec2(real_size.width / 2, 0)), ScaleTo::create(1.0f, 1.0f), nullptr),
		Spawn::create(FadeOut::create(1.0f), MoveBy::create(1.0f, Vec2(real_size.width / 2, 0)), ScaleTo::create(1.0f, .0f), nullptr), nullptr)));

	_spr_glow_top->stopAllActions();
	_spr_glow_top->runAction(RepeatForever::create(Sequence::create( CallFunc::create(CC_CALLBACK_0(SpecialCardsBound::setUpTop, this) ),
		Spawn::create(FadeIn::create(1.0f), MoveBy::create(1.0f, Vec2(-real_size.width / 2, 0)), ScaleTo::create(1.0f, 1.0f), nullptr),
		Spawn::create(FadeOut::create(1.0f), MoveBy::create(1.0f, Vec2(-real_size.width / 2, 0)), ScaleTo::create(1.0f, .0f), nullptr), nullptr)));
}