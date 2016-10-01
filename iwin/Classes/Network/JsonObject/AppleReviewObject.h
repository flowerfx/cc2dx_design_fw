#ifndef _AppleReviewObject_H_
#define _AppleReviewObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "ReviewApp.h"

namespace iwinmesage
{
	class AppleReviewObject
	{
		
CC_SYNTHESIZE(std::vector<ReviewApp>, reviewApps, ReviewApps);
CC_SYNTHESIZE(std::vector<ReviewApp>, disabledGPlus, DisabledGPlus);

	public:
		static  std::string LINK;

		AppleReviewObject();
		virtual ~AppleReviewObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
