#ifndef _LobbyChatMessage_H_
#define _LobbyChatMessage_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class LobbyChatMessage
	{
		
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(int, userId, UserId);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		LobbyChatMessage();
		virtual ~LobbyChatMessage();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
