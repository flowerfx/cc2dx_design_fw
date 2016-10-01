#ifndef _MatchList_H_
#define _MatchList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Match.h"

namespace iwinmesage
{
	class MatchList
	{
		
CC_SYNTHESIZE(std::vector<Match>, matchList, MatchList);
CC_SYNTHESIZE(std::string, pageToken, PageToken);
CC_SYNTHESIZE(int, leagueId, LeagueId);
CC_SYNTHESIZE(int64_t, startTime, StartTime);
CC_SYNTHESIZE(int64_t, countDownTime, CountDownTime);
CC_SYNTHESIZE(int, status, Status);
CC_SYNTHESIZE(std::string, info, Info);

	public:
		
		MatchList();
		virtual ~MatchList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
