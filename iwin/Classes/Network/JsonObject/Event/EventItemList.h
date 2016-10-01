#ifndef _EventItemList_H_
#define _EventItemList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "EventItem.h"

namespace iwinmesage
{
	class EventItemList
	{
		
CC_SYNTHESIZE(std::vector<EventItem>, list, List);

	public:
		
		EventItemList();
		virtual ~EventItemList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
