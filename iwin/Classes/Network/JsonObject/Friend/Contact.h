#ifndef _Contact_H_
#define _Contact_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Contact
	{
		
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(int, contactId, ContactId);
CC_SYNTHESIZE(std::vector<std::string>, phones, Phones);
CC_SYNTHESIZE(std::vector<std::string>, emails, Emails);

	public:
		
		Contact();
		virtual ~Contact();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
