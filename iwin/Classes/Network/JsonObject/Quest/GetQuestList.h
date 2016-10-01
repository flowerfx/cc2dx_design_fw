#ifndef _GetQuestList_H_
#define _GetQuestList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GetQuestList
	{
		
CC_SYNTHESIZE(bool, contactStatus, ContactStatus);
CC_SYNTHESIZE(bool, notificationStatus, NotificationStatus);

	public:
		
		GetQuestList();
		virtual ~GetQuestList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
