#ifndef _PromotionItem_H_
#define _PromotionItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "../payment/ChargeType.h"
#include "ActionType.h"

namespace iwinmesage
{
	class PromotionItem
	{
		

CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, chargeType, chargeType);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, des, Des);
CC_SYNTHESIZE(std::string, actionType, actionType);
CC_SYNTHESIZE(std::string, imgUrl, ImgUrl);

	public:
		
		PromotionItem();
		virtual ~PromotionItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
