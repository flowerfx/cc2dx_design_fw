#ifndef _EventAwardList_H_
#define _EventAwardList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "EventAward.h"

namespace iwinmesage
{
	class EventAwardList
	{
		
CC_SYNTHESIZE(int, eventItemId, EventItemId);
CC_SYNTHESIZE(std::vector<EventAward>, listAward, ListAward);

	public:
		
		EventAwardList();
		virtual ~EventAwardList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
