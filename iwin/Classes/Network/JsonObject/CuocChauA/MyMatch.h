#ifndef _MyMatch_H_
#define _MyMatch_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Match.h"

namespace iwinmesage
{
	class MyMatch
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(Match, match, Match);
CC_SYNTHESIZE(int, myTeamID, MyTeamID);
CC_SYNTHESIZE(int, myTickets, MyTickets);
CC_SYNTHESIZE(int64_t, winMoney, WinMoney);
CC_SYNTHESIZE(int64_t, totalBetMoney, TotalBetMoney);
CC_SYNTHESIZE(int64_t, time, Time);
CC_SYNTHESIZE(bool, canGetWin, CanGetWin);

	public:
		
		MyMatch();
		virtual ~MyMatch();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
