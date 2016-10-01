#ifndef _AvatarList_H_
#define _AvatarList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Avatar.h"

namespace iwinmesage
{
	class AvatarList
	{
		
CC_SYNTHESIZE(std::vector<Avatar*>, list, List);

	public:
		
		AvatarList();
		virtual ~AvatarList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
