#ifndef _EventInfo_H_
#define _EventInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "EventItem.h"
#include "EventAwardList.h"
#include "UpgrageItem.h"

namespace iwinmesage
{
	class EventInfo
	{
		
CC_SYNTHESIZE(int, eventType, EventType);
CC_SYNTHESIZE(std::vector<EventItem>, listItems, ListItems);
CC_SYNTHESIZE(std::vector<EventAwardList>, listAward, ListAward);
CC_SYNTHESIZE(std::string, lastResult, LastResult);
CC_SYNTHESIZE(std::vector<UpgrageItem>, listUpgrageSuggestion, ListUpgrageSuggestion);

	public:
		
		EventInfo();
		virtual ~EventInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
