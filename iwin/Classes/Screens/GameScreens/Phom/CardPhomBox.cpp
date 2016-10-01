#include "CardPhomBox.h"
#include "PhomScreen.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/SpecialCardsBound.h"
#include "Screens/GameScreens/DealCardLayer.h"
#include "Screens/GameScreens/MauBinh/LabelBG.h"

extern int s_card_h;
extern int s_card_w;

#define PHOM_FIRE_CARD_SCALE 0.6f
#define PHOM_FREE_CARD_SCALE 0.7f

#define PHOM_FIRE_CARD_OVERLAP 0.5f
#define PHOM_FIRE_CARD_OVERLAP 0.5f

void CardPhomBox::init(PhomScreen* game_screen, int seat)
{
	_game_screen = game_screen;
	_lbl_score = new LabelBG();
	_lbl_score->autorelease();
	_lbl_score->init("loginscreen/nine/thanh_dangcap.9.png", 9);
	_game_screen->addChild(_lbl_score);
	_lbl_score->setVisible(false);

	_lbl_info = new LabelBG();
	_lbl_info->autorelease();
	_lbl_info->init("loginscreen/nine/thanh_dangcap.9.png", 5);
	_game_screen->addChild(_lbl_info);
	_lbl_info->setVisible(false);

	setSeat(seat);
	if (seat == 0) _card_size = Vec2(s_card_w, s_card_h);
	else _card_size = Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE);
}

void CardPhomBox::addFreeCard(int cardId)
{
	if (checkExist(_freeCards, cardId)) return;
	Card* c = _game_screen->getCard(cardId);
	c->SetSize(_card_size);
	c->GetResource()->setZOrder(_freeCards.size());
	_freeCards.push_back(c);
}

void CardPhomBox::addPhomCard(int cardId)
{
	if (checkExist(_phomCards, cardId)) return;
	Card* c = _game_screen->getCard(cardId);
	c->SetSize(_card_size);
	c->GetResource()->setZOrder(_phomCards.size());
	_phomCards.push_back(c);
}

void CardPhomBox::addEatCard(int cardId)
{
	if (checkExist(_eatCards, cardId)) return;
	Card* c = _game_screen->getCard(cardId);
	if (_seat == 0) c->SetSize(Vec2(s_card_w, s_card_h));
	else c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
	c->GetResource()->setZOrder(_eatCards.size());
	_eatCards.push_back(c);
}

void CardPhomBox::addFireCard(int cardId)
{
	if (checkExist(_firedCards, cardId)) return;

	Card* c = _game_screen->getCard(cardId);
	c->SetSize(Vec2(s_card_w * PHOM_FIRE_CARD_SCALE, s_card_h * PHOM_FIRE_CARD_SCALE));
	c->GetResource()->setZOrder(_firedCards.size());
	_firedCards.push_back(_game_screen->getCard(cardId));
}

void CardPhomBox::layoutFreeCard()
{
	if (_seat == 0)
	{
		CardUtils::layoutListCardCenter(_freeCards, _posFree, .0f);
	}
	else if (_seat == 2)
	{
		CardUtils::layoutListCardCenter(_freeCards, _posFree, .5f);
	}
	else
	{
		CardUtils::layoutListCardCenter_Vertical(_freeCards, _posFree, .6f);
	}

	for (Card* c : _freeCards) {
		c->SetVisible(true);
		c->SelectCardAction(false, false);
	}
}

void CardPhomBox::layoutFireCard()
{
	CardUtils::layoutListCardCenter(_firedCards, _posFire, .5f);
	for (Card* c : _firedCards) {
		c->SetVisible(true);
	}
}

void CardPhomBox::layoutPhomCard()
{
	if (_phomCards.empty()) return;
	Vec2 posPhom;
	Vec2 card_size = _card_size;
	if (_seat == 0)
	{
		posPhom = Vec2(_posFree.x, _posFree.y + card_size.y / 2);
		CardUtils::layoutListCardCenter(_phomCards, posPhom, .0f);
		sortCards(_phomCards);
		int phom_size = _phomCards.size();
		int idx = 0;
		for(auto c : _freeCards)
		{
			NODE(c)->setZOrder(phom_size + idx);
			idx++;
		}
	}
	else if (_seat == 2)
	{
		posPhom = Vec2(_posFree.x, _posFree.y + card_size.y * .5f);
		CardUtils::layoutListCardCenter(_phomCards, posPhom, .5f);
		for (size_t i = 0; i < _phomCards.size(); i++)
		{
			NODE(_phomCards[i])->setZOrder(i);
		}
		for (size_t i = 0; i < _freeCards.size(); i++)
		{
			NODE(_freeCards[i])->setZOrder(_phomCards.size() + i + 1);
		}
	}
	else if (_seat == 1)
	{
		posPhom = Vec2(_posFree.x + card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_phomCards, posPhom, .6f);
	}
	else
	{
		posPhom = Vec2(_posFree.x - card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_phomCards, posPhom, .6f);
		
	}

	for (Card* c : _phomCards) {
		c->SetVisible(true);
	}

}

