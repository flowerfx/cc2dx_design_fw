#ifndef _CaptchaInfo_H_
#define _CaptchaInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CaptchaInfo
	{
		
CC_SYNTHESIZE(std::string, id, Id);
CC_SYNTHESIZE(std::string, image, Image);

	public:
		
		CaptchaInfo();
		virtual ~CaptchaInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
