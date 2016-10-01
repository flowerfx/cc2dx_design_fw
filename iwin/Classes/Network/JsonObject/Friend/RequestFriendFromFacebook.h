#ifndef _RequestFriendFromFacebook_H_
#define _RequestFriendFromFacebook_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RequestFriendFromFacebook
	{
		
CC_SYNTHESIZE(std::string, accessToken, AccessToken);
CC_SYNTHESIZE(int, page, Page);

	public:
		
		RequestFriendFromFacebook();
		virtual ~RequestFriendFromFacebook();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
