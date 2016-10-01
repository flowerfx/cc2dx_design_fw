#ifndef _AvatarCat_H_
#define _AvatarCat_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class AvatarCat
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, img, Img);

	public:
		
		AvatarCat();
		virtual ~AvatarCat();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
