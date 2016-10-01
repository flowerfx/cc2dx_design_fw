#ifndef _FacebookFriendList_H_
#define _FacebookFriendList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FacebookFriend.h"
#include "FacebookFriendListPage.h"

namespace iwinmesage
{
	class FacebookFriendList
	{
		
CC_SYNTHESIZE(std::vector<FacebookFriend>, data, Data);
CC_SYNTHESIZE(FacebookFriendListPage, paging, Paging);

	public:
		
		FacebookFriendList();
		virtual ~FacebookFriendList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
