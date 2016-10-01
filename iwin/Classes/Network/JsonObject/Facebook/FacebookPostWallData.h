#ifndef _FacebookPostWallData_H_
#define _FacebookPostWallData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookPostWallData
	{
		
CC_SYNTHESIZE(std::string, display, Display);
CC_SYNTHESIZE(std::string, caption, Caption);
CC_SYNTHESIZE(std::string, link, Link);
CC_SYNTHESIZE(std::string, redirect_uri, Redirect_uri);

	public:
		
		FacebookPostWallData();
		virtual ~FacebookPostWallData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
