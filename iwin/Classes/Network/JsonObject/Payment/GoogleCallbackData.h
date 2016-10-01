#ifndef _GoogleCallbackData_H_
#define _GoogleCallbackData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "GoogleCallbackPurchaseData.h"

namespace iwinmesage
{
	class GoogleCallbackData
	{
		
CC_SYNTHESIZE(GoogleCallbackPurchaseData, purchaseData, PurchaseData);
CC_SYNTHESIZE(std::string, dataSignature, DataSignature);

	public:
		
		GoogleCallbackData();
		virtual ~GoogleCallbackData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
