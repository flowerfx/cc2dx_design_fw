

#include "UI/AdvanceUI/WidgetEntity.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Common/GameController.h"

#include "BaiCaoPlayer.h"
#include "BaiCaoScreen.h"
#include "AnimUtils.h"
BaiCaoPlayer::BaiCaoPlayer()
{
	cardValue.clear();
	cards.clear();
	id_already_flip.clear();
}

BaiCaoPlayer::BaiCaoPlayer(Player * p)
{
	CopyValue(p);
	//
	cardValue.clear();
	cards.clear();
	id_already_flip.clear();
}

BaiCaoPlayer::~BaiCaoPlayer()
{
	for (auto c : cards)
	{
		NODE(p_parent)->removeChild(NODE(c),true);
		delete c;
	}
	cards.clear();
	cardValue.clear();
	id_already_flip.clear();
}

void BaiCaoPlayer::CreateCard(WidgetEntity* parent , Vec2 init_pos)
{
	if (cards.size() > 0)
	{
		return;
	}
	p_parent = parent;
	for (size_t i = 0; i < NUM_CAO; i++)
	{
		Card* c = Card::NewInstance(-1, DECK_TYPE::DECK_BAICAO);
		NODE(parent)->addChild(NODE(c));
		NODE(c)->setAnchorPoint(Vec2(0.5,0.5));
		cards.push_back(c);

		Vec2 nPos = NODE(c)->getParent()->convertToNodeSpace(init_pos);
		c->SetPosition(nPos);
	}
}

void BaiCaoPlayer::ClearCard()
{
	for (auto c : cards)
	{
		NODE(p_parent)->removeChild(NODE(c), true);
		delete c;
	}
	cards.clear();
	cardValue.clear();
	id_already_flip.clear();
}

void BaiCaoPlayer::DealCard()
{
	if (cards.size() < 0 || !cards.at(0))
		return;
	float startX = cards[0]->GetSize().x / 2;
	float startY = cards[0]->GetSize().y / 2;
	float time = 0.1f;
	int idx = 0;
	std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(startX, startY, p_parent->GetSize().x, cards.size(), cards[0]->GetSize());
	for (auto c : cards)
	{
		if (c) 
		{
			c->ResetID(-1, DECK_TYPE::DECK_BAICAO);
			AnimUtils::RunEffectChiaBaiCao(
				c, 
				list_pos_move_to[idx],
				(idx + 1) * time, 
				this->IDDB == GameController::myInfo->IDDB,
				nullptr);
			idx++;
		}
	}
}

std::vector<unsigned char> BaiCaoPlayer::GetIdAlreadyFlip()
{
	return id_already_flip;
}

void BaiCaoPlayer::FlipCard(std::vector<char> ids)
{
	if (cards.size() < 0
		|| !cards.at(0)
		|| ids.size() <= 0
		|| cards.size() != ids.size()
		)
	{
		return;
	}
	int idx = 0;
	for (auto id : ids)
	{
		auto c = cards[idx];
		auto current_id = c->getID();
		if (id != -1 && id != cards[idx]->getID() && !IdAlreadyFlip(id))
		{
			AnimUtils::RunEffectFlipBaiCao(cards[idx], id, CallFunc::create([c,id]() {
				c->EventActionFinish();
				c->ResetID(id, DECK_TYPE::DECK_BAICAO);
			}));
			id_already_flip.push_back(id);
		}
		idx++;
	}
}

void BaiCaoPlayer::HideAllCard()
{
	if (cards.size() <= 0)
		return;
	for (auto c : cards)
	{
		c->SetVisible(false);
	}
}

ubyte BaiCaoPlayer::GetCardValueIdx(int idx)
{
	if (cardValue.size() > 0 && idx < (int)cardValue.size())
		return cardValue[idx];
	return 0;
}

bool BaiCaoPlayer::IdAlreadyFlip(ubyte id)
{
	if (id_already_flip.size() < 0)
		return false;
	for (auto i : id_already_flip)
	{
		if (i == id)
			return true;
	}
	return false;
}