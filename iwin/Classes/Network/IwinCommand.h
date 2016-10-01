#ifndef IWINCOMMAND_H
#define IWINCOMMAND_H

#include <iostream>
#include <iomanip>
#include <type_traits>
#include <cstdint>
#pragma region Port string to long




#pragma endregion
namespace IwinCommand
{
	/**
	* @see RequestFriendFromFacebook info of request object
	* @see ResponseFriendFromFacebook info of response object
	*/
	static std::string REQUEST_FRIEND_FROM_FACEBOOK = "REQUEST_FRIEND_FROM_FACEBOOK";
	/**
	* @see RequestFriendFromLocation info of request object
	* @see ResponseFriendFromLocation info of response object
	*/
	static std::string REQUEST_FRIEND_FROM_LOCATION = "REQUEST_FRIEND_FROM_LOCATION";
	static std::string	REQUEST_FRIEND_FROM_SHAKE = "REQUEST_FRIEND_FROM_SHAKE";
	/**
	* @see RequestFriendFromContact info of request object
	* @see ResponseFriendFromContact info of response object
	*/
	static std::string REQUEST_FRIEND_FROM_CONTACT = "REQUEST_FRIEND_FROM_CONTACT";
	/**
	* @see RequestFacebookRequest info of request object
	* @see ResponseFacebookRequest info of response object
	*/
	static std::string	FACEBOOK_REQUEST = "FACEBOOK_REQUEST";
	/**
	* @see OpenFacebookRequest info of request object
	* @see ResponseFacebookRequest info of response object
	*/
	static std::string OPEN_FACEBOOK_REQUEST = "OPEN_FACEBOOK_REQUEST";
	/**
	* Láº¥y danh sÃ¡ch báº¡n bÃ¨ hiá»‡n táº¡i.
	*/
	static std::string FRIEND_LIST = "FRIEND_LIST";
	/**
	* thÃªm 1 ngÆ°á»�i báº¡n vÃ o danh sÃ¡ch báº¡n bÃ¨. Tráº£ vá»� danh sÃ¡ch báº¡n bÃ¨ má»›i.
	*
	* @see
	*/
	static std::string FRIEND_REQUEST = "FRIEND_REQUEST";
	/**
	* tÃ¬m 1 ngÆ°á»�i báº¡n vá»›i 1 tÃªn nÃ o Ä‘Ã³.
	*/
	static std::string FRIEND_FIND = "FRIEND_FIND";
	/**
	* Ä�á»“ng Ã½/Deny káº¿t báº¡n vá»›i 1 user nÃ o Ä‘Ã³.
	*/
	static std::string FRIEND_RESPONSE = "FRIEND_RESPONSE";
	/**
	* XÃ³a 1 ngÆ°á»�i báº¡n khá»�i danh sÃ¡ch.
	*/
	static std::string FRIEND_DELETE_FRIEND = "FRIEND_DELETE_FRIEND";
	/**
	* Chat vá»›i 1 ngÆ°á»�i báº¡n.
	*/
	static std::string FRIEND_CHAT = "FRIEND_CHAT";

	/*
	* ----------------- GAME ---------------------------------------------
	*/
	static std::string CREATE_BOARD = "CREATE_BOARD";
	static std::string GET_MIN_MAX_BOARD = "GET_MIN_MAX_BOARD";
	static std::string REGISTER_SUCCESS = "REGISTER_SUCCESS";
	static std::string GET_OWNER_BOARD_LIST = "GET_OWNER_BOARD_LIST";
	/**
	* client gá»­i lÃªn Ä‘á»ƒ ghi nháº­n tráº¡ng thÃ¡i game (pause/resume)
	*/
	static std::string GAME_STATE = "GAME_STATE";
	/**
	* láº¥y danh sÃ¡ch game phÃ¢n theo loáº¡i
	*/
	static std::string GET_GAME_LIST = "GET_GAME_LIST";

	/*
	* ----------------- INFORMATION
	* ---------------------------------------------
	*/
	static std::string ERROR_COMMAND = "ERROR_COMMAND";
	/**
	* @see ResponseQuestList
	*/
	static std::string GET_QUEST_LIST = "GET_QUEST_LIST";
	/**
	* @see RequestSubmitQuest
	*/
	static std::string SUBMIT_QUEST = "SUBMIT_QUEST";
	static std::string ACCEPT_QUEST = "ACCEPT_QUEST";

