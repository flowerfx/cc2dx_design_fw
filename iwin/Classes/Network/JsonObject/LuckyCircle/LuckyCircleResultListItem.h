#ifndef _LuckyCircleResultListItem_H_
#define _LuckyCircleResultListItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LuckyCircleResultListItem
	{
		
CC_SYNTHESIZE(int, itemId, ItemId);
CC_SYNTHESIZE(std::string, giftName, GiftName);
CC_SYNTHESIZE(int, numberOfItem, NumberOfItem);

	public:
		
		LuckyCircleResultListItem();
		virtual ~LuckyCircleResultListItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
