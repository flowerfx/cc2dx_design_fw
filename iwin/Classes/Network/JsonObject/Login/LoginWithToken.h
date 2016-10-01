#ifndef _LoginWithToken_H_
#define _LoginWithToken_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LoginWithToken
	{
		
CC_SYNTHESIZE(std::string, type, Type);
CC_SYNTHESIZE(std::string, token, Token);
CC_SYNTHESIZE(std::string, version, Version);

	public:
		static  std::string TYPE_FACEBOOK;
static  std::string TYPE_GOOGLE;
static  std::string TYPE_MOBO;
static  std::string TYPE_CHANGE_SERVER;
static  std::string TYPE_APPLEID;

		LoginWithToken();
		virtual ~LoginWithToken();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
