#ifndef _GoogleCallbackPurchaseData_H_
#define _GoogleCallbackPurchaseData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GoogleCallbackPurchaseData
	{
		
CC_SYNTHESIZE(std::string, orderId, OrderId);
CC_SYNTHESIZE(std::string, packageName, PackageName);
CC_SYNTHESIZE(std::string, productId, ProductId);
CC_SYNTHESIZE(int64_t, purchaseTime, PurchaseTime);
CC_SYNTHESIZE(int, purchaseState, PurchaseState);
CC_SYNTHESIZE(std::string, developerPayload, DeveloperPayload);
CC_SYNTHESIZE(std::string, purchaseToken, PurchaseToken);

	public:
		
		GoogleCallbackPurchaseData();
		virtual ~GoogleCallbackPurchaseData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
