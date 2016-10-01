#ifndef _GameEventList_H_
#define _GameEventList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "GameEvent.h"

namespace iwinmesage
{
	class GameEventList
	{
		
CC_SYNTHESIZE(std::vector<GameEvent>, list, List);

	public:
		
		GameEventList();
		virtual ~GameEventList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
