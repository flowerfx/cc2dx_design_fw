#ifndef _GetOnwerBoardListResponse_H_
#define _GetOnwerBoardListResponse_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "BoardOwner.h"

namespace iwinmesage
{
	class GetOnwerBoardListResponse
	{
		
CC_SYNTHESIZE(std::vector<BoardOwner>, boardOwners, BoardOwners);
CC_SYNTHESIZE(int, win, Win);
CC_SYNTHESIZE(int, lose, Lose);
CC_SYNTHESIZE(int, draw, Draw);
CC_SYNTHESIZE(int, leave, Leave);

	public:
		
		GetOnwerBoardListResponse();
		virtual ~GetOnwerBoardListResponse();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
