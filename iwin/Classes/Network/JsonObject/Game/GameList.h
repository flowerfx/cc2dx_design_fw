#ifndef _GameList_H_
#define _GameList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GameList
	{
		
CC_SYNTHESIZE(std::vector<int>, gamePopular, GamePopular);
CC_SYNTHESIZE(std::vector<int>, gameCard, GameCard);
CC_SYNTHESIZE(std::vector<int>, gameChess, GameChess);
CC_SYNTHESIZE(std::vector<int>, gameOther, GameOther);
CC_SYNTHESIZE(std::vector<int>, gameHot, GameHot);
CC_SYNTHESIZE(std::vector<int>, gameNew, GameNew);
CC_SYNTHESIZE(std::vector<int>, gameUpdate, GameUpdate);
CC_SYNTHESIZE(std::vector<int>, gameTournaments, GameTournaments);
CC_SYNTHESIZE(std::vector<int>, gameHasNewLobby, GameHasNewLobby);

	public:
		
		GameList();
		virtual ~GameList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);

		bool	hasNewLobby(int gameId);
	};
}
#endif