void CardPhomBox::layoutEatCard()
{
	if (_seat == 0)
	{
		CardUtils::layoutListCardCenter(_eatCards, _posFree, .0f);
	}
	else if (_seat == 2)
	{
		CardUtils::layoutListCardCenter(_eatCards, _posFree, .5f);
	}
	else
	{
		CardUtils::layoutListCardCenter_Vertical(_eatCards, _posFree, .6f);
	}

	for (Card* c : _eatCards) {
		c->SetVisible(true);
	}
}


void CardPhomBox::clearBox()
{
	for (Card* c : _firedCards) {
		c->SetVisible(false);
	}
	for (Card* c : _phomCards) {
		c->SetVisible(false);
	}
	for (Card* c : _freeCards) {
		c->SetVisible(false);
	}
	for (Card* c : _eatCards) {
		c->SetVisible(false);
	}
	_firedCards.clear();
	_phomCards.clear();
	_freeCards.clear();
	_eatCards.clear();
}

void CardPhomBox::layoutBox()
{
	if (_seat == 0) {
		_game_screen->clearAllCardBound();
	}
	
	layoutFreeCard();
	layoutFireCard();
	layoutPhomCard();
	if (_seat == 0) {
		for (size_t i = 0; i < _myPhoms.size(); i++)
		{
			for (size_t ii = 0; ii < _freeCards.size(); ii++)
			{
				if (_myPhoms[i]._cards[0] == _freeCards[ii]->getID())
				{
					Vec2 card_size = _freeCards[ii]->GetSize();
					Vec2 origin = _freeCards[ii]->GetPosition();
					cocos2d::Rect bound = cocos2d::Rect(origin, cocos2d::Size(card_size.x * _myPhoms[i]._cards.size(), card_size.y));
					SpecialCardsBound* card_bound = _game_screen->getFreeSpecialCardBound();
					card_bound->setVisible(true);
					card_bound->setBound(bound);
					card_bound->setPosition(origin);
					break;
				}
			}
		}
	}
}

void CardPhomBox::addPhomToFreeCard(Card* c)
{
	K_VEC_REMOVE(_freeCards,c);
	K_VEC_REMOVE(_firedCards, c);
	K_VEC_REMOVE(_phomCards, c);
	if (_seat == 0) c->SetSize(Vec2(s_card_w, s_card_h));
	else c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
	_freeCards.insert(_freeCards.begin(), c);
	sortCards(_freeCards);
}

void CardPhomBox::sortCards(std::vector<Card*> list_card)
{
	for (size_t i = 0; i < list_card.size(); i++)
	{
		NODE(list_card[i])->setZOrder(i);
	}
}

void CardPhomBox::setMyPhomEat(std::vector<Phom> myPhoms, VecByte myEatCards)
{
	_myPhoms.clear();
	_myPhoms.insert(_myPhoms.begin(), myPhoms.begin(), myPhoms.end());

	_myEatCards.clear();
	_myEatCards.insert(_myEatCards.begin(), myEatCards.begin(), myEatCards.end());
}

void CardPhomBox::clearFreeCard()
{
	_freeCards.clear();
}

void CardPhomBox::addFreeCard(VecCard list_card)
{
	K_VEC_INSERT(_freeCards, list_card);
}

void CardPhomBox::removeFreeCard(int cardId) 
{
	if (_seat != 0) return;
	for (size_t i = 0; i < _freeCards.size(); i++)
	{
		if (_freeCards[i]->getID() == cardId) {
			_freeCards.erase(_freeCards.begin() + i);
		}
	}
}

void CardPhomBox::removeFiredCard(int cardId)
{
	for(size_t i = 0; i < _firedCards.size(); i++)
	{
		if (_firedCards[i]->getID() == cardId)
		{
			_firedCards.erase(_firedCards.begin() + i);
		}
	}
}

void CardPhomBox::selectFreeCard(int cardId) 
{
	if (_seat != 0) return;
	for (Card* c: _freeCards) {
		if (c->getID() == cardId) {
			c->SelectCardAction(true);
		}
	}
}

void CardPhomBox::selectAllCardPhom( std::vector<Phom> myPhoms)
{
	unselectFreeCards();

	for (size_t i = 0; i < myPhoms.size(); i++)
	{
		for (size_t ii = 0; ii < myPhoms[i]._cards.size(); ii++)
		{
			selectFreeCard(myPhoms[i]._cards[ii]);
		}
	}
}

void CardPhomBox::unselectFreeCards()
{
	if (_seat != 0) return;
	for (Card* c : _freeCards) {
		c->SelectCardAction(false);
	}
}

bool CardPhomBox::hasFireCard(int cardId)
{
	for (Card* c: _firedCards) {
		if (c->getID() == cardId) {
			return true;
		}
	}
	return false;
}

void CardPhomBox::fireCard(int cardId) 
{
	Card* fire_card = _game_screen->getCard(cardId);
	fire_card->SetVisible(false);
	if (_seat == 0)
	{
		fire_card->SetSize(Vec2(s_card_w, s_card_h));
	}
	else
	{
		fire_card->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
		fire_card->SetPosition(_posFree);
	}

	Vec2 fire_card_size = Vec2(s_card_w * PHOM_FIRE_CARD_SCALE, s_card_h * PHOM_FIRE_CARD_SCALE);
	Vec2 new_pos = CardUtils::calculateCardPosCenter(fire_card_size, _firedCards.size() + 1, _posFire, PHOM_FIRE_CARD_OVERLAP);
	_game_screen->getDealLayer()->moveCard(fire_card, new_pos, fire_card_size, [this, cardId]() {
		this->addFireCard(cardId);
		this->removeFreeCard(cardId);
		this->layoutBox();
	});

}

VecByte CardPhomBox::getCardSelected()
{
	VecByte rs;
	for (Card* c:_freeCards)
	{
		if (c->isSelected())
		{
			rs.push_back(c->getID());
		}
	}
	return rs;
}

void CardPhomBox::layoutFreeCardsWithAnim(VecByte cards)
{
	if (_seat != 0) return;
	Vec2 card_size = _card_size;
	VecByte old_ids = CardUtils::convertCardsToIds(_freeCards);
	std::vector<Vec2> old_pos = CardUtils::calculateCardListPosCenter(card_size, old_ids.size(), _posFree, .0f);
	mapByteToCard(_freeCards, cards);
	layoutFreeCard();


	std::vector<Vec2> list_pos_next = CardUtils::calculateCardListPosCenter(card_size, cards.size(), _posFree, .0f);
	for (size_t i = 0; i < _freeCards.size(); i++)
	{
		for (size_t ii = 0; ii < old_ids.size(); ii++)
		{
			if (_freeCards[i]->getID() == old_ids[ii])
			{
				_freeCards[i]->SetPosition(old_pos[ii]);
				NODE(_freeCards[i])->runAction(MoveTo::create(.5f, list_pos_next[ii]));
			}
		}
	}
}

void CardPhomBox::mapByteToCard(VecCard& card_array, VecByte byte_array)
{
	card_array.clear();
	for (size_t i = 0; i < byte_array.size(); i++)
	{
		card_array.push_back( _game_screen->getCard(byte_array[i]));
	}
}

void CardPhomBox::setupFreeCard(Card* c)
{
	if(_seat == 0) c->SetSize(Vec2(s_card_w, s_card_h));
	else c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
}

void CardPhomBox::getCard(int cardId, VecByte newFreeCard)
{
	size_t i = 0;
	for(i = 0; i < newFreeCard.size(); i++)
	{
		if (newFreeCard[i] == cardId)
		{
			break;
		}
	}

	layoutFreeCardsWithAnim(newFreeCard);

	Card* card_new = _freeCards[i];
	_game_screen->getDealLayer()->dealCard(0, _freeCards[i], [card_new]() {
		card_new->SetVisible(true);
	});
}

void CardPhomBox::eatCard(int cardId, VecByte newFreeCard)
{
	size_t i;
	for (i = 0; i < newFreeCard.size(); i++)
	{
		if (newFreeCard[i] == cardId)
		{
			break;
		}
	}

	layoutFreeCardsWithAnim(newFreeCard);

	Card* card_new = _freeCards[i];
	_game_screen->getDealLayer()->dealCard(0, _freeCards[i], [card_new]() {
		card_new->SetVisible(true);
	});
}


VecCard CardPhomBox::getFreeCards()
{
	return _freeCards;
}

void CardPhomBox::reset()
{
	resetListCard(_firedCards);
	resetListCard(_phomCards);
	resetListCard(_freeCards);
	resetListCard(_eatCards);
	_firedCards.clear();
	_phomCards.clear();
	_freeCards.clear();
	_eatCards.clear();
	_lbl_score->setVisible(false);
	_lbl_info->setVisible(false);
}

void CardPhomBox::resetListCard(VecCard cards)
{
	for (Card* c : cards)
	{
		c->SetVisible(false);
	}
}

int CardPhomBox::getLastFireCardID()
{
	if (_firedCards.size() > 0)
	{
		return _firedCards.back()->getID();
	}
	else
	{
		return -1;
	}
}

void CardPhomBox::hideFireCards()
{
	for (Card* c : _firedCards)
	{
		c->SetVisible(false);
	}
}

void CardPhomBox::showScore(int score)
{
	Vec2 card_size = _card_size;
	Vec2 pos_score;
	_lbl_score->setText(StringUtils::format("%d", score).c_str());
	cocos2d::Size text_size = _lbl_score->getContentSize();
	if (_seat == 0)
	{
		pos_score = Vec2(_posFree.x, _posFree.y + card_size.y * 3 / 2 + text_size.height / 2);
	}
	else if (_seat == 2)
	{
		pos_score = Vec2(_posFree.x, _posFree.y + card_size.y);
	}
	else if (_seat == 1)
	{
		pos_score = Vec2(_posFree.x + card_size.x * 2 + text_size.width / 2, _posFree.y);
	}
	else
	{
		pos_score = Vec2(_posFree.x - card_size.x - text_size.width / 2, _posFree.y);
	}
	_lbl_score->setVisible(true);
	_lbl_score->setPosition(pos_score);
	_lbl_score->setText(StringUtils::format("%d", score).c_str());
}

void CardPhomBox::showInfo(int type)
{
	switch (type)
	{
	case 0:
		_lbl_info->setText(LangMgr->GetString("u").GetString());
		break;
	case 1:
		_lbl_info->setText(LangMgr->GetString("u_khan").GetString());
		break;
	case 2:
		_lbl_info->setText(LangMgr->GetString("u_den").GetString());
		break;
	case -1:
		_lbl_info->setText(LangMgr->GetString("damaged").GetString());
		break;
	case 3:
		_lbl_info->setText(LangMgr->GetString("lose").GetString());
		break;
	case 4:
		_lbl_info->setText(LangMgr->GetString("win").GetString());
		break;
	}
	Vec2 card_size = _card_size;
	Vec2 pos_info;
	cocos2d::Size text_size = _lbl_info->getContentSize();
	if (_seat == 0)
	{
		//pos_info = Vec2(_posFree.x, _posFree.y + card_size.y * 3 / 2 + text_size.height / 2);
		pos_info = _posFree;
	}
	else if (_seat == 2)
	{
		pos_info = Vec2(_posFree.x, _posFree.y);
	}
	else if (_seat == 1)
	{
		pos_info = Vec2(_posFree.x + card_size.x * 2 + text_size.width / 2, _posFree.y - text_size.height);
	}
	else
	{
		pos_info = Vec2(_posFree.x - card_size.x - text_size.width / 2, _posFree.y - text_size.height);
	}
	
	_lbl_info->setVisible(true);
	_lbl_info->setPosition(pos_info);
}

void CardPhomBox::clearPhomCard()
{
	_phomCards.clear();
}

bool CardPhomBox::checkExist(VecCard cards, byte cardId)
{
	for (Card* c : cards)
	{
		if (c->getID() == cardId)
		{
			return true;
		}
	}
	return false;
}

void CardPhomBox::replacePhom(VecByte new_phom)
{
	int last_idx = 0;
	for (size_t i = 0; i < new_phom.size(); i++)
	{
		bool exist = false;
		for (size_t ii = 0; ii < _phomCards.size(); ii++)
		{
			if (new_phom[i] == _phomCards[ii]->getID())
			{
				last_idx = ii;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
			Card* c = _game_screen->getCard(new_phom[i]);
			c->SetSize(_card_size);
			_phomCards.insert(_phomCards.begin() + last_idx, c);
			last_idx++;
		}

	}
	sortCards(_phomCards);
}