#ifndef _CaptchaResult_H_
#define _CaptchaResult_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CaptchaResult
	{
	public:
		enum class CaptchaCheckType
		{
			SUCCESS = 0,
			ERROR1 = 1,
			EXPIRED = 2,
			WRONG_ANSWER = 3
		};
	
CC_SYNTHESIZE(CaptchaCheckType, code, Code);
CC_SYNTHESIZE(std::string, desc, Desc);

	public:
		CaptchaResult();
		virtual ~CaptchaResult();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
