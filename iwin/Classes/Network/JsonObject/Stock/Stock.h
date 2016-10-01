#ifndef _Stock_H_
#define _Stock_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Stock
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(double, stockIndex, StockIndex);
CC_SYNTHESIZE(int64_t, pricePerStock, PricePerStock);
CC_SYNTHESIZE(std::string, imgLink, ImgLink);
CC_SYNTHESIZE(int, difference, Difference);
CC_SYNTHESIZE(bool, canBuy, CanBuy);

	public:
		
		Stock();
		virtual ~Stock();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
