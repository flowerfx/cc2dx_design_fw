#ifndef _UserInfo_H_
#define _UserInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class UserInfo
	{
		
CC_SYNTHESIZE(std::string, fullname, Fullname);
CC_SYNTHESIZE(std::string, phone, Phone);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		UserInfo();
		virtual ~UserInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
