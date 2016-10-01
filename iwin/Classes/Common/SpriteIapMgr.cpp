#include "Social/SocialManager.h"
#include "SpriteIapMgr.h"
#include "SpriteIap.h"


using namespace Utility;

#define SPR_DOWNLOAD_STATE_DOWNLOADING 0
#define SPR_DOWNLOAD_STATE_FINISH_DOWNLOAD 1

SpriteIapMgr* SpriteIapMgr::s_instance = nullptr;

SpriteIapMgr* SpriteIapMgr::getInstance()
{
	if (!s_instance)
	{
		s_instance = new SpriteIapMgr();
	}
	return s_instance;
}

SpriteIapMgr::SpriteIapMgr()
{
	s_instance = this;
}

SpriteIapMgr::~SpriteIapMgr()
{
	for (auto it = _map_download.begin(); it != _map_download.end(); it++)
	{
		CC_SAFE_DELETE(it->second);
	}
}


void SpriteIapMgr::registerDownload(SpriteIap* spr, std::string url, std::string name_save)
{
	if (_map_download.find(url) != _map_download.end())
	{
		SpriteDownloadInfo* info = _map_download[url];
		if (info->download_state == SPR_DOWNLOAD_STATE_DOWNLOADING)
		{
			info->registerSpr.push_back(spr);
		}
		else
		{
			Texture2D* tex = TextureCache::getInstance()->getTextureForKey(info->url);
			if (tex)
			{
				spr->initWithTextureDone(tex);
			}
		}
	}
	else
	{
		SpriteDownloadInfo* info = new SpriteDownloadInfo();
		info->download_state = SPR_DOWNLOAD_STATE_DOWNLOADING;
		info->registerSpr.push_back(spr);
		info->url = url;
		info->name_save = name_save;
		_map_download.insert(std::pair<std::string, SpriteDownloadInfo*>(url, info));
		SocialMgr->DownloadFile(info->url.c_str(), info->name_save.c_str(), CC_CALLBACK_3(SpriteIapMgr::doneDownload, this));
	}
}

void SpriteIapMgr::doneDownload(void* data, void* str, int tag)
{
	const char* name_save = (const char*)str;
	SpriteDownloadInfo* info = _map_download[name_save];
	Image * img = (Image *)data;
	Texture2D* tex = nullptr;
	tex = TextureCache::getInstance()->addImage(img, name_save);
	info->download_state = SPR_DOWNLOAD_STATE_FINISH_DOWNLOAD;
	for (size_t i = 0; i < info->registerSpr.size(); i++)
	{
		info->registerSpr[i]->initWithTextureDone(tex);
	}
	info->registerSpr.clear();

	CC_SAFE_DELETE(img);

}