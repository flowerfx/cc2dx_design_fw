#ifndef __CARD_PHOM_BOX_H__
#define __CARD_PHOM_BOX_H__
#include "cocos2d.h"
#include "Common/Common.h"
#include "PhomScreen.h"
#include <vector>

USING_NS_CC;

class Card;
class PhomScreen;
class LabelBG;

class CardPhomBox
{
private:
	std::vector<Card*>	_firedCards;
	std::vector<Card*>	_phomCards;
	std::vector<Card*>	_freeCards;
	std::vector<Card*>	_eatCards;

	PhomScreen*			_game_screen;

	CC_SYNTHESIZE(int, _seat, Seat);
	CC_SYNTHESIZE(Vec2, _posEat, PosEat);
	CC_SYNTHESIZE(Vec2, _posFire, PosFire);
	CC_SYNTHESIZE(Vec2, _posFree, PosFree);
	CC_SYNTHESIZE(bool, _hlPhom, HighlightPhom);


	std::vector<Phom>		_myPhoms;
	VecByte					_myEatCards;

	LabelBG*				_lbl_score;
	LabelBG*				_lbl_info;
	Vec2					_card_size;

	void sortCards(std::vector<Card*> list_card);
	bool checkExist(VecCard cards, byte cardId);

public:
	void init(PhomScreen* game_creen, int seat);
	void addFreeCard(int cardId);
	void addFireCard(int cardId);
	void addPhomCard(int cardId);
	void addEatCard(int cardId);
	void layoutFreeCard();
	void layoutFireCard();
	void layoutPhomCard();
	void layoutEatCard();

	void addPhomToFreeCard(Card* c);

	void clearBox();
	void layoutBox();

	void setMyPhomEat(std::vector<Phom> myPhoms, VecByte myEatCards);
	void clearFreeCard();
	void clearPhomCard();
	void addFreeCard(VecCard list_card);

	void removeFreeCard(int cardId);
	void removeFiredCard(int cardId);
	void selectAllCardPhom(std::vector<Phom> myPhoms);
	void selectFreeCard(int cardId);
	void unselectFreeCards();
	bool hasFireCard(int cardId);
	void fireCard(int cardId);
	void getCard(int cardId, VecByte newFreeCard);
	void eatCard(int cardId, VecByte newFreeCard);
	void layoutFreeCardsWithAnim(VecByte cards);
	void mapByteToCard(VecCard& card_array, VecByte byte_array);
	void hideFireCards();

	VecByte getCardSelected();
	VecCard getFreeCards();

	int getLastFireCardID();

	void showScore(int score);
	void showInfo(int type);

	void reset();
	void resetListCard(VecCard cards);
	void replacePhom(VecByte new_phom);
private:
	void setupFreeCard(Card* c);

	

};

#endif // __CARD_PHOM_BOX_H__