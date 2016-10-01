#ifndef _DoPlayNow_H_
#define _DoPlayNow_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class DoPlayNow
	{
		
CC_SYNTHESIZE(int, moneyType, MoneyType);
CC_SYNTHESIZE(unsigned int, money, Money);

	public:
		
		DoPlayNow();
		virtual ~DoPlayNow();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
