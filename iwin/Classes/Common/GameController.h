#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "Common/Common.h"
#include "Network/JsonObject/Game/GameEventList.h"
#include "Models/Player.h"
#include "Models/User.h"
#include "Network/JsonObject/Game/GameList.h"

using namespace iwinmesage;
class GameScreen;
enum GameType
{
	NULL_TYPE = -1,
	GAMEID_TIENLEN = 3,
	GAMEID_PHOM = 7,
	GAMEID_POKER = 4,
	GAMEID_CARO = 1,
	GAMEID_CO_UP = 20,
	GAMEID_TETRIS = 8,
	GAMEID_COVUA = 9,
	GAMEID_BAICAO = 11,
	GAMEID_BAICAORUA = 17,
	GAMEID_BLACKJACK = 2,
	GAMEID_TIENLEN_MB = 12,
	GAMEID_PHOM_MB = 13,
	GAMEID_BINH = 16,
	GAMEID_CHESS = 5,
	GAMEID_BAUCUA = 18,
	GAMEID_POKER_HK = 6,
	GAMEID_LIENG = 21,
	GAMEID_TIENLEN_SOLO = 22,
	GAMEID_XAM = 31,
	GAMEID_CATTE = 24,
	GAMEID_WORLDCUP_COPHIEU = 25,
	GAMEID_WORLDCUP_CUOCCHAUA = 26,
	GAMEID_DOMINO = 27,
	GAMEID_VONGQUAYTAILOC = 29,
	GAMEID_GIAI_DAU_CO_UP = 30,
	GAMEID_LUCKYCIRCLE = 35
};

class GameController
{
private:

	/**
	* Giu lai account vua moi logout ra. Dung de check khi user login bang
	* account moi hay khong -> Reset lai data o cac man hinh.
	*/
	static std::string lastLoginAccount;
	static GameEventList* gameEventList;
	static AccountTypes loginType;
	static GameList*			gameList;
	static std::vector<unsigned char> serverAllGames;
	GameController();
	virtual ~GameController();
public:
	static GameType gameId;
	static Player*   myInfo;
	static bool enableRuby;
	static bool enableCashOut;
	static int currentMoneyType;

	static GameScreen* currentGameScreen;
	static void resetToNewAccount();
	static void setGameEventList(GameEventList* _gameEventList);
	static GameEventList* getGameEventList();
	static void setLoginType(AccountTypes type);
	static void saveAutoLogin();
	static void doPlay(s16 game_type);
	static bool isLoggedIn();
	static AccountTypes getLoginType();
	static void setGameList(GameList* gamelist);
	static void setServerAllGames(std::vector<unsigned char> games);

	static void setGameType(int _gameId);
	static GameType getCurrentGameType();
	static std::string getCurrentGameName();
	static Player* createPlayerByGameID();
	static int getMaxPlayerOf(GameType t);
	static void createGameServiceHandlerOf(int gameId);
	static void showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players);

	static std::vector<Player*> FriendList;
	static void SetFriendList(std::vector<Player*> friendlist);
	static std::string searchNick;
	static bool isFriend(int id);
	static bool isFriend(std::string username);
	static bool isNewLobby(int gameId);
	static bool isBaseMoney();
	static std::string getMoneyType();
	static bool isHiddenChess();
	/**
	* type money tai xiu
	*/
	static int typeMoneyTaiXiu;

	/**
	* luu y chi danh cho game tai xiu
	*/
	static std::string getTextWinRubyTaiXiu();

	//screen id
	static int currentScreenId;
	static int lastScreenId;
};

#endif //__GAME_CONTROLLER_H__

