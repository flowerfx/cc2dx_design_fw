#ifndef _PaymentHistoryList_H_
#define _PaymentHistoryList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "PaymentHistory.h"

namespace iwinmesage
{
	class PaymentHistoryList
	{
		
CC_SYNTHESIZE(std::vector<PaymentHistory>, historyList, HistoryList);

	public:
		
		PaymentHistoryList();
		virtual ~PaymentHistoryList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
