#ifndef _RequestFriendFromContact_H_
#define _RequestFriendFromContact_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Contact.h"

namespace iwinmesage
{
	class RequestFriendFromContact
	{
		
CC_SYNTHESIZE(std::vector<Contact>, contacts, Contacts);

	public:
		
		RequestFriendFromContact();
		virtual ~RequestFriendFromContact();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
