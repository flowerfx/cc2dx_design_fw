#ifndef _InvalidLoginToken_H_
#define _InvalidLoginToken_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class InvalidLoginToken
	{
		
CC_SYNTHESIZE(std::string, type, Type);
CC_SYNTHESIZE(std::string, reason, Reason);

	public:
		
		InvalidLoginToken();
		virtual ~InvalidLoginToken();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
