#ifndef _League_H_
#define _League_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class League
	{
		
CC_SYNTHESIZE(int, leagueId, LeagueId);
CC_SYNTHESIZE(std::string, imgUrl, ImgUrl);
CC_SYNTHESIZE(std::string, name, Name);

	public:
		
		League();
		virtual ~League();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
