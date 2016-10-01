#ifndef _AvatarCatList_H_
#define _AvatarCatList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "AvatarCat.h"

namespace iwinmesage
{
	class AvatarCatList
	{
		
CC_SYNTHESIZE(std::vector<AvatarCat>, list, List);

	public:
		
		AvatarCatList();
		virtual ~AvatarCatList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
