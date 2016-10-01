#ifndef _MoboData_H_
#define _MoboData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "MoboMap.h"

namespace iwinmesage
{
	class MoboData
	{
		
CC_SYNTHESIZE(std::string, code, Code);
CC_SYNTHESIZE(std::string, accessToken, AccessToken);
CC_SYNTHESIZE(std::string, usernameIwin, UsernameIwin);
CC_SYNTHESIZE(MoboMap, moboMap, MoboMap);

	public:
		
		MoboData();
		virtual ~MoboData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
