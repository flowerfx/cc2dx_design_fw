#ifndef _PromotionGetWin_H_
#define _PromotionGetWin_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class PromotionGetWin
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(bool, isSuccess, IsSuccess);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		PromotionGetWin();
		virtual ~PromotionGetWin();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
