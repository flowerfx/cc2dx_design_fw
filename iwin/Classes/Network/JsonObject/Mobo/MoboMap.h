#ifndef _MoboMap_H_
#define _MoboMap_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "IwinUser.h"

namespace iwinmesage
{
	class MoboMap
	{
		
CC_SYNTHESIZE(std::vector<IwinUser>, userList, UserList);
CC_SYNTHESIZE(std::string, phone, Phone);

	public:
		
		MoboMap();
		virtual ~MoboMap();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
