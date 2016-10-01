#ifndef _UpgrageItem_H_
#define _UpgrageItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "EventItem.h"

namespace iwinmesage
{
	class UpgrageItem
	{
		
CC_SYNTHESIZE(std::vector<EventItem>, list, List);
CC_SYNTHESIZE(EventItem, resultItem, ResultItem);
CC_SYNTHESIZE(int, eventType, EventType);

	public:
		
		UpgrageItem();
		virtual ~UpgrageItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
