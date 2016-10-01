#ifndef _FriendContact_H_
#define _FriendContact_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Friend.h"

namespace iwinmesage
{
	class FriendContact
	{
		
CC_SYNTHESIZE(std::string, phoneNumber, PhoneNumber);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::vector<Friend>, friends, Friends);

	public:
		
		FriendContact();
		virtual ~FriendContact();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
