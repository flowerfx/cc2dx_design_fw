#ifndef _FacebookPictureData_H_
#define _FacebookPictureData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookPictureData
	{
		
CC_SYNTHESIZE(bool, is_silhouette, Is_silhouette);
CC_SYNTHESIZE(std::string, url, Url);

	public:
		
		FacebookPictureData();
		virtual ~FacebookPictureData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
