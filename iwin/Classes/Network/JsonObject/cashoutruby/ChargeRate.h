#ifndef _ChargeRate_H_
#define _ChargeRate_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "CardTelco.h"

namespace iwinmesage
{
	class ChargeRate
	{
		
CC_SYNTHESIZE(int, telcoId, TelcoId);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::vector<Card>, cards, Cards);

	public:
		
		ChargeRate();
		virtual ~ChargeRate();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
