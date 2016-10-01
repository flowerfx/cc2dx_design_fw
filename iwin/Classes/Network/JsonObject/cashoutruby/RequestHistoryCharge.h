#ifndef _RequestHistoryCharge_H_
#define _RequestHistoryCharge_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RequestHistoryCharge
	{
		
CC_SYNTHESIZE(int, pageIndex, PageIndex);

	public:
		
		RequestHistoryCharge();
		virtual ~RequestHistoryCharge();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
