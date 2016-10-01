#ifndef _ScreenID_H_
#define _ScreenID_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	enum class SCREEN_ID
	{
		SCREEN_NONE = 0,
		SCREEN_MAIN_MENU = 1,
		/**
		* man hinh nap win
		* */
		SCREEN_CHARGE_MONEY = 2,
		SCREEN_AVATAR_SHOP = 3,
		// public static final int SCREEN_ROOMlIST = 4;
		// public static final int SCREEN_BOARDLIST = 5;
		SCREEN_PROFILE = 6,
		SCREEN_FRIENDLIST = 7,
		// public static final int SCREEN_BANNER = 8;
		SCREEN_RICHLIST = 9,
		SCREEN_STRONGLIST = 10,
		// public static final int SCREEN_MESSAGES = 11;
		// public static final int SCREEN_LOGOUT = 12;
		SCREEN_REGISTER_IWIN_ACCOUNT = 13,
		SCREEN_LOGIN_IWIN_ACCOUNT = 14,
		// public static final int SCREEN_PROMOTION = 15;
		SCREEN_SEARCH_FRIEND = 16,
		SCREEN_WINFREE = 17,
		SCREEN_SETTING = 18,
		SCREEN_MORE_GAME = 19,
		SCREEN_PLAY_GAME = 20,
		SCREEN_LOGOUT = 21,
		SCREEN_NEWYEAR = 22,
		SCREEN_LIXI_DOMINO = 22,
		SCREEN_RANKING_DOMINO = 23,
		SCREEN_DANG_TRONG = 24,
		SCREEN_DANG_TRONG_SHOP = 25,
		SCREEN_PICKROOM = 26,
		SCREEN_CHAT = 27,
		SCREEN_CUOCCHAUA = 28,
		SCREEN_NEW_LOBBY_ROOM = 29,
		SCREEN_NEW_LOBBY_TABLE = 30,
		SCREEN_DOI_THUONG = 31,
		SCREEN_SHOP_WIN = 32,// man hinh nap win 
		SCREEN_SHOP_RUBY = 33,// man hinh nap ruby

		SCREEN_TAIXIU = 100,
		SCREEN_TAIXIU_RUBY = 101,
	};

	class ScreenID
	{
		

	public:

		ScreenID();
		virtual ~ScreenID();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
