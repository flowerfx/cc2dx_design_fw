#ifndef _BuyingStock_H_
#define _BuyingStock_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BuyingStock
	{
		
CC_SYNTHESIZE(int, numberOfStock, NumberOfStock);
CC_SYNTHESIZE(int, countryID, CountryID);
CC_SYNTHESIZE(bool, isSucceed, IsSucceed);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		BuyingStock();
		virtual ~BuyingStock();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
