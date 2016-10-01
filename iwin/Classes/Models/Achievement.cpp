#include "Achievement.h"

Achievement::Achievement(char gameId, int exp, int win, int lose, int draw, int givenUp, int eloNumber, std::string elo, int eloItemId)
{
	this->gameId = gameId;
	 this->exp = exp;
	 this->win = win;
	 this->lose = lose;
	 this->draw = draw;
	 this->givenUp = givenUp;
	 this->eloNumber = eloNumber;
	 this->eloItemId = eloItemId;
	 this->elo = elo;
}

Achievement::Achievement(Achievement * achieve)
{
	this->gameId = achieve->gameId;
	this->exp = achieve->exp;
	this->win = achieve->win;
	this->lose = achieve->lose;
	this->draw = achieve->draw;
	this->givenUp = achieve->givenUp;
	this->eloNumber = achieve->eloNumber;
	this->eloItemId = achieve->eloItemId;
	this->elo = achieve->elo;
}

Achievement::~Achievement()
{

}
