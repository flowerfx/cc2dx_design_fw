#ifndef _FacebookFriend_H_
#define _FacebookFriend_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FacebookPicture.h"

namespace iwinmesage
{
	class FacebookFriend
	{
		
CC_SYNTHESIZE(std::string, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(FacebookPicture, picture, Picture);
CC_SYNTHESIZE(bool, selected, Selected);

	public:
		
		FacebookFriend();
		virtual ~FacebookFriend();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
