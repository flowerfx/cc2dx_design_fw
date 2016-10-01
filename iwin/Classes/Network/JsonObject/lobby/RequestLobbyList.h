#ifndef _RequestLobbyList_H_
#define _RequestLobbyList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RequestLobbyList
	{
		
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(int, moneyType, MoneyType);

	public:
		
		RequestLobbyList();
		virtual ~RequestLobbyList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
