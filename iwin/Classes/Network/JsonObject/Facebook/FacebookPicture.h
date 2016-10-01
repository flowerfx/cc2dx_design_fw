#ifndef _FacebookPicture_H_
#define _FacebookPicture_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FacebookPictureData.h"

namespace iwinmesage
{
	class FacebookPicture
	{
		
CC_SYNTHESIZE(FacebookPictureData, data, Data);

	public:
		
		FacebookPicture();
		virtual ~FacebookPicture();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
