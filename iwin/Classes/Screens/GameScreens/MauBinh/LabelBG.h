#ifndef __LABEL_BG_H__
#define __LABEL_BG_H__

#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "UI/UITextBMFont.h"

USING_NS_CC;
using namespace ui;


class LabelBG: public cocos2d::Node
{
private:
	Scale9Sprite*			_bg;
	TextBMFont*				_label;
	Vec2					_offset;
public:
	LabelBG();

	virtual bool init(const char* bg_name, int font_idx);
	
	void setText(const char* text);
	void reset(const char* bg_name, int font_idx);
};


#endif //__LABEL_BG_H__