#ifndef _Friend_H_
#define _Friend_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Location.h"

namespace iwinmesage
{
	class Friend
	{
		
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(std::string, facebookId, FacebookId);
CC_SYNTHESIZE(std::string, facebookName, FacebookName);
CC_SYNTHESIZE(int64_t, money, Money);
CC_SYNTHESIZE(int, avatarId, AvatarId);
CC_SYNTHESIZE(Location, location, Location);
CC_SYNTHESIZE(int, userId, UserId);

	public:
		
		Friend();
		virtual ~Friend();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
