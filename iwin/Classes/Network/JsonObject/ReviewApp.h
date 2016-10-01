#ifndef _ReviewApp_H_
#define _ReviewApp_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ReviewApp
	{
		
CC_SYNTHESIZE(std::string, bundle, Bundle);
CC_SYNTHESIZE(std::string, version, Version);
CC_SYNTHESIZE(int, status, Status);
CC_SYNTHESIZE(int, platform, Platform);

	public:
		static  int IOS;
static  int OS_MAC;
static  int OS_ANDROID;
static  int OS_WINDOWS;

		ReviewApp();
		virtual ~ReviewApp();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
