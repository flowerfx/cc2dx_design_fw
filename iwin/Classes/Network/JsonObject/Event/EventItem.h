#ifndef _EventItem_H_
#define _EventItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "EventAward.h"

namespace iwinmesage
{
	class EventItem
	{
		
CC_SYNTHESIZE(int, eventType, EventType);
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(EventAward, award, Award);
CC_SYNTHESIZE(std::string, des, Des);
CC_SYNTHESIZE(int, numOfItem, NumOfItem);
CC_SYNTHESIZE(bool, isUsedSuccess, IsUsedSuccess);

	public:
		
		EventItem();
		virtual ~EventItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
