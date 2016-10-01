#ifndef _MyStockList_H_
#define _MyStockList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "MyStock.h"

namespace iwinmesage
{
	class MyStockList
	{
		
CC_SYNTHESIZE(std::vector<MyStock>, stockList, StockList);

	public:
		
		MyStockList();
		virtual ~MyStockList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