	/**
	* láº¥y danh sÃ¡ch category trong mÃ n hÃ¬nh khuyáº¿n
	*/
	static std::string GET_QUEST_CATEGORY_LIST = "GET_QUEST_CATEGORY_LIST";

	/**
	* láº¥y danh sÃ¡ch sá»± kiá»‡n trong category dá»±a theo ID
	*/
	static std::string GET_QUESTS_BY_CATEGORY_ID = "GET_QUESTS_BY_CATEGORY_ID";

	/**
	* {@link RegisterInfor}
	*/
	static std::string REGISTER_INFOR = "REGISTER_INFOR";

	/*
	* ----------------- MOBO ---------------------------------------------
	*/
	static std::string LIST_USER_BY_CODE = "LIST_USER_BY_CODE";
	static std::string LIST_USER_BY_ACCESS_TOKEN = "LIST_USER_BY_ACCESS_TOKEN";
	static std::string ACCESS_TOKEN_FAIL = "ACCESS_TOKEN_FAIL";
	static std::string CREATE_USER_BY_MOBO_ACCOUNT = "CREATE_USER_BY_MOBO_ACCOUNT";
	static std::string LOGIN_BY_MOBO_ACCOUNT = "LOGIN_BY_MOBO_ACCOUNT";

	/*
	* ----------------- PAYMENT ---------------------------------------------
	*/
	static std::string REQUEST_PAYMENT_INFOR = "REQUEST_PAYMENT_INFOR";
	static std::string GET_LOBBY_LIST = "GET_LOBBY_LIST";
	static std::string DO_PLAY_NOW = "DO_PLAY_NOW";
	static std::string GET_BOARD_FROM_LOBBYID = "GET_BOARD_FROM_LOBBYID";
	static std::string SEND_GLOBAL_CHAT = "SEND_GLOBAL_CHAT";
	static std::string GET_GLOBAL_CHAT_LIST = "GET_GLOBAL_CHAT_LIST";

	/*
	* ----------------- SMARTPHONE
	* ---------------------------------------------
	*/
	/**
	* gui thong tin client infor cua smartphone.<br>
	* {"command":"SET_CLIENT_INFOR";"data":{"providerID":1,"refcode":"r";
	* "subProvider"
	* :1,"version":"vs";"phone":"p";"lang":"l";"os":"os";"device":"d"
	* ,"osVersion":"os";"carrier":"c";"net":"n";"isJaibreak":1}}
	*
	* @see ClientInforObject
	*/
	static std::string SET_CLIENT_INFOR = "SET_CLIENT_INFOR";
	static std::string GET_BOARD_MIN_MAX = "GET_BOARD_MIN_MAX";
	/**
	* láº¥y file zip gui cho báº£n smartphone
	*/
	static std::string GET_GUI = "GET_GUI";
	/**
	* bat popup dieu huong den 1 tinh nang
	*/
	static std::string REQUEST_FEATURE = "REQUEST_FEATURE";

	/*
	* ----------------- DICE GAME ---------------------------------------------
	*/
	/**
	* láº¥y sao cá»§a tÃ´i , server tráº£ vá»� Integer sá»‘ sao
	*/
	static std::string GET_MY_STARS = "GET_MY_STARS";
	/**
	* Ä‘áº·t sao
	*/
	static std::string BET_STARS = "BET_STARS";

	/*
	* ----------------- AVATAR ---------------------------------------------
	*/
	/**
	* Get Avatar category.
	*/
	static std::string GET_AVA_CAT = "GET_AVA_CAT";

	/**
	* Lay danh sach avatar theo category.
	*/
	static std::string GET_AVA = "GET_AVA";

	/**
	* mua Avatar
	*/
	static std::string BUY_AVA = "BUY_AVA";

	/*
	* ----------------- GAME ---------------------------------------------
	*/
	/**
	* bat dau game
	*/
	static std::string START = "START";
	/**
	* danh 1 quan
	*/
	static std::string FIRE = "FIRE";
	/**
	* rut 1 quan
	*/
	static std::string GET = "GET";
	/**
	* ket thuc game
	*/
	static std::string STOP = "STOP";
	/**
	* vao ban dang choi
	*/
	static std::string VIEW = "VIEW";
	/**
	* reconnect vao game
	*/
	static std::string RETURN_GAME = "RETURN_GAME";
	/**
	* loi trong game
	*/
	//static std::string ERROR = "ERROR";
	/**
	* tu dong danh
	*/
	static std::string AUTO_FIRE = "AUTO_FIRE";
	/**
	* chuyen luot trong game
	*/
	static std::string NEXT_TURN = "NEXT_TURN";
	/**
	* Bo luot trong game
	*/
	static std::string SKIP = "SKIP";
	/**
	* thu há»“i láº¡i cá»� cá»§a user
	*/
	static std::string RECOVERY = "RECOVERY";

	/*
	* ----------------- HALLOWEEN ---------------------------------------------
	*/
	/**
	* nhan duoc mot item
	*/
	static std::string HALLOWEEN_RECEIVED_ITEM = "HALLOWEEN_RECEIVED_ITEM";
	/**
	* lay thong tin nhiem vu
	*/
	static std::string HALLOWEEN_GET_QUEST = "HALLOWEEN_GET_QUEST";
	/**
	* lay danh sach cac nhiem vu
	*/
	static std::string HALLOWEEN_GET_WAND_LIST = "HALLOWEEN_GET_WAND_LIST";
	/**
	* nhan phan thuong
	*/
	static std::string HALLOWEEN_RECEIVE_QUEST = "HALLOWEEN_RECEIVE_QUEST";
	/**
	* nhan phan thuong
	*/
	static std::string HALLOWEEN_COMPLETE_QUEST = "HALLOWEEN_COMPLETE_QUEST";
	/**
	* nhan phan thuong
	*/
	static std::string HALLOWEEN_GET_REWARD = "HALLOWEEN_GET_REWARD";
	/**
	* tra ve nhan hoac tra quest co thanh cong hay khong
	*/
	static std::string HALLOWEEN_QUEST_RESULT = "HALLOWEEN_QUEST_RESULT";

	/*
	* ----------------- QUOC KHANH
	* ---------------------------------------------
	*/
	/**
	* nháº­n Ä‘Æ°á»£c lÃ¡ cá»�
	*/
	static std::string INDEPENDENCE_RECEIVED_FLAG = "INDEPENDENCE_RECEIVED_FLAG";
	/**
	* nháº­n win thÆ°á»Ÿng
	*/
	static std::string INDEPENDENCE_GET_WIN = "INDEPENDENCE_GET_WIN";
	/**
	* nháº­n thÃ´ng tin cá»� cá»§a mÃ¬nh
	*/
	static std::string INDEPENDENCE_GET_MY_FLAGS = "INDEPENDENCE_GET_MY_FLAGS";

	/*
	* ----------------- LOGIN ---------------------------------------------
	*/
	/**
	* dang nhap bang access token
	*/
	static std::string LOGIN_WITH_TOKEN = "LOGIN_WITH_TOKEN";
	/**
	* token sai hoac het han
	*/
	static std::string INVALID_LOGIN_TOKEN = "INVALID_LOGIN_TOKEN";
	/**
	* chua tao tai khoan iwin voi token nay => phai tao
	*
	*/
	static std::string MUST_REGISTER_WITH_TOKEN = "MUST_REGISTER_WITH_TOKEN";
	/**
	* tao tai khoan iwin bang access token
	*/
	static std::string REGISTER_WITH_TOKEN = "REGISTER_WITH_TOKEN";
	/**
	* dang nhap bang access token thanh cong
	*/
	static std::string LOGIN_WITH_TOKEN_SUCCESS = "LOGIN_WITH_TOKEN_SUCCESS";

	/**
	* tra ve khi dang nhap bang guest thanh cong
	*/
	static std::string LOGIN_AS_GUEST_SUCCESS = "LOGIN_AS_GUEST_SUCCESS";
	/**
	* dang nhap bang tai khoan khach
	*/
	static std::string LOGIN_AS_GUEST = "LOGIN_AS_GUEST";
	/**
	* tra ve khi can dang nhap bang tai khoan chinh thuc
	*/
	static std::string REQUIRE_LOGIN = "REQUIRE_LOGIN";

