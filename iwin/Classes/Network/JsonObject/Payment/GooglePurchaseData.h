#ifndef _GooglePurchaseData_H_
#define _GooglePurchaseData_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GooglePurchaseData
	{
		
CC_SYNTHESIZE(std::string, productId, ProductId);
CC_SYNTHESIZE(std::string, developerPayload, DeveloperPayload);

	public:
		
		GooglePurchaseData();
		virtual ~GooglePurchaseData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
