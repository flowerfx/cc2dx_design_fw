#ifndef __TIENLEN_PLAYER_H__
#define __TIENLEN_PLAYER_H__
#include "Models/Player.h"
class Card;
class TienLenPlayer : public Player
{
public:
	std::vector<unsigned char>  remain_cards;
	std::vector<Card*> cards;
public:
	TienLenPlayer();
	TienLenPlayer(Player * p);
	virtual ~TienLenPlayer();
	void unSelectAllCards();
	std::vector<unsigned char> getSelectedCardsValue();
	int GetRemainCard();
};

#endif 