	/*
	* ----------------- TRUNG THU ---------------------------------------------
	*/
	/**
	* nhan duoc mot item (long den, banh, hop banh)
	*/
	static std::string RECEIVED_MOONFESTIVAL_ITEM = "RECEIVED_MOONFESTIVAL_ITEM";
	/**
	* lay danh sach cac nhiem vu thu thap banh
	*/
	static std::string GET_CAKE_TASKS = "GET_CAKE_TASKS";
	/**
	* lay thong tin nhiem vu long den
	*/
	static std::string GET_LANTERN_TASK = "GET_LANTERN_TASK";
	/**
	* nhan win nhiem vu long den
	*/
	static std::string GET_WIN_LANTERN_TASK = "GET_WIN_LANTERN_TASK";
	/**
	* nhan win nhiem vu thu thap banh
	*/
	static std::string GET_WIN_CAKE_TASK = "GET_WIN_CAKE_TASK";

	/*
	* ----------------- CUOC CHAP ---------------------------------------------
	*/
	/**
	* Láº¥y danh sÃ¡ch cÃ¡c giáº£i Ä‘áº¥u
	*/
	static std::string GET_LEAGUE_LIST = "GET_LEAGUE_LIST";
	/**
	* Láº¥y thÃ´ng tin cá»§a cÃ¡c tráº­n cÆ°á»£c.
	*/
	static std::string GET_MATCH_LIST = "GET_MATCH_LIST";

	/**
	* Dat cÆ°á»£c cho Ä‘á»™i nÃ o Ä‘Ã³.
	*/
	static std::string BET_TEAM = "BET_TEAM";

	/**
	* Láº¥y danh sÃ¡ch cÃ¡c tráº­n Ä‘áº§u mÃ¬nh cÃ¡ cÆ°á»£c.
	*/
	static std::string GET_MY_MATCH_LIST = "GET_MY_MATCH_LIST";

	/**
	* Láº¥y tiá»�n tháº¯ng cá»§a tráº­n.
	*/
	static std::string GET_WIN = "GET_WIN";

	/**
	* láº¥y giÃ¡ trá»‹ cá»§a 1 vÃ©, Ä‘á»ƒ Ä‘áº·t Ä‘Æ°á»£c
	*/
	static std::string GET_TICKET_PRICE = "GET_TICKET_PRICE";
	/*
	* ------------------------------------ Giáº£i Ä‘áº¥u cá»� Ãºp ----------------------------------------
	*/
	/**
	* Láº¥y danh sÃ¡ch cÃ¡c giáº£i Ä‘áº¥u
	*/
	static std::string GET_HIDDEN_CHESS_LEAGUE_LIST = "GET_HIDDEN_CHESS_LEAGUE_LIST";
	/**
	* Láº¥y thÃ´ng tin cá»§a cÃ¡c tráº­n cÆ°á»£c.
	*/
	static std::string GET_HIDDEN_CHESS_MATCH_LIST = "GET_HIDDEN_CHESS_MATCH_LIST";

	/**
	* Dat cÆ°á»£c cho Ä‘á»™i nÃ o Ä‘Ã³.
	*/
	static std::string BET_HIDDEN_CHESS_TEAM = "BET_HIDDEN_CHESS_TEAM";

	/**
	* Láº¥y danh sÃ¡ch cÃ¡c tráº­n Ä‘áº§u mÃ¬nh cÃ¡ cÆ°á»£c.
	*/
	static std::string GET_HIDDEN_CHESS_MY_MATCH_LIST = "GET_HIDDEN_CHESS_MY_MATCH_LIST";

	/**
	* Láº¥y tiá»�n tháº¯ng cá»§a tráº­n.
	*/
	static std::string GET_HIDDEN_CHESS_WIN = "GET_HIDDEN_CHESS_WIN";

	/**
	* láº¥y giÃ¡ trá»‹ cá»§a 1 vÃ©, Ä‘á»ƒ Ä‘áº·t Ä‘Æ°á»£c
	*/
	static std::string GET_HIDDEN_CHESS_TICKET_PRICE = "GET_HIDDEN_CHESS_TICKET_PRICE";

	/**
	* láº¥y thÃ´ng tin xáº¿p háº¡ng
	*/

	static std::string GET_HIDDEN_CHESS_RANKING = "GET_HIDDEN_CHESS_RANKING";

	/**
	* láº¥y thÃ´ng tin giáº£i Ä‘áº¥u
	*/

	static std::string GET_HIDDEN_CHESS_INFO = "GET_HIDDEN_CHESS_INFO";

	/**
	* láº¥y thÃ´ng 1 tráº­n Ä‘áº¥u
	*/

	static std::string GET_HIDDEN_CHESS_MATCH_INFO = "GET_HIDDEN_CHESS_MATCH_INFO";

	/**
	* TÃ¬m tráº­n Ä‘áº¥u
	*/

	static std::string FIND_HIDDEN_CHESS_MATCH = "FIND_HIDDEN_CHESS_MATCH";

	/**
	* Ä�á»•i server
	*/

	static std::string CHANGE_SERVER = "CHANGE_SERVER";

	/**
	* Show thÃ´ng tin Ä‘áº¿m ngÆ°á»£c
	*/

	static std::string COUNTDOWN_TIME = "COUNTDOWN_TIME";


	/*
	* -------------- TÃ­nh nÄƒng Æ°u Ä‘Ã£i cho user
	*/
	/**
	* láº¥y danh sÃ¡ch cÃ¡c Æ°u Ä‘Ã£i
	*/
	static std::string PROMOTION_GET_LIST = "PROMOTION_GET_LIST";
	/**
	* nháº­n thÆ°á»Ÿng
	*/
	static std::string PROMOTION_GET_WIN = "PROMOTION_GET_WIN";

	/**
	* ------------ Sá»± kiá»‡n phÃºc lá»™c thá»�
	*/
	/**
	* Láº¥y thÃ´ng tin vá»� Sá»± kiá»‡n Ä‘á»‘t phÃ¡o
	*/
	static std::string NEWYEAR_FIRECRACKERS_INFO = "NEWYEAR_FIRECRACKERS_INFO";
	/**
	* Báº¯t Ä‘áº§u Ä‘á»‘t phÃ¡o
	*/
	static std::string NEWYEAR_FIRECRACKERS_START = "NEWYEAR_FIRECRACKERS_START";
	/**
	* Ä�á»‘t phÃ¡o xong
	*/
	static std::string NEWYEAR_FIRECRACKERS_DONE = "NEWYEAR_FIRECRACKERS_DONE";
	/**
	* ThoÃ¡t ngang khi Ä‘á»‘t phÃ¡o
	*/
	static std::string NEWYEAR_FIRECRACKERS_QUIT = "NEWYEAR_FIRECRACKERS_QUIT";
	/**
	* nhan duoc mot item
	*/
	static std::string RECEIVED_ITEM = "RECEIVED_ITEM";
	/**
	* thÃ´ng tin hiá»‡n táº¡i vá»� event
	*/
	static std::string GET_EVENT_INFO = "GET_EVENT_INFO";
	/**
	* sá»­ dá»¥ng item
	*/
	static std::string USE_ITEM = "USE_ITEM";
	/*
	* ------------------- Sá»± kiá»‡n Ä‘ua top domino
	*/
	/**
	* láº¥y thÃ´ng tin Ä‘ua top domino
	*/
	static std::string DOMINO_RANKING_INFO = "DOMINO_RANKING_INFO";
	/**
	* láº¥y win Ä‘ua top
	*/
	static std::string GET_WIN_RANKING = "GET_WIN_RANKING";
	/**
	* láº¥y thÃ´ng tin event cá»§a cÃ¡c game
	*/
	static std::string GET_GAME_EVENTS = "GET_GAME_EVENTS";
	/**
	* nÃ¢ng cáº¥p item
	*/
	static std::string UPGRAGE_ITEM = "UPGRAGE_ITEM";
	/**
	* thÃ´ng tin sticker
	*/
	static std::string GET_STICKER_INFO = "GET_STICKER_INFO";
	/**
	* thÃ´ng tin shop váº­t pháº©m
	*/
	static std::string REQUEST_PAYMENT_SHOP = "REQUEST_PAYMENT_SHOP";
	/**
	* thÃ´ng tin user gá»­i lÃªn cho server khi tham gia sá»± kiá»‡n
	*/
	static std::string EVENT_USER_INFO = "EVENT_USER_INFO";
	/**
	* láº¥y danh sÃ¡ch cÃ¡c váº­t pháº©m sá»± kiá»‡n cÃ³ bÃ¡n
	*/
	static std::string GET_SHOP_ID = "GET_SHOP_ID";
	/**
	* thá»±c hiá»‡n mua váº­t pháº©m sá»± kiá»‡n báº±ng win
	*/
	static std::string BUY_ITEM = "BUY_ITEM";
	/**
	* Show Dialog duoi client.
	*/
	static std::string CLIENT_DIALOG = "CLIENT_DIALOG";
	static std::string CLIENT_DIALOG_SUBMIT = "COUNT_QUEST_LIST";
	/**
	* láº¥y sá»‘ lÆ°á»£ng quest Ä‘ang cÃ³
	*/
	static std::string COUNT_QUEST_LIST = "COUNT_QUEST_LIST";
	/**
	* danh sÃ¡ch báº¡n facebook Ä‘Ã£ má»�i.
	*/
	static std::string INVITE_FACEBOOK = "GET_LUCKY_CIRCLE_INFO";
	/**
	* Láº¥y thÃ´ng tin LuckyCircle
	*/
	static std::string GET_LUCKY_CIRCLE_INFO = "GET_LUCKY_CIRCLE_INFO";
	/**
	* Quay LuckyCircle
	*/
	static std::string TURNING_LUCKY_CIRCLE = "TURNING_LUCKY_CIRCLE";
	static std::string GET_CAPTCHA = "GET_CAPTCHA";
	static std::string CHECK_CAPTCHA = "CHECK_CAPTCHA";
	static std::string RUBYDT_GET_INFO = "RUBYDT_GET_INFO";
	/**
	* thuc hien doi the va nhan ket qua ve
	* */
	static std::string RUBYDT_GET_CARD = "RUBYDT_GET_CARD";
	/**
	* lay lich su doi qua
	* */
	static std::string RUBYDT_GET_HISTORY = "RUBYDT_GET_HISTORY";
	/**
	  huy the cao
	*/
	static std::string RUBYDT_CANCEL_GET_CARD = "RUBYDT_CANCEL_GET_CARD";

