#ifndef _Guest_H_
#define _Guest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Guest
	{
		
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(int, avatar, Avatar);
CC_SYNTHESIZE(int, userid, Userid);
CC_SYNTHESIZE(int64_t, money, Money);

	public:
		
		Guest();
		virtual ~Guest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
