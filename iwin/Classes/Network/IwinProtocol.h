#ifndef IWINPROTOCOL_H
#define IWINPROTOCOL_H
#include "Common/Common.h"
namespace IwinProtocol
{

	static const s16 LOGIN = 1;
	static const s16 LOGOUT = 2;
	static const s16 LOGIN_SUCESS = 3;
	static const s16 LOGIN_FAIL = 4;
	static const s16 CHAT_TO = 5;
	static const s16 ROOM_LIST = 6;
	static const s16 BOARD_LIST = 7;
	static const s16 JOIN_BOARD = 8;
	static const s16 ON_CHAT_BOARD = 9;
	static const s16 SYSTEM_INFO = 124;
	static const s16 ERROR_MESSAGE = 10;
	static const s16 KICK = 11;
	static const s16 SOMEONE_JOINBOARD = 12;
	static const s16 SOMEONE_LEAVEBOARD = 14;
	static const s16 LEAVE_BOARD = 15;
	static const s16 READY = 16;
	static const s16 TRANSFER_MONEY = 17;
	static const s16 SET_PASS = 18;
	static const s16 SET_MONEY = 19;
	static const s16 FORCE_LOSE = 23;
	static const s16 WANT_DRAW = 24;
	static const s16 DRAW = 25;
	static const s16 DENY_DRAW = 26;
	static const s16 WANT_LOSE = 27;
	static const s16 JOIN_ANY_BOARD = 28;
	static const s16 FRIENDLIST = 29;
	static const s16 STRONGEST_LIST = 30;
	static const s16 RICHEST_LIST = 31;
	static const s16 BET_LIST_CAO6 = 25;
	static const s16 ADJUST_MONEY = 26;
	static const s16 DELETE_FRIEND = 33;
	static const s16 PLAYER_DETAIL = 34;
	static const s16 REGISTER_INFO = 35;
	static const s16 SEARCH = 36;
	static const s16 GAME_RESULT = 37;
	static const s16 AVATAR_IMAGE = 38;
	static const s16 AVATAR_LIST = 39;
	static const s16 USER_DATA = 40;
	static const s16 REQUEST_AVATAR = 38;
	static const s16 REQUEST_AVATARLIST = 39;
	static const s16 SET_USER_DATA = 41;
	static const s16 BUY_AVATAR = 1202;
	static const s16 CHARGE_MONEY_INFO = 44;
	static const s16 SERVER_MESSAGE = 45;
	static const s16 SERVER_INFO = 46;
	static const s16 ADMIN_COMMAND = 147;
	static const s16 VERSION = 48;
	static const s16 SKIP_TIENLEN = 49;
	static const s16 STOP_GAME = 50;
	static const s16 BONUS_MONEY = 52;
	static const s16 FORCE_FINISH = 53;
	static const s16 MOVE_ERROR = 54;
	static const s16 ROOM_NAME = 55;
	static const s16 SET_MAX_PLAYER = 56;
	static const s16 SEND_SMS_FINISH = 57;
	static const s16 SET_PROVIDER = 58;
	static const s16 UP_BO = 49;
	static const s16 CHIA_BAI = 59;
	static const s16 WIN = 60;
	static const s16 PLAYING = 62;
	static const s16 START = 20;
	static const s16 MOVE = 21;
	static const s16 MOVE_AND_WIN = 22;
	static const s16 SET_GAME_TYPE = 61;
	static const s16 GET_CARD = 63;
	static const s16 EAT_CARD = 64;
	static const s16 HA_PHOM = 65;
	static const s16 SEND_LIST_ADD_CARD = 260;
	static const s16 SET_CLIENT_TYPE = -1;
	static const s16 FINISH = 51;
	static const s16 ADD_CARD = 67;
	static const s16 RESET_PHOM_EAT = 68;
	static const s16 DEN_BAI = 69;
	static const s16 ONCE_WIN = 70;
	static const s16 CHARGE_MONEY = 71;
	static const s16 GET_TIME_BAR = 261;
	static const s16 QUICK_PLAY = 262;
	/**
	* Tham so la 1 byte nhu sau 1 : Tell Friend. 2 : Lay so dien thoai hotline.
	* 3 : Láº¥y link cho game khÃ¡c 4 : DÃ nh cho link vFarm 5 : App nÃ o Ä‘ang
	* review 6 ; ThÃ´ng tin sá»± kiá»‡n 7 : Kiá»ƒm tra cÃ³ sá»± kiá»‡n má»›i khÃ´ng 8 :
	* Application nÃ o Ä‘ang review. Ä�á»‹nh dáº¡ng : id-version;id-version
	*
	* <pre>
	*    Vá»›i :
	*          id = 0 : iWinJ2MEAll
	*          id = 1  : iWiniPhone
	*          id = 2  : iWinAndroid
	* </pre>
	*
	* 11 : Lay danh sach server.
	*/
	static const s16 GET_INFO = 72;
	/**
	* Láº¥y thÃ´ng tin cÃº phÃ¡p tin nháº¯n : 1 : Nháº¯n tin láº¥y máº­t kháº©u.
	*/
	static const s16 GET_SMS_SYNTAX = 73;
	/**
	* Dung message nay de request nhung player dang ranh.
	*/
	static const s16 INVITE_PLAYER = 74;
	/**
	* Command chung duÌ€ng Ä‘ÃªÌ‰ get danh saÌ�ch. ThÃªm sub id Ä‘ÃªÌ‰ xaÌ�c Ä‘inh lÃ¢Ì�y
	* danh saÌ�ch giÌ€ Danh saÌ�ch sub id hiÃªÌ£n taÌ£i: 1 : Danh saÌ�ch Top Rank (
	* Bao gom game id va chuoi de hien thi duoi client, game id == 0 : Danh
	* sach dai gia ) 2 : Lay danh sach nguoi choi dang cho.
	*
	*
	*/
	static const s16 GET_LIST = 78;
	/**
	* Command dung de lay hinh tu server<br>
	* subID 1: lay hinh items. 2: Hinh Avatar 3: Láº¥y hÃ¬nh background. 4:
	*
	*/
	static const s16 GET_IMAGE = 79;
	/**
	* Gui thong tin don them cua Tetris.
	*/
	static const s16 TETRIS_COMMAND = 80;
	/**
	* Set client information
	*/
	static const s16 SET_CLIENT_INFO = -36;
	/**
	* Trong truong hop client xay ra loi. Dung command nay de gui thong bao loi
	* o ham nao va dang loi len server.
	*/
	static const s16 REPORT_ERROR = 99;
	/**
	* Sub id : 0 : Lisst
	*/
	static const s16 QUEST = 102;
	/*
	* set languge
	*/
	static const s16 SET_SESSION_INFOR = 100;
	/*
	* Command dung cho viá»‡c láº¥y danh sÃ¡ch game
	*/
	static const s16 GET_GAMES_LIST = 81;
	// server -> Client
	static const s16 ADMIN_COMMAND_RESPONSE = -109;
	static const s16 CLEAR_RMS_REQUEST = 76;
	static const s16 SET_CLIENT_INFOR = 220;
	static const s16 SET_CLIENT_INFOR_WITH_SUBPROVIDER = 222;
	// <editor-fold defaultstate="collapsed"
	// desc="Há»‡ thá»‘ng command dugn cho Item">
	static const s16 ITEMS = 101;
	/**
	* sub command cua ITEMS, táº¥t ca sub Ä‘á»�u cÃ³ tiá»�n tá»‘ ITEMS
	*/
	/**
	* CÃ³ thá»ƒ tráº£ vá»� danh sÃ¡ch categories hoáº·c danh sÃ¡ch items
	*/
	static const s16 ITEMS_LIST_CATEGORIES = 1;
	/**
	* Tráº£ vá»� danh sÃ¡ch items theo category
	*/
	static const s16 ITEMS_GET_ITEMS_BY_CATEGORY = 2;
	/**
	* Láº¥y danh sÃ¡ch item user sá»Ÿ há»¯u
	*/
	static const s16 ITEMS_GET_ITEMS_BY_USER = 3;
	/**
	* Láº¥y thÃ´ng tin chi tiáº¿t cá»§a item
	*/
	static const s16 ITEMS_GET_DETAILS_ITEM = 4;
	/**
	* Láº¥y thÃ´ng tin chi tiáº¿t cá»§a item lÃ  avatar
	*/
	static const s16 ITEMS_GET_DETAILS_AVATAR = 5;
	/**
	* User yÃªu cáº§u mua item
	*/
	static const s16 ITEMS_BUY_ITEM = 6;
	/**
	* User yÃªu cáº§u táº·ng item
	*/
	static const s16 ITEMS_GIVE_ITEM = 7;
	/**
	* User chá»�n take-off item Ä‘ang dÃ¹ng trong shop avatar. Náº¿u cÃ³ thá»ƒ remove vÃ­
	* dá»¥ nhÆ° phá»¥ kiá»‡n: Ä‘á»“ng há»“, tháº¯t lÆ°ngâ€¦ thÃ¬ tráº£ vá»� action TAKE_OFF Náº¿u item
	* cÃ³ default: nhÆ° Ä‘áº§u, thÃ¢n.. ThÃ¬ tra vá»� action SET_DEFAUT vÃ  chi tiáº¿t item
	* default
	*/
	static const s16 ITEM_GET_SHORT_AVATAR = 8;
	/**
	* chÆ°a dinh nghÄ©a
	*/
	static const s16 ITEM_GET_DETAILS_USER_AVATAR = 9;
	/*
	* User remove item ra khá»�i danh sÃ¡ch Ä‘ang sá»Ÿ há»¯u
	*/
	static const s16 ITEMS_REMOVE_ITEM = 10;
	/**
	* User set use cho item hiá»‡n táº¡i. Server set item use vÃ  set not use cho
	* item cÃ¹ng loáº¡i Ä‘ang dÃ¹ng
	*/
	static const s16 ITEMS_SET_USING = 11;
	/**
	* Lay avatar cua 2 user cu the
	*/
	static const s16 ITEMS_GET_USING_AVATAR = 12;
	static const s16 ITEMS_GET_DETAILS_USER_ITEM = 13;
	// </editor-fold>
	/*
	* Command cho chá»©c nang Buzz;
	*/
	// static const s16 BUZZ = 103;
	/*
	* Chá»©c nang káº¿t báº¡n cÃ³ xÃ¡c nháº­n cá»§a Ä‘á»‘i phÆ°Æ¡ng
	*/
	static const s16 FRIEND = 104;
	static const s16 UNDO = 105;
	static const s16 SEND_CAPTURE_IMG = 1000;
	static const s16 GET_AVATAR_BY_USERNAME = 1001;
	static const s16 UPDATE_PROFILE = 1002;
	static const s16 CHANGE_PASSWORD = 1003;
	static const s16 UPDATE_STATUS = 1004;
	static const s16 GET_PROFILE = 1005;
	static const s16 UPDATE_MONEY = 1006;
	static const s16 BOARD_INFO = 1300;
	/**
	* Lay tat ca cac avatar ma player dang so huu.
	*/
	static const s16 GET_BOUGHT_AVATARS = 1007;
	/**
	* Thay doi avatar.
	*/
	static const s16 CHANGE_AVATAR = 1008;
	/**
	* Xoa 1 avatar ma user dang co.
	*/
	static const s16 DELETE_MY_AVATAR = 1009;
	static const s16 GET_BOUGHT_AVATARS_USE_TEXT_TIME = 1010;
	/**
	* ***************************Cac command dung cho game
	* banh************************************
	*/
	static const s16 LIST_ACTIVE_MATCH = 1501;
	static const s16 ROUND_CALLENDAR = 1502;
	static const s16 MATCH_CALLENDAR = 1503;
	static const s16 BET = 1504;
	static const s16 MY_BET = 1505;
	static const s16 RESULT_BET = 1506;
	static const s16 CANCEL_BET = 1507;
	static const s16 GET_MY_FOOTBALL_INFO = 1508;
	static const s16 GET_RANKING = 1509;
	static const s16 GET_RATE_WIN = 1510;
	static const s16 CHANGE_BALL = 1511;
	static const s16 GET_TOURNAMENT = 1512;
	static const s16 GET_NAP = 1513;
	static const s16 GET_MY_RANK = 1515;
	static const s16 GET_LEVEL_IMAGE = 1516;
	static const s16 GET_FLAG = 1514;
	static const s16 SEARCH_WAITING_LIST = 1303;
	static const s16 POKER_COMMAND = 82;
	static const s16 BAUCUA_CMD = 84;
	static const s16 JOIN_VIEWER = 1900;
	static const s16 AUTO_JOIN_VIEWER = 1901;
	static const s16 PLAYER_TO_VIEWER = 1902;
	static const s16 REQUEST_HTTP = 1800;
	static const s16 GET_AVATARS = 1203;
	static const s16 GET_WAITING_LIST = 1301;

	/**
	* dung khi 2 thang chat voi nhau
	*/
	static const s16 NEW_CHAT = 1707;
	static const s16 MESSAGE = 1701;
	static const s16 CHAT_ROOM = 1708;

	/**
	* lay thong tin elo tat ca game ...
	*
	**/
	static const s16 GET_STATISTIC = 1011;
	static const s16 GET_BEST_ELO = 85;

	static const s16 CO_TUONG_COMMAND = 77;
	static const s16 CO_UP_COMMAND = 86;
	/**
	* Return game
	*/
	static const s16 RETURN_GAME = 2200;
	static const s16 RECONNECT = 2201;
	static const s16 RECONNECTED_AND_GOTOGAME = 2202;

	// login facebook
	// In: access token.
	// Out: INVALID_ACCESS_TOKEN message.
	static const s16 LOGIN_WITH_ACCESS_TOKEN = 2301; // Ä‘Äƒng nháº­p báº±ng
															  // facebook
	static const s16 INVALID_ACCESS_TOKEN = 2302; // access token sai
														   // hoáº·c háº¿t háº¡n
	static const s16 CREATE_IWIN_ACCOUNT = 2303; // server tráº£ vá»� khi
														  // má»›i Ä‘Äƒng nháº­p láº§n
														  // Ä‘áº§u tiÃªn (chÆ°a cÃ³
														  // nick iwin)
	static const s16 CREATE_IWIN_ACCOUNT_AND_LOGIN = 2304; // gá»­i ngay
																	// khi Ä‘Äƒng
																	// kÃ­ nick
																	// iwin báº±ng
																	// tÃ i khoáº£n
																	// facebook
	static const s16 IWIN_ACCOUNT_INFO = 2306; // Lay
														// location
	static const s16 LOCATION = 2500;
	static const s16 PICKROOM = 2700;
	static const s16 REPORT = 2703;

	// LIENG
	static const s16 LIENG_COMMAND = 106;

	/**
	* cháº·n ko cho chat vá»›i mÃ¬nh
	*/
	static const s16 BAN_NICK = 2901;

	// feature question and answer when login successfull
	static const s16 RESPOND_QUESTION_FROM_SERVER = 3001;
	static const s16 SEND_ANSWER_TO_SERVER = 3002;
	static const s16 SEND_ID_QUESTION_TO_SERVER = 3003;
	static const s16 RESPOND_HAS_QUESTION_FROM_SERVER = 3004;
	static const s16 CHECK_USERNAME = 2401;
	static const s16 VALID_PASS = 2402;

	/**
	* Command dung cho game tien len solo
	*/
	static const s16 TIENLENSOLO_COMMAND = 107;

	/**
	* Command dung cho game xam
	*/
	static const s16 XAM_COMMAND = 108;

	/**
	* Command dung cho game catte
	*/
	static const s16 CATTE_COMMAND = 109;
	/**
	* buzz má»›i, tráº£ vá»� id cá»§a tháº±ng buzz
	*/
	static const s16 BUZZ = 110;
	/**
	* nháº­n Ä‘Æ°á»£c cÃ¡i nÃ y khi lÃ m 1 cÃ¡i gÃ¬ Ä‘Ã³ mÃ  ko Ä‘á»§ tiá»�n, nháº­n Ä‘c cÃ¡i nÃ y thÃ¬
	* hiá»‡n dialog há»�i cÃ³ muá»‘n náº¡p win ko
	*/
	static const s16 NOT_ENOUGH_MONEY = 120;
	/**
	* nháº­n Ä‘Æ°á»£c cÃ¡i nÃ y khi pháº£i chá»� Ä‘á»ƒ Ä‘Æ°á»£c sáºµn sÃ ng trong game
	*/
	static const s16 WAIT_TIME_INFO = 122;
	/**
	* nháº­n cÃ¡i nÃ y thÃ¬ hiá»‡n báº£ng thÃ´ng bÃ¡o confirm ,báº¥m ok lÃ  gá»­i tin nháº¯n
	*/
	static const int SEND_SMS_SYNTAX = 123;
	/**
	* cmd id game Domino
	*/
	static const int DOMINO_COMMAND = 87;

	static const int RULET_COMMAND = 88;

	/**
	* cmd update list server
	*/
	static const int CHANGE_SERVER = 501;

	static const int CONFIRM_LEAVE_GAME = 502;

	static const int HEART_BEAT = 505;

	/**
	*buoc doi mat khau khi mat khau cu yeu
	* */
	static const int REQUIRE_UPDATE_PASS = 1013;

	/**
	*Lay so luot quay cau may cua user
	* */
	static const int REQUEST_USER_POINT = 2105;

	/**
	*Lay list ban xoay cau may
	* */
	static const int REQUEST_LIST = 2101;

	/**
	*request so luot quay (checked)
	* */
	static const int REQUEST_TURN = 2102;

	/**
	*Toast message tá»« server tráº£ vá»�
	* */
	static const int TOAST_MESSAGE = 506;


	/**
	* cho phep nhan loi moi choi game
	* */
	static const int INVITATION_SETTING = 510;

	/**
	* Chứng thực email, số điện thoại trong profile
	*/
	static const int VERIFY_USER = 511;
	/**
	* lay so user cho, phong cho, cho man hinh mainscreen
	* */
	static const int GET_STATISTIC_WAITING_BOARD_USER = 512;

	static const int GET_PASSWORD = 520;

	static const int CURRENCY = 513;
	/**
	* Enable ruby or not
	*/
	static const int ENABLE_RUBY = 514;

	/**
	* Gửi mã code khi dc server trả mã Code về
	*/
	static const int CONFIRM_SMS_CODE = 515;
	/**
	* Nhận về money type hiện tại
	*/
	static const int MONEY_TYPE_INFO = 2203;

	/**
	* Danh sach top dai gia Ruby
	*/
	static const int RICHEST_RUBY_LIST = 126;

	/**
	* Check IP nuoc ngoai
	*/
	static const int CHECK_REVIEW = 508;

	/**
	* gui
	* */
	static const int SEND_SCREEDID = 509;
};
#endif