#ifndef _LobbyList_H_
#define _LobbyList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "LobbyItem.h"

namespace iwinmesage
{
	class LobbyList
	{
		
CC_SYNTHESIZE(int, moneyType, MoneyType);
CC_SYNTHESIZE(std::vector<LobbyItem*>, list, List);

	public:
		
		LobbyList();
		virtual ~LobbyList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
		LobbyItem* getLobbyItem(int idx);
	};
}
#endif
