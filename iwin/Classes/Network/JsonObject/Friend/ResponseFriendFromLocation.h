#ifndef _ResponseFriendFromLocation_H_
#define _ResponseFriendFromLocation_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Friend.h"

namespace iwinmesage
{
	class ResponseFriendFromLocation
	{
		
CC_SYNTHESIZE(std::vector<Friend>, friends, Friends);
CC_SYNTHESIZE(int, totalPage, TotalPage);

	public:
		
		ResponseFriendFromLocation();
		virtual ~ResponseFriendFromLocation();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
