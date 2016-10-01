#ifndef _GetOwnerBoardListRequest_H_
#define _GetOwnerBoardListRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GetOwnerBoardListRequest
	{
		
CC_SYNTHESIZE(int, win, Win);

	public:
		
		GetOwnerBoardListRequest();
		virtual ~GetOwnerBoardListRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
