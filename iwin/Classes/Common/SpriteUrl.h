#include "cocos2d.h"

#ifndef __IWIN_SPRITE_H__
#define __IWIN_SPRITE_H__

class SpriteUrl:public cocos2d::Sprite
{

private:
	
public:
	SpriteUrl();
	SpriteUrl(std::string url);
	~SpriteUrl();
	static cocos2d::Sprite* create(std::string url);
};

#endif // __IWIN_SPRITE_H__