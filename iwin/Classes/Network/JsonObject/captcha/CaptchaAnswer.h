#ifndef _CaptchaAnswer_H_
#define _CaptchaAnswer_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CaptchaAnswer
	{
		
CC_SYNTHESIZE(std::string, id, Id);
CC_SYNTHESIZE(std::string, answer, Answer);

	public:
		
		CaptchaAnswer();
		virtual ~CaptchaAnswer();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
