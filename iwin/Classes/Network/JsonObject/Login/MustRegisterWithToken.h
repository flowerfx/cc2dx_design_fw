#ifndef _MustRegisterWithToken_H_
#define _MustRegisterWithToken_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class MustRegisterWithToken
	{
		
CC_SYNTHESIZE(std::string, type, Type);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		MustRegisterWithToken();
		virtual ~MustRegisterWithToken();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
