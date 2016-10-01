#include "EmotionBoard.h"
#include "UI/UIButton.h"

#define NUM_EMOTION 60

EmotionBoard* EmotionBoard::create(cocos2d::Size board_size)
{
	EmotionBoard* rs = new EmotionBoard();
	rs->initWithSize(board_size);
	rs->autorelease();
	return rs;
}


EmotionBoard::EmotionBoard()
{
	_func_handle_chat = nullptr;
}


EmotionBoard::~EmotionBoard()
{

}


void EmotionBoard::initWithSize(cocos2d::Size size)
{
	Layout::init();
	setContentSize(size);
	_layout_container = Layout::create();
	_layout_container->setBackGroundImage("loginscreen/nine/whitepixel.png", Widget::TextureResType::PLIST);
	_layout_container->setBackGroundImageScale9Enabled(true);
	_layout_container->setBackGroundColor(Color3B::BLACK);
	_layout_container->setBackGroundImageColor(Color3B::BLACK);
	_layout_container->setBackGroundColorOpacity(255);
	_layout_container->setContentSize(size);
	cocos2d::Size emo_size = SpriteFrameCache::getInstance()->getSpriteFrameByName("loadingscreen/emotions/e (1).png")->getOriginalSizeInPixels();
	const int padding = 2;
	int num_col = size.width / ( emo_size.width + padding * 2 );
	int num_row = floor(NUM_EMOTION / num_col) + ((NUM_EMOTION % num_col == 0) ? 0 : 1);
	int new_padding = (size.width - emo_size.width * num_col) / num_col;
	for (int i = 0; i < NUM_EMOTION; i++)
	{
		std::string emo_path = StringUtils::format("loadingscreen/emotions/e (%d).png", i + 1);
		Button* btn_emo = Button::create(emo_path, emo_path, emo_path, ui::Widget::TextureResType::PLIST);
		_layout_container->addChild(btn_emo);
		btn_emo->setPosition(Vec2(new_padding / 2 + (emo_size.width + new_padding) * (i % num_col), new_padding / 2 + (emo_size.height + new_padding) * (num_row - floor(i / num_col) - 1)));
		btn_emo->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		btn_emo->setTouchEnabled(true);
		btn_emo->setSwallowTouches(true);
		//btn_emo->setZoomScale(0.85);
		btn_emo->setPressedActionEnabled(true);
		btn_emo->setTag(i);
		btn_emo->addClickEventListener(CC_CALLBACK_1(EmotionBoard::handleEmotionBtn, this));
	}
	_layout_container->setContentSize(cocos2d::Size(new_padding / 2 + (emo_size.width + new_padding) * num_col, new_padding / 2 + (emo_size.height + new_padding) * num_row));

	_scrollview = ui::ScrollView::create();
	_scrollview->setContentSize(size);
	_scrollview->setInnerContainerSize(_layout_container->getContentSize());
	_scrollview->addChild(_layout_container);
	addChild(_scrollview);
	this->setVisible(false);
}

void EmotionBoard::handleEmotionBtn(Ref* ref)
{
	Button* btn = static_cast<Button*>(ref);
	if(_func_handle_chat) _func_handle_chat( btn->getTag());
}

void EmotionBoard::setHandleChat(std::function<void(int)> handle_chat)
{
	_func_handle_chat = handle_chat;
}

void EmotionBoard::show()
{
	cocos2d::Size content_size = getContentSize();
	auto action = Sequence::create(
		CallFunc::create([this]() { this->setVisible(true); }), 
		MoveTo::create(.5f, Vec2(.0f, content_size.height)),nullptr);
	action->setTag(9999);
	this->getParent()->stopActionByTag(9998);
	this->getParent()->runAction(action);
}

void EmotionBoard::hide()
{
	this->getParent()->stopActionByTag(9999);
	auto action = Sequence::create(
		MoveTo::create(.5f, Vec2(0.f, 0.f)), CallFunc::create([this]() { this->setVisible(false); }), nullptr);
	action->setTag(9998);
	this->getParent()->runAction(action);
}
