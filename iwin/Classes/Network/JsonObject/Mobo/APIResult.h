#ifndef _APIResult_H_
#define _APIResult_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class APIResult
	{
		
CC_SYNTHESIZE(int, code, Code);
CC_SYNTHESIZE(std::string, desc, Desc);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		APIResult();
		virtual ~APIResult();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
