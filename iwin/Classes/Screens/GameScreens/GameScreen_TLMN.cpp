#include "GameScreen_TLMN.h"
#include "../Object/CardObject.h"

GameScreen_TLMN::GameScreen_TLMN()
{
	p_current_game_type = GameType::GAMEID_TIENLEN;
}

GameScreen_TLMN::~GameScreen_TLMN()
{

}

bool GameScreen_TLMN::Init(int game_id)
{
	p_list_object_in_game.pushBack(CardObj::NewInstance(10 , DECK_TYPE::DECK_TIENLEN));
	//static_cast<CardObj*>(p_list_object_in_game.at(0))->ResetID(20, DECK_TYPE::DECK_TIENLEN);
	return true;
}



void GameScreen_TLMN::update(float dt)
{
	GameScreen::update(dt);
}
