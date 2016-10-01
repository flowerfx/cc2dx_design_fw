#ifndef _UserRanking_H_
#define _UserRanking_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class UserRanking
	{
		
CC_SYNTHESIZE(int, index, Index);
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(int, point, Point);
CC_SYNTHESIZE(int, won, Won);
CC_SYNTHESIZE(int, lose, Lose);
CC_SYNTHESIZE(int, draw, Draw);

	public:
		
		UserRanking();
		virtual ~UserRanking();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
