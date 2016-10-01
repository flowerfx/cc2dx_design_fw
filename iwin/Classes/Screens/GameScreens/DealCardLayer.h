#ifndef __DEAL_CARD_LAYER_H__
#define __DEAL_CARD_LAYER_H__
#include "cocos2d.h"
#include <functional>

USING_NS_CC;

class Card;

class DealCardLayer : public Layer
{
private:
	std::vector<Card*> _list_cards;

	CC_SYNTHESIZE(Vec2, _card_size, CardSize);

	Card* getFreeCard();

public:
	virtual bool init();
	void dealCard(float delay, Card* c, std::function<void()> actionDone);
	void moveCard(Card* c, Vec2 des_pos, Vec2 des_size, const std::function<void()>& actionDone);

};

#endif // __DEAL_CARD_LAYER_H__