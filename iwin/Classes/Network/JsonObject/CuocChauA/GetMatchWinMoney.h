#ifndef _GetMatchWinMoney_H_
#define _GetMatchWinMoney_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GetMatchWinMoney
	{
		
CC_SYNTHESIZE(int, myMatchID, MyMatchID);
CC_SYNTHESIZE(int64_t, money, Money);

	public:
		
		GetMatchWinMoney();
		virtual ~GetMatchWinMoney();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
