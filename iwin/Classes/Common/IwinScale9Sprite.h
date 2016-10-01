#ifndef __IWIN_SCALE9SPRITE_H__
#define __IWIN_SCALE9SPRITE_H__ 
#include "cocos2d.h"
#include "UI/UIScale9Sprite.h"
USING_NS_CC;
using namespace ui;

class IwinScale9Sprite: public Scale9Sprite
{
private:
	Vec2				_origin_offset;
	CC_SYNTHESIZE(int, _off_left, Left)
	CC_SYNTHESIZE(int, _off_right, Right)
	CC_SYNTHESIZE(int, _off_top, Top)
	CC_SYNTHESIZE(int, _off_bot, Bot)
public:

	virtual bool initWithSpriteFrameName(const std::string& spriteFrameName) override;


    void setInternalSize(cocos2d::Size size);
	void setInternalPos(Vec2 pos);
	static IwinScale9Sprite* createWithSpriteFrameName(const char* spriteFrameName);
};

#endif //__IWIN_SCALE9SPRITE_H__ 