#ifndef _RegisterWithToken_H_
#define _RegisterWithToken_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RegisterWithToken
	{
		
CC_SYNTHESIZE(std::string, type, Type);
CC_SYNTHESIZE(std::string, token, Token);
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(std::string, pass, Pass);
CC_SYNTHESIZE(std::string, phone, Phone);
CC_SYNTHESIZE(std::string, version, Version);

	public:
		
		RegisterWithToken();
		virtual ~RegisterWithToken();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
