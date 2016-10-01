#ifndef _GetMinMaxBoard_H_
#define _GetMinMaxBoard_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "Common/Common.h"
namespace iwinmesage
{
	class GetMinMaxBoard
	{
		
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(int, roomID, RoomID);
CC_SYNTHESIZE(int, boardID, BoardID);
CC_SYNTHESIZE(int, minMoney, MinMoney);
CC_SYNTHESIZE(s64, maxMoney, MaxMoney);

	public:
		
		GetMinMaxBoard();
		virtual ~GetMinMaxBoard();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
