#ifndef _GetStockDividend_H_
#define _GetStockDividend_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GetStockDividend
	{
		
CC_SYNTHESIZE(int, myStockID, MyStockID);
CC_SYNTHESIZE(int, countryID, CountryID);
CC_SYNTHESIZE(int64_t, dividend, Dividend);

	public:
		
		GetStockDividend();
		virtual ~GetStockDividend();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
