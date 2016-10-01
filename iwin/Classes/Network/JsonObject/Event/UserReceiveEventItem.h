#ifndef _UserReceiveEventItem_H_
#define _UserReceiveEventItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class UserReceiveEventItem
	{
		
CC_SYNTHESIZE(int, userId, UserId);
CC_SYNTHESIZE(std::string, userName, UserName);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, imgUrl, ImgUrl);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		UserReceiveEventItem();
		virtual ~UserReceiveEventItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
