#include "IwinScale9Sprite.h"

using namespace ui;

IwinScale9Sprite* IwinScale9Sprite::createWithSpriteFrameName(const char* spriteFrameName)
{
	IwinScale9Sprite* pReturn = new (std::nothrow) IwinScale9Sprite();
	if (pReturn && pReturn->initWithSpriteFrameName(spriteFrameName))
	{
		pReturn->autorelease();
		return pReturn;
	}
	CC_SAFE_DELETE(pReturn);

	log("Could not allocate Scale9Sprite()");
	return NULL;
}

bool IwinScale9Sprite::initWithSpriteFrameName(const std::string& spriteFrameName)
{
	bool rs = Scale9Sprite::initWithSpriteFrameName(spriteFrameName);
	setLeft(_capInsetsInternal.origin.x);
	setTop(_capInsetsInternal.origin.y);
	setRight(_preferredSize.width - _capInsetsInternal.origin.x - _capInsetsInternal.size.width);
	setBot(_preferredSize.height - _capInsetsInternal.origin.y - _capInsetsInternal.size.height);
	return rs;
}

void IwinScale9Sprite::setInternalSize(Size size)
{
	setContentSize(Size(size.width + getLeft() + getRight(), size.height + getTop() + getBot()));
}

void IwinScale9Sprite::setInternalPos(Vec2 pos)
{
	setPosition(pos - Vec2(getLeft(), getBot()));
}