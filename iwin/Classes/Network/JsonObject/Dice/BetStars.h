#ifndef _BetStars_H_
#define _BetStars_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BetStars
	{
		
CC_SYNTHESIZE(int64_t, numStars, NumStars);
CC_SYNTHESIZE(int64_t, win, Win);
CC_SYNTHESIZE(std::vector<int>, dices, Dices);
CC_SYNTHESIZE(std::string, message, Message);
CC_SYNTHESIZE(bool, isError, IsError);
CC_SYNTHESIZE(int64_t, myStars, MyStars);

	public:
		
		BetStars();
		virtual ~BetStars();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
