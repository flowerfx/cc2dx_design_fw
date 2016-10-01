#ifndef _BetMatch_H_
#define _BetMatch_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BetMatch
	{
		
CC_SYNTHESIZE(int, matchID, MatchID);
CC_SYNTHESIZE(int, teamID, TeamID);
CC_SYNTHESIZE(int, tickets, Tickets);
CC_SYNTHESIZE(bool, isSucceed, IsSucceed);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		BetMatch();
		virtual ~BetMatch();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
