#ifndef _StockList_H_
#define _StockList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Stock.h"

namespace iwinmesage
{
	class StockList
	{
		
CC_SYNTHESIZE(std::vector<Stock>, countryList, CountryList);

	public:
		
		StockList();
		virtual ~StockList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
