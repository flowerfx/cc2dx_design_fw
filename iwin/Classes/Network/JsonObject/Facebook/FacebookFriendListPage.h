#ifndef _FacebookFriendListPage_H_
#define _FacebookFriendListPage_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FacebookFriendListPageCursor.h"

namespace iwinmesage
{
	class FacebookFriendListPage
	{
		
CC_SYNTHESIZE(std::string, next, Next);
CC_SYNTHESIZE(FacebookFriendListPageCursor, cursors, Cursors);

	public:
		
		FacebookFriendListPage();
		virtual ~FacebookFriendListPage();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
