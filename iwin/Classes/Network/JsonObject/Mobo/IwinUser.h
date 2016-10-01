#ifndef _IwinUser_H_
#define _IwinUser_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class IwinUser
	{
		
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(int, userid, Userid);
CC_SYNTHESIZE(int64_t, win, Win);

	public:
		
		IwinUser();
		virtual ~IwinUser();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
