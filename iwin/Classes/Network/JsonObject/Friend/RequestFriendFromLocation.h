#ifndef _RequestFriendFromLocation_H_
#define _RequestFriendFromLocation_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Location.h"

namespace iwinmesage
{
	class RequestFriendFromLocation
	{
		
CC_SYNTHESIZE(Location, location, Location);
CC_SYNTHESIZE(int, page, Page);

	public:
		
		RequestFriendFromLocation();
		virtual ~RequestFriendFromLocation();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
