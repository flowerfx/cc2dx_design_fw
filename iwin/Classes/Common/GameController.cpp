#include "Screens/ScreenManager.h"

#include "GameController.h"
#include "Constants.h"
#include "Utils.h"
#include "Network/JsonObject/Game/GameList.h"
#include "Network/Global/Globalservice.h"
#include "Network/Game/GameLogicHandler.h"
#include "Screens/GameScreens/TienLen/TienLenMsgHandler.h"
#include "Screens/GamePlayMgr.h"

#include "Screens/GameScreens/GameScreens.h"
#include "Screens/GameScreens/TienLen/TienLenScreen.h"
#include "Screens/GameScreens/TienLen/TienLenPlayer.h"
#include "Screens/GameScreens/MauBinh/MauBinhScreen.h"
#include "Screens/GameScreens/MauBinh/MauBinhPlayer.h"
#include "Screens/GameScreens/MauBinh/MauBinhMsgHandler.h"
#include "Screens/GameScreens/BaiCao/BaiCaoPlayer.h"
#include "Screens/GameScreens/BaiCao/BaiCaoMsgHandler.h"
#include "Screens/GameScreens/Phom/PhomMsgHandler.h"
#include "Screens/GameScreens/XiZach/XiZachMsgHandler.h"
#include "Screens/GameScreens/XiZach/XiZachPlayer.h"
#include "Screens/GameScreens/CoTuong/CoTuongMsgHandler.h"
#include "Screens/GameScreens/CoTuong/CoTuongPlayer.h"


GameType		GameController::gameId = GameType::NULL_TYPE;
std::string		GameController::lastLoginAccount = "";
GameEventList*	GameController::gameEventList = nullptr;
AccountTypes	GameController::loginType = AccountTypes::invalid;
Player*			GameController::myInfo = nullptr;
GameList*		GameController::gameList = nullptr;
std::vector<unsigned char> GameController::serverAllGames;
bool			GameController::enableRuby = false;
bool			GameController::enableCashOut = false;
std::vector<Player*> GameController::FriendList;
std::string		GameController::searchNick = "";
int				GameController::currentMoneyType = BASE_MONEY;
int				GameController::typeMoneyTaiXiu = BASE_MONEY;

int				GameController::currentScreenId = -1;
int				GameController::lastScreenId = -1;

GameScreen* GameController::currentGameScreen = nullptr;

GameController::GameController()
{
	FriendList.clear();
}

GameController::~GameController()
{

}

bool GameController::isFriend(int id) 
{
	for (auto fr : FriendList)
	{
		if (fr->IDDB == id)
		{
			return true;
		}
	}
	return false;
}

bool GameController::isFriend(std::string username)
{
	for (auto fr : FriendList)
	{
		if (fr->name == username)
		{
			return true;
		}
	}
	return false;
}

void  GameController::SetFriendList(std::vector<Player*> friendlist)
{
	for (Player * p : GameController::FriendList)
	{
		delete p;
	}
	GameController::FriendList.clear();

	GameController::FriendList = friendlist;
}

//@PDT: TODO
void GameController::resetToNewAccount()
{
	if (myInfo->name.empty())
		return;
	if (Utils::trim(Utils::toLowerCase(myInfo->name)) != lastLoginAccount)
	{
		//	screenManager.resetAllScreen();
		//	FriendFacebookListItem.clearListAlreadySendAddFriendIDs();
		//	GameController.friendList.clear();
		//	FacebookFriendManager.clearFriends();	
	}

	lastLoginAccount = Utils::trim(Utils::toLowerCase(myInfo->name));
	gameEventList = nullptr;
	//FacebookInviteFriendDialog.instance = null;
}

void GameController::setGameEventList(GameEventList* _gameEventList) {
	gameEventList = _gameEventList;
}

GameEventList* GameController::getGameEventList()
{
	return gameEventList;
}

void GameController::setLoginType(AccountTypes _loginType)
{
	loginType = _loginType;
}

//@PDT: TODO
void GameController::saveAutoLogin()
{

}

void setGameTypeCallback()
{

}

//@PDT: TODO
void GameController::doPlay(s16 game_type)
{
	//screenManager.showPickRoomScreen(ChangeScreenType.fromRight, gameID);
	//screenManager.pickRoom.showLoadingCircle(true, screenManager.pickRoom.getGroupCenter());
	//DialogUtil.startWaitDialog();
	GlobalService::setGameType(game_type, []() {
		//DialogUtil.hideWaitDialog();
		/*if (GameController::getLobbyScreen())
		{
		GameController::getLobbyScreen()->onSetGameTypeSuccessed();
		}*/
		//if (GameController::game::getScreen() == GameController.screenManager.pickRoom) {
		//GameController::screenManager::pickRoom.onSetGameTypeSuccessed();
		//}
	});

}

bool GameController::isLoggedIn()
{
	return myInfo->isLoggedIn() && loginType != AccountTypes::invalid;
}

AccountTypes GameController::getLoginType()
{
	return loginType;
}

void GameController::setGameList(GameList* _gameList)
{
	gameList = _gameList;
	std::vector<unsigned char> games;
	std::vector<int> gameCards = gameList->getGameCard();
	for (auto i : gameCards)
	{
		games.push_back(i);
	}
	std::vector<int> gameChess = gameList->getGameChess();
	for (auto i : gameChess)
	{
		games.push_back(i);
	}
	std::vector<int> gameOther = gameList->getGameOther();
	for (auto i : gameOther)
	{
		games.push_back(i);
	}
	setServerAllGames(games);
}

void GameController::setServerAllGames(std::vector<unsigned char> games) {
	serverAllGames.clear();
	for (ubyte game : games)
	{
		serverAllGames.push_back(game);
	}
}


void GameController::setGameType(int _gameId)
{
	gameId = (GameType)_gameId;
}

GameType GameController::getCurrentGameType()
{
	return gameId;
}

std::string GameController::getCurrentGameName()
{
	switch (gameId)
	{
	case GAMEID_TIENLEN:
		return LangMgr->GetString("gamename_tlmn").GetString();
	case GAMEID_TIENLEN_SOLO:
		return LangMgr->GetString("gamename_tlsolo").GetString();
	case GAMEID_BINH:
		return LangMgr->GetString("gamename_binh").GetString();
	case GAMEID_BLACKJACK:
		return LangMgr->GetString("gamename_xizach").GetString();
	case GAMEID_PHOM_MB:
		return LangMgr->GetString("gamename_phom").GetString();
	default:
		return "Invalid Game Name";
	}
}

Player * GameController::createPlayerByGameID()
{
	Player* p ;
	switch (gameId)
	{
	case GAMEID_TIENLEN:
	case GAMEID_TIENLEN_MB:
	case GAMEID_TIENLEN_SOLO:
	case GAMEID_XAM:
	case GAMEID_CATTE:
		p = new TienLenPlayer();
		break;
	case GAMEID_BINH:
		p = new MauBinhPlayer();
		break;
	case GAMEID_POKER:
	case GAMEID_POKER_HK:
		//p = new PokerPlayer();
		break;
	case GAMEID_BAUCUA:
		//p = new BauCuaPlayer();
		break;
	case GAMEID_LIENG:
		//p = new LiengPlayer();
		break;
	case GAMEID_BAICAO:
	case GAMEID_BAICAORUA:
		p = new BaiCaoPlayer();
		break;
	case GAMEID_BLACKJACK:
		p = new XiZachPlayer();
		break;
	case GAMEID_CHESS:
		p = new CoTuongPlayer();
		break;
	default:
		p = new Player();
		break;
	}
	return p;
}

/**
* Do show a game screen following current gameID.
*
* @param roomID
* @param boardID
* @param ownerID
* @param money
*            - bet money in the board.
* @param players
*            - Vector player contain all players in the board.
*/
void GameController::showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players)
{
	
	GameLogicHandler::isJoinGame = true;
	switch (getCurrentGameType()) {
	case GAMEID_TIENLEN:
	case GAMEID_TIENLEN_MB:
	case GAMEID_TIENLEN_SOLO:
		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());

		break;
	case GAMEID_PHOM_MB:

		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());

		break;

	case GAMEID_BAICAO:
	case GAMEID_BAICAORUA:

		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());

		break;
	case GAMEID_BLACKJACK:
		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());
		break;
	case GAMEID_POKER:


		break;
	case GAMEID_CARO:


		break;
	case GAMEID_CHESS:
	case GAMEID_GIAI_DAU_CO_UP:
	case GAMEID_CO_UP:
		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());
		break;
	case GAMEID_TETRIS:


		break;
	case GAMEID_COVUA:

		break;
	case GAMEID_BINH:
		GameController::currentGameScreen = static_cast<GameScreen*>(ScrMgr->GetGameScreen());
		break;
	case GAMEID_BAUCUA:

		break;
	case GAMEID_POKER_HK:

		break;
	case GAMEID_LIENG:


		break;
	case GAMEID_XAM:


		break;
	case GAMEID_CATTE:

		break;
	case GAMEID_DOMINO:


		break;
	default:
		break;
	}
	if (GameController::currentGameScreen != nullptr )
	{
		GameController::currentGameScreen->setPlayers(roomID, boardID, ownerID, money, players);
		if (ScrMgr->GetCurrentMenuUI(0) != HUD_SCREEN)
		{
			GameController::currentGameScreen->updateTextWithMoneyType();
			createGameServiceHandlerOf(gameId);
			ScrMgr->SwithToGameScreen(getCurrentGameType() , players.size());
		}

	}

}

/**
* *
*
* @param gameType
*/
void GameController::createGameServiceHandlerOf(int gameId)
{
	GameLogicHandler::gameHandlers = nullptr;
	switch (GameController::getCurrentGameType())
	{
	case GAMEID_CATTE:

		break;
	case GAMEID_XAM:

		break;
	case GAMEID_TIENLEN:
	case GAMEID_TIENLEN_MB:
	case GAMEID_TIENLEN_SOLO:
		GameLogicHandler::gameHandlers = TienLenMsgHandler::getInstance();
		break;
	case GAMEID_POKER:

		break;
	case GAMEID_CARO:

		break;
	case GAMEID_CO_UP:
	case GAMEID_GIAI_DAU_CO_UP:
	case GAMEID_CHESS:
		GameLogicHandler::gameHandlers = CoTuongMsgHandler::getInstance();
		break;
	case GAMEID_TETRIS:
		break;
	case GAMEID_COVUA:

		break;

	case GAMEID_BAICAO:
	case GAMEID_BAICAORUA:
		GameLogicHandler::gameHandlers = BaiCaoMsgHandler::getInstance();
		break;
	case GAMEID_BLACKJACK:
		GameLogicHandler::gameHandlers = XiZachMsgHandler::getInstance();
		break;
	case GAMEID_PHOM_MB:
		GameLogicHandler::gameHandlers = PhomMsgHandler::getInstance();
		break;
	case GAMEID_BINH:
		GameLogicHandler::gameHandlers = MauBinhMsgHandler::getInstance();
		break;
	case GAMEID_BAUCUA:

		break;
	case GAMEID_POKER_HK:

		break;
	case GAMEID_LIENG:

		break;
	case GAMEID_DOMINO:

		break;
	default:
		break;
	}



}
int GameController::getMaxPlayerOf(GameType gameID) {
	switch (GameController::getCurrentGameType()) {
	case GAMEID_BAICAORUA:
	case GAMEID_POKER:
	case GAMEID_BAUCUA:
	case GAMEID_LIENG:
		return 6;
	case GAMEID_BAICAO:
	case GAMEID_BLACKJACK:
	case GAMEID_PHOM_MB:
	case GAMEID_TIENLEN:
	case GAMEID_TIENLEN_MB:
	case GAMEID_BINH:
	case GAMEID_DOMINO:
	case GAMEID_XAM:
		return 4;
	case GAMEID_CARO:
	case GAMEID_CO_UP:
	case GAMEID_COVUA:
	case GAMEID_CHESS:
	case GAMEID_TETRIS:
	case GAMEID_TIENLEN_SOLO:
	case GAMEID_GIAI_DAU_CO_UP:
		return 2;
	case GAMEID_POKER_HK:
	case GAMEID_CATTE:
		return 5;
	case GAMEID_VONGQUAYTAILOC:
		return 8;
	case GAMEID_LUCKYCIRCLE:
		return 1;
	default:
		return 4;
	}
}


bool GameController::isNewLobby(int gameId)
{
	if (gameList)
	{
		return gameList->hasNewLobby(gameId);
	}
	else
	{
		return false;
	}
}

bool GameController::isBaseMoney()
{
	return currentMoneyType == BASE_MONEY;
}
std::string GameController::getTextWinRubyTaiXiu() {
	return typeMoneyTaiXiu == RUBY_MONEY ? "Ruby" : "Win";
}

std::string GameController::getMoneyType()
{
	if (currentMoneyType == BASE_MONEY) return "win";
	return "ruby";
}

bool GameController::isHiddenChess() {
	return (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP);
}