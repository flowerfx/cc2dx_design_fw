#ifndef _LobbyItem_H_
#define _LobbyItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LobbyItem
	{
		
CC_SYNTHESIZE(int, moneyType, MoneyType);
CC_SYNTHESIZE(int, boardsWaitingCount, BoardsWaitingCount);
CC_SYNTHESIZE(int, usersWaitingCount, UsersWaitingCount);
CC_SYNTHESIZE(int64_t, money, Money);
CC_SYNTHESIZE(bool, isEnable, IsEnable);

	public:
		
		LobbyItem();
		virtual ~LobbyItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
