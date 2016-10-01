#ifndef _ResponseFriendFromFacebook_H_
#define _ResponseFriendFromFacebook_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Friend.h"

namespace iwinmesage
{
	class ResponseFriendFromFacebook
	{
		
CC_SYNTHESIZE(std::vector<Friend>, friends, Friends);
CC_SYNTHESIZE(int, totalPage, TotalPage);

	public:
		
		ResponseFriendFromFacebook();
		virtual ~ResponseFriendFromFacebook();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