	/*---------------------------------------
	*
	*	TAI XIU COMMAND
	*
	---------------------------------------*/


	/**
	* Lấy thông tin tài xỉu
	*/
	static std::string TAIXIU_GET_USER_INFO = "TAIXIU_GET_USER_INFO";
		/**
		* Lấy thông tin tài xỉu ruby
		*/
	static std::string TAIXIU_GET_USER_INFO_RUBY = "TAIXIU_GET_USER_INFO_RUBY";
		/**
		* Gửi thông tin đặt cược tài xỉu
		*/
	static std::string TAIXIU_BET = "TAIXIU_BET";
		/**
		* Gửi thông tin đặt cược ruby tài xỉu
		*/
	static std::string TAIXIU_BET_RUBY = "TAIXIU_BET_RUBY";
		/**
		* lấy thông tin leaderboard tài xỉu
		*/
	static std::string TAIXIU_GET_TOP_USERS = "TAIXIU_GET_TOP_USERS";
		/**
		* lấy thông tin leaderboard ruby tài xỉu
		*/
	static std::string TAIXIU_GET_TOP_USERS_RUBY = "TAIXIU_GET_TOP_USERS_RUBY";
		/**
		* lấy thông tin my history tài xỉu
		*/
	static std::string TAIXIU_GET_USER_HISTORY = "TAIXIU_GET_USER_HISTORY";

		/**
		* lấy thông tin my history ruby tài xỉu
		*/
	static std::string TAIXIU_GET_USER_HISTORY_RUBY = "TAIXIU_GET_USER_HISTORY_RUBY";
		/**
		* lấy thông tin real time tài xỉu
		*/
	static std::string TAIXIU_GET_INFO = "TAIXIU_GET_INFO";
		/**
		* lấy thông tin những màn hình, game k hỗ trợ tài xỉu
		*/
	static std::string TAIXIU_GET_UNSUPPORTED_LIST = "TAIXIU_GET_UNSUPPORTED_LIST";
		/**
		* lấy thông tin những màn hình, game k hỗ trợ tài xỉu
		*/
	static std::string TAIXIU_GET_UNSUPPORTED_LIST_RUBY = "TAIXIU_GET_UNSUPPORTED_LIST_RUBY";
};
#endif