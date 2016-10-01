#ifndef _ServerControlButtonObject_H_
#define _ServerControlButtonObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "../payment/Payment.h"

namespace iwinmesage
{
	class ServerControlButtonObject
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(Payment, payment, Payment);
CC_SYNTHESIZE(std::string, img, Img);

	public:
		
		ServerControlButtonObject();
		virtual ~ServerControlButtonObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
