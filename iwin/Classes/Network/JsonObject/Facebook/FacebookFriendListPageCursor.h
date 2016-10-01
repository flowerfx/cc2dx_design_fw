#ifndef _FacebookFriendListPageCursor_H_
#define _FacebookFriendListPageCursor_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookFriendListPageCursor
	{
		
CC_SYNTHESIZE(std::string, before, Before);
CC_SYNTHESIZE(std::string, after, After);

	public:
		
		FacebookFriendListPageCursor();
		virtual ~FacebookFriendListPageCursor();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
