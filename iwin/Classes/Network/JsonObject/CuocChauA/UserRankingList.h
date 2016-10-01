#ifndef _UserRankingList_H_
#define _UserRankingList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "UserRanking.h"

namespace iwinmesage
{
	class UserRankingList
	{
		
CC_SYNTHESIZE(std::vector<UserRanking>, rankingList, RankingList);

	public:
		
		UserRankingList();
		virtual ~UserRankingList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
