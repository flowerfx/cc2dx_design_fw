#ifndef __TAI_XIU_MONEY_OBJECT_H__
#define __TAI_XIU_MONEY_OBJECT_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class MoneyObject
	{
		CC_SYNTHESIZE(s64, moneyLong, MoneyLong);
		CC_SYNTHESIZE(std::string, moneyShort, MoneyShort);
	public:
		MoneyObject();
		MoneyObject(s64 moneyLong, std::string moneyShort);
		virtual ~MoneyObject();

		void setMoneyObject(s64 moneyLong, std::string moneyShort);

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TAI_XIU_MONEY_OBJECT_H__