#ifndef _PaymentResult_H_
#define _PaymentResult_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class PaymentResult
	{
		
CC_SYNTHESIZE(int, code, Code);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		PaymentResult();
		virtual ~PaymentResult();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
