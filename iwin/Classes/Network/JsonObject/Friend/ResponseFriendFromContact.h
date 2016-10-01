#ifndef _ResponseFriendFromContact_H_
#define _ResponseFriendFromContact_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FriendContact.h"

namespace iwinmesage
{
	class ResponseFriendFromContact
	{
		
CC_SYNTHESIZE(std::vector<FriendContact>, friendContacts, FriendContacts);

	public:
		
		ResponseFriendFromContact();
		virtual ~ResponseFriendFromContact();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
