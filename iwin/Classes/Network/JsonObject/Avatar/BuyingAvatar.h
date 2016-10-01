#ifndef _BuyingAvatar_H_
#define _BuyingAvatar_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BuyingAvatar
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(bool, isSucceed, IsSucceed);
CC_SYNTHESIZE(std::string, message, Message);
CC_SYNTHESIZE(int64_t, money, Money);

	public:
		
		BuyingAvatar();
		virtual ~BuyingAvatar();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
