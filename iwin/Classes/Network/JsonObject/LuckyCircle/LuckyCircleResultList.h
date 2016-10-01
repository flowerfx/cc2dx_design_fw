#ifndef _LuckyCircleResultList_H_
#define _LuckyCircleResultList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "LuckyCircleResultListItem.h"
#include "NewLuckyGetTiketsObject.h"

namespace iwinmesage
{
	class LuckyCircleResultList
	{
		
CC_SYNTHESIZE(int, circleId, CircleId);
CC_SYNTHESIZE(std::vector<LuckyCircleResultListItem>, list, List);
CC_SYNTHESIZE(NewLuckyGetTiketsObject, tiketsObject, TiketsObject);
CC_SYNTHESIZE(int, userPoint, UserPoint);
CC_SYNTHESIZE(std::string, resultString, ResultString);
CC_SYNTHESIZE(int64_t, bonusMoney, BonusMoney);
CC_SYNTHESIZE(int, bonusType, BonusType);
CC_SYNTHESIZE(int64_t, winsDown, WinsDown);
CC_SYNTHESIZE(int, ticketsDown, TicketsDown);

	public:
		
		LuckyCircleResultList();
		virtual ~LuckyCircleResultList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
