#ifndef _BoardOwner_H_
#define _BoardOwner_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BoardOwner
	{
		
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(int64_t, money, Money);
CC_SYNTHESIZE(int, avatar, Avatar);
CC_SYNTHESIZE(int, exp, Exp);
CC_SYNTHESIZE(int, win, Win);
CC_SYNTHESIZE(int, lose, Lose);
CC_SYNTHESIZE(int, draw, Draw);
CC_SYNTHESIZE(int, leave, Leave);
CC_SYNTHESIZE(int, roomId, RoomId);
CC_SYNTHESIZE(int, boardId, BoardId);
CC_SYNTHESIZE(bool, password, Password);
CC_SYNTHESIZE(int, boardMoney, BoardMoney);
CC_SYNTHESIZE(int, maxBet, MaxBet);
CC_SYNTHESIZE(bool, playing, Playing);
CC_SYNTHESIZE(int, countPlayer, CountPlayer);
CC_SYNTHESIZE(int, countMaxPlayer, CountMaxPlayer);
CC_SYNTHESIZE(int, boardLevel, BoardLevel);
CC_SYNTHESIZE(std::string, platform, Platform);
CC_SYNTHESIZE(std::string, formatedUsername, FormatedUsername);
CC_SYNTHESIZE(std::string, formatedMoney, FormatedMoney);
CC_SYNTHESIZE(std::string, formatedWin, FormatedWin);
CC_SYNTHESIZE(std::string, formatedBetMoney, FormatedBetMoney);

	public:
		static  std::string J2ME_NAME;
static  std::string IOS_NAME;
static  std::string ANDROID_NAME;
static  std::string DESKTOP_NAME;
static  std::string WEB_APP_NAME;
static  std::string WINDOWS_PHONE_NAME;
static  std::string UNKNOW_NAME;

		BoardOwner();
		virtual ~BoardOwner();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
