#ifndef _GameEvent_H_
#define _GameEvent_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GameEvent
	{
		
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(std::string, bannerImg, BannerImg);
CC_SYNTHESIZE(std::string, popupImg, PopupImg);
CC_SYNTHESIZE(std::string, popupIcoImg, PopupIcoImg);
CC_SYNTHESIZE(int, screenId, ScreenId);

	public:
		
		GameEvent();
		virtual ~GameEvent();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
