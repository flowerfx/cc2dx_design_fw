#ifndef __ACTION_PHASE_MANAGER_H__
#define __ACTION_PHASE_MANAGER_H__



#include "cocos2d.h"
#include "DisplayView.h"
#include "Factory.h"
#include "Common/GameController.h"
using namespace cocos2d;
using namespace Utility;
class GameScreen;
class GamePlayMgr 
{
private:

	Factory::FactoryWithIndex<GameScreen>	p_factory_game_screen_entity;

	GameScreen * p_current_game_screen;
protected:

	void RegisterGameScreen();
public:
	GamePlayMgr();
	virtual ~GamePlayMgr();

	bool Init(int game_id);

	GameType GetGameTypeID(int game_id);

	GameScreen * GetCurrentGameScreen() {
		return p_current_game_screen;
	}
};

#endif //#define __GAME_SCREENS_H__


