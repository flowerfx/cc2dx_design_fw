#ifndef __EMOTION_BOARD_H__
#define __EMOTION_BOARD_H__

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIScrollView.h"

USING_NS_CC;
using namespace ui;

class EmotionBoard : public ui::Layout 
{
private:
	Layout*		_layout_container;
	ScrollView* _scrollview;
	std::function<void(int)>		_func_handle_chat;


	void handleEmotionBtn(Ref* ref);
public:
	EmotionBoard();
	virtual ~EmotionBoard();
	void setHandleChat(std::function<void(int)> handle_chat);
	void initWithSize(cocos2d::Size size);
	static EmotionBoard* create(cocos2d::Size board_size);
	void show();
	void hide();
};

#endif // __EMOTION_BOARD_H__