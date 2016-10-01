#ifndef __FLOP_CARD_UI_H__
#define __FLOP_CARD_UI_H__
#include "cocos2d.h"
class Card;
class FlopCardUI : public cocos2d::LayerColor
{
private:
	Card* cardFlop;
	Card* cardValue;
	int index;
	std::vector<unsigned char> _card_value;
	cocos2d::Vec2 _previous_pos_touch;
	std::function<void(bool)> _action_flop;
public:

	FlopCardUI();
	~FlopCardUI();

	virtual bool init() override;
	virtual void update(float dt) override;
	void setValueCards(std::vector<unsigned char> ids);
	void HandleTouch(float dt);
	void SetAction(const std::function<void(bool)> & call_back);
};

#endif 

