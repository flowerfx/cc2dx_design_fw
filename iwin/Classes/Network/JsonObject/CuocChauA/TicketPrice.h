#ifndef _TicketPrice_H_
#define _TicketPrice_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class TicketPrice
	{
		
CC_SYNTHESIZE(int64_t, price, Price);

	public:
		
		TicketPrice();
		virtual ~TicketPrice();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
