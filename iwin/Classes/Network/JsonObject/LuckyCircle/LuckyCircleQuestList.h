#ifndef _LuckyCircleQuestList_H_
#define _LuckyCircleQuestList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "LuckyCircleQuestListItem.h"
#include "NewLuckyGetTiketsObject.h"

namespace iwinmesage
{
	class LuckyCircleQuestList
	{
		
CC_SYNTHESIZE(int, circleId, CircleId);
CC_SYNTHESIZE(std::vector<LuckyCircleQuestListItem>, list, List);
CC_SYNTHESIZE(NewLuckyGetTiketsObject, tiketsObject, TiketsObject);
CC_SYNTHESIZE(int, userPoint, UserPoint);
CC_SYNTHESIZE(int, ticketRequire, TicketRequire);
CC_SYNTHESIZE(std::string, eventDescription, EventDescription);
CC_SYNTHESIZE(std::string, urlImg, UrlImg);
CC_SYNTHESIZE(std::string, urlRules, UrlRules);
CC_SYNTHESIZE(int, screenIdToGetTickets, ScreenIdToGetTickets);

	public:
		
		LuckyCircleQuestList();
		virtual ~LuckyCircleQuestList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
