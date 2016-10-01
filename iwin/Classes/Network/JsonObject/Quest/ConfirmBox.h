#ifndef _ConfirmBox_H_
#define _ConfirmBox_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ConfirmBox
	{
		
CC_SYNTHESIZE(std::string, confirmTitle, ConfirmTitle);
CC_SYNTHESIZE(std::string, confirmMsg, ConfirmMsg);
CC_SYNTHESIZE(std::string, yesButton, YesButton);
CC_SYNTHESIZE(std::string, noButton, NoButton);

	public:
		
		ConfirmBox();
		virtual ~ConfirmBox();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
