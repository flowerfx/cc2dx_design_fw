#ifndef _FacebookMyInfo_H_
#define _FacebookMyInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookMyInfo
	{
		
CC_SYNTHESIZE(std::string, id, Id);
CC_SYNTHESIZE(std::string, email, Email);
CC_SYNTHESIZE(std::string, first_name, First_name);
CC_SYNTHESIZE(std::string, gender, Gender);
CC_SYNTHESIZE(std::string, last_name, Last_name);
CC_SYNTHESIZE(std::string, link, Link);
CC_SYNTHESIZE(std::string, locale, Locale);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, timezone, Timezone);
CC_SYNTHESIZE(std::string, updated_time, Updated_time);
CC_SYNTHESIZE(bool, verified, Verified);

	public:
		
		FacebookMyInfo();
		virtual ~FacebookMyInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
