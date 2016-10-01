
#include "ObjectTexture.h"
#include "LabelBG.h"
using namespace Utility;

LabelBG::LabelBG():
_bg(nullptr),
_label(nullptr)
{

}


bool LabelBG::init(const char* bg_name, int font_idx)
{
	Node::init();

	reset(bg_name, font_idx);
	return true;
}

void LabelBG::reset(const char* bg_name, int font_idx)
{
	if (_bg) _bg->removeFromParent();
	if (_label) _label->removeFromParent();

	SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(bg_name);
	_bg = Scale9Sprite::createWithSpriteFrameName(bg_name);
	_offset = frame->GetCapInsert().origin;
	cocos2d::Rect test = _bg->getCapInsets();
	_bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	addChild(_bg);

	_label = TextBMFont::create("",  File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(font_idx)->path_fnt).GetString());
	_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	addChild(_label);
}

void LabelBG::setText(const char* text)
{
	_label->setText(text);
	cocos2d::Size size = static_cast<Label*>(_label->getVirtualRenderer())->getContentSize();
	size.width += _offset.x * 2;
	size.height += _offset.y * 2;
	_bg->setContentSize(size);
	setContentSize(size);
}