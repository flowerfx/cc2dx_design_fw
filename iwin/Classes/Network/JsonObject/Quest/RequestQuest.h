#ifndef _RequestQuest_H_
#define _RequestQuest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Quest.h"

namespace iwinmesage
{
	class RequestQuest
	{
		
CC_SYNTHESIZE(std::vector<Quest>, quests, Quests);

	public:
		
		RequestQuest();
		virtual ~RequestQuest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
