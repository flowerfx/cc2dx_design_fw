#ifndef __SPRITE_IAP_H__
#define __SPRITE_IAP_H__

#include "cocos2d.h"

USING_NS_CC;

class SpriteIap : public cocos2d::Sprite
{
	CC_SYNTHESIZE(std::string, _url, URL)
		CC_SYNTHESIZE(std::string, _name_save, NameSave)

		void doneDownload(void* data, void* str, int tag);

public:
	void startDownload();
	void startDownload(std::string url);
	void initWithTextureDone(Texture2D* tex);
public:
	static SpriteIap* create(const char* default_name, const char* url, const char* name_save);
};

#endif //__SPRITE_IAP_H__
