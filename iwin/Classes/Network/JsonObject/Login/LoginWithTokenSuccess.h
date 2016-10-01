#ifndef _LoginWithTokenSuccess_H_
#define _LoginWithTokenSuccess_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LoginWithTokenSuccess
	{
		
CC_SYNTHESIZE(std::string, type, Type);
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, username, Username);

	public:
		
		LoginWithTokenSuccess();
		virtual ~LoginWithTokenSuccess();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
