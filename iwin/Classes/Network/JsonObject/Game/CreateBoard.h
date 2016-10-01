#ifndef _CreateBoard_H_
#define _CreateBoard_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CreateBoard
	{
		
CC_SYNTHESIZE(int, money, Money);
CC_SYNTHESIZE(int, maxPlayer, MaxPlayer);
CC_SYNTHESIZE(std::string, password, Password);

	public:
		
		CreateBoard();
		virtual ~CreateBoard();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
