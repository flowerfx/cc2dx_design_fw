#ifndef __SPRITE_IAP_MGR_H__
#define __SPRITE_IAP_MGR_H__

#include "cocos2d.h"

class SpriteIap;

struct SpriteDownloadInfo
{
	std::string url;
	std::string name_save;
	int			download_state;
	std::vector<SpriteIap*> registerSpr;
};

class SpriteIapMgr
{
	std::map<std::string, SpriteDownloadInfo*> _map_download;
	static SpriteIapMgr*	s_instance;
public:
	SpriteIapMgr();
	virtual ~SpriteIapMgr();
	void registerDownload(SpriteIap* spr, std::string url, std::string name_save);
	void doneDownload(void* data, void* str, int tag);

	static SpriteIapMgr* getInstance();
};

#define GetSpriteIapMgr() SpriteIapMgr::getInstance()

#endif //__SPRITE_IAP_H__
