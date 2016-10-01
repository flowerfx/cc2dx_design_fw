#ifndef _Match_H_
#define _Match_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Team.h"

namespace iwinmesage
{
	class Match
	{
		
CC_SYNTHESIZE(int, leagueId, LeagueId);
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(Team, owner, Owner);
CC_SYNTHESIZE(Team, visitor, Visitor);
CC_SYNTHESIZE(int, ownerGoals, OwnerGoals);
CC_SYNTHESIZE(int, visitorGoals, VisitorGoals);
CC_SYNTHESIZE(int, ownerTickets, OwnerTickets);
CC_SYNTHESIZE(int, visitorTickets, VisitorTickets);
CC_SYNTHESIZE(int64_t, time, Time);
CC_SYNTHESIZE(double, ownerRate, OwnerRate);
CC_SYNTHESIZE(double, visitorRate, VisitorRate);
CC_SYNTHESIZE(int64_t, downTime, DownTime);
CC_SYNTHESIZE(int, winTeamID, WinTeamID);
CC_SYNTHESIZE(int, matchStatus, MatchStatus);

	public:
		
		Match();
		virtual ~Match();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
