#ifndef _LeagueList_H_
#define _LeagueList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "League.h"

namespace iwinmesage
{
	class LeagueList
	{
		
CC_SYNTHESIZE(std::vector<League>, leagueList, LeagueList);
CC_SYNTHESIZE(int, currentRound, CurrentRound);

	public:
		
		LeagueList();
		virtual ~LeagueList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
