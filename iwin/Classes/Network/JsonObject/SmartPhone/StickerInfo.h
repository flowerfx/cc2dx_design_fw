#ifndef _StickerInfo_H_
#define _StickerInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class StickerInfo
	{
		
CC_SYNTHESIZE(std::string, version, Version);
CC_SYNTHESIZE(int, count, Count);
CC_SYNTHESIZE(std::string, link, Link);

	public:
		
		StickerInfo();
		virtual ~StickerInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
