#ifndef _ResponseQuestList_H_
#define _ResponseQuestList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Quest.h"

namespace iwinmesage
{
	class ResponseQuestList
	{
		
CC_SYNTHESIZE(std::vector<Quest>, quests, Quests);
CC_SYNTHESIZE(int, categoryId, CategoryId);

	public:
		
		ResponseQuestList();
		virtual ~ResponseQuestList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
