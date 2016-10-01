#ifndef _MyStock_H_
#define _MyStock_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Stock.h"

namespace iwinmesage
{
	class MyStock
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(Stock, stock, Stock);
CC_SYNTHESIZE(int, numberOfStock, NumberOfStock);
CC_SYNTHESIZE(double, stockIndex, StockIndex);
CC_SYNTHESIZE(int64_t, time, Time);
CC_SYNTHESIZE(int64_t, price, Price);
CC_SYNTHESIZE(int64_t, dividend, Dividend);
CC_SYNTHESIZE(bool, canGetDividend, CanGetDividend);

	public:
		
		MyStock();
		virtual ~MyStock();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
