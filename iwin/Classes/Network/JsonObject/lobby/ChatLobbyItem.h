#ifndef _ChatLobbyItem_H_
#define _ChatLobbyItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ChatLobbyItem
	{
		
CC_SYNTHESIZE(int, userId, UserId);
CC_SYNTHESIZE(std::string, userName, UserName);
CC_SYNTHESIZE(int, userType, UserType);
CC_SYNTHESIZE(std::string, message, Message);
CC_SYNTHESIZE(int64_t, chatTime, ChatTime);

	public:
		
		ChatLobbyItem();
		virtual ~ChatLobbyItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
