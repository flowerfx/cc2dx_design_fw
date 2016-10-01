#ifndef _ChatLobbyList_H_
#define _ChatLobbyList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "ChatLobbyItem.h"

namespace iwinmesage
{
	class ChatLobbyList
	{
		
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(std::vector<ChatLobbyItem*>, chatList, ChatList);
CC_SYNTHESIZE(int, isRequest, IsRequest);

	public:
		
		ChatLobbyList();
		virtual ~ChatLobbyList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
