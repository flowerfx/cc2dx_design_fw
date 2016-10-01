#ifndef _PaymentInfo_H_
#define _PaymentInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class PaymentInfo
	{
		
CC_SYNTHESIZE(int, cardId, CardId);
CC_SYNTHESIZE(int, vnd, Vnd);
CC_SYNTHESIZE(int, amount, Amount);

	public:
		
		PaymentInfo();
		virtual ~PaymentInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
