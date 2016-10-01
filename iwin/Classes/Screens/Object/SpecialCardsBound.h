#ifndef __SPECIAL_CARDS_BOUND_H__
#define __SPECIAL_CARDS_BOUND_H__

#include "cocos2d.h"
#include "UI/UIScale9Sprite.h"

USING_NS_CC;
using namespace ui;

class SpecialCardsBound : public cocos2d::Node
{
private:
	cocos2d::Rect			_cur_rect;
	Scale9Sprite*			_spr_rect;
	Sprite*					_spr_glow_top;
	Sprite*					_spr_glow_bot;

	void		updateBound();
	void		setUpBot();
	void		setUpTop();

public:
	virtual bool init();

			void setBound(cocos2d::Rect);
};

#endif // __SPECIAL_CARDS_BOUND_H__