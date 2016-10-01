#ifndef _ImageLinkContainerObject_H_
#define _ImageLinkContainerObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ImageLinkContainerObject
	{
		
CC_SYNTHESIZE(std::string, avatar_new, Avatar_new);
CC_SYNTHESIZE(std::string, items_new, Items_new);
CC_SYNTHESIZE(std::string, avatar, Avatar);
CC_SYNTHESIZE(std::string, items, Items);

	public:
		
		ImageLinkContainerObject();
		virtual ~ImageLinkContainerObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
