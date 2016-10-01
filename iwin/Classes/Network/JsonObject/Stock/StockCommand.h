#ifndef _StockCommand_H_
#define _StockCommand_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class StockCommand
	{
		

	public:
		static  std::string GET_STOCK_LIST;
static  std::string BUY_STOCK;
static  std::string GET_MY_STOCK_LIST;
static  std::string GET_DIVIDEND;

		StockCommand();
		virtual ~StockCommand();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
