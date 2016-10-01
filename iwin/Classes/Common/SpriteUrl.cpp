#include "SpriteUrl.h"
#include "MainGame.h"
#include "Screens\BaseScreen.h"
USING_NS_CC;

SpriteUrl::SpriteUrl(std::string url)
{
	
}

SpriteUrl::SpriteUrl()
{

}

SpriteUrl::~SpriteUrl()
{

}

cocos2d::Sprite* SpriteUrl::create(std::string url)
{
	SpriteUrl* rs = new SpriteUrl();
	rs->autorelease();
	GameMgr->HandleAvatarDownload(
		url, [rs](void * data, void * str, int tag)
	{
		Image * img = (Image *)data;
		Texture2D * tex = new Texture2D();
		tex->initWithImage(img, cocos2d::Texture2D::PixelFormat::RGB5A1);
		rs->initWithTexture(tex);

		CC_SAFE_DELETE(img);
	});
	return rs;
}
