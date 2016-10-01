#ifndef _LuckyCircleQuestListItem_H_
#define _LuckyCircleQuestListItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LuckyCircleQuestListItem
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(int, order, Order);

	public:
		
		LuckyCircleQuestListItem();
		virtual ~LuckyCircleQuestListItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
