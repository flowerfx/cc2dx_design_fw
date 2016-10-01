#ifndef _CancelGetCard_H_
#define _CancelGetCard_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CancelGetCard
	{
		
CC_SYNTHESIZE(int, logId, LogId);

	public:
		
		CancelGetCard();
		virtual ~CancelGetCard();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
