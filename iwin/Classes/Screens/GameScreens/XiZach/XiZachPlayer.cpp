
#include "UI/AdvanceUI/WidgetEntity.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Common/GameController.h"

#include "XiZachPlayer.h"
#include "XiZachScreen.h"
#include "AnimUtils.h"

XiZachPlayer::XiZachPlayer()
{
	cardValue.clear();
	cards.clear();
}

XiZachPlayer::XiZachPlayer(Player * p)
{
	CopyValue(p);
	//
	cardValue.clear();
	cards.clear();
}

XiZachPlayer::~XiZachPlayer()
{
	for (auto c : cards)
	{
		NODE(p_parent)->removeChild(NODE(c), true);
		delete c;
	}
	cards.clear();
	cardValue.clear();

}

void XiZachPlayer::CreateCard(WidgetEntity* parent, Vec2 init_pos,int numCard)
{
	numCard = 5;
	if (cards.size() > 0)
	{
		return;
	}
	p_parent = parent;
	for (int i = 0; i < numCard; i++)
	{
		Card* c = Card::NewInstance(-1, DECK_TYPE::DECK_BAICAO);
		NODE(parent)->addChild(NODE(c));
		NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
		cards.push_back(c);
		Vec2 nPos = NODE(c)->getParent()->convertToNodeSpace(init_pos);
		c->SetPosition(nPos);

		NODE(c)->setZOrder(numCard -i);
		NODE(c)->setVisible(false);
	}
}

void XiZachPlayer::ClearCard()
{
	for (auto c : cards)
	{
		NODE(p_parent)->removeChild(NODE(c), true);
		delete c;
	}
	cards.clear();
	cardValue.clear();
}

void XiZachPlayer::DealCard(int s,int numCard)
{
	if (cards.size() < 0 || !cards.at(0) || numCard >cards.size())
		return;
	float time = 0.05f;
	int idx = 0;
	seat = s;
	std::vector<Vec2> list_pos_move_to;
	list_pos_move_to = GetPosListCard(numCard);
	int n = 0;
	for (auto c : cards)
	{
		if(n == numCard)
			break;
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
			c->SetVisible(true);
		}
		n++;
	}
	
}

void XiZachPlayer::FlipCard(std::vector<char> ids)
{
	if (cards.size() < 0
		|| !cards.at(0)
		|| ids.size() <= 0
		)
	{
		return;
	}
	std::vector<Vec2> list_pos_move_to;
	list_pos_move_to = GetPosListCard(ids.size());
	int idx = 0;
	//cardValue.clear();
	for (auto id : ids)
	{
		auto c = cards[idx];
		auto current_id = c->getID();
		if (id != cards[idx]->getID() && id != -1)
		{
			if (!cards[idx]->IsVisible())
			{
				AnimUtils::RunEffectChiaBaiCao(
					cards[idx],
					list_pos_move_to[idx],
					0.01f,
					this->IDDB == GameController::myInfo->IDDB,
					CallFunc::create([idx, list_pos_move_to, c, id]() {
						c->EventActionFinish();
						c->ResetID(id, DECK_TYPE::DECK_BAICAO);
					})
				);
			}
			else
			{
				AnimUtils::RunEffectFlipBaiCao(cards[idx], id, CallFunc::create([idx, list_pos_move_to, c, id]() {
					c->EventActionFinish();
					c->ResetID(id, DECK_TYPE::DECK_BAICAO);
				}));
			}
			
		}
		cards[idx]->SetPosition(list_pos_move_to[idx]);
		cards[idx]->SetVisible(true);
		NODE(cards[idx])->setZOrder(cards.size() - idx);
		if (GameController::myInfo->IDDB == IDDB) {
			CCLOG("FlipCard : x:%f-------y:%f", list_pos_move_to[idx].x, list_pos_move_to[idx].y);
		}
		//cardValue.push_back(id);
		idx++;
	}


}

void XiZachPlayer::HideAllCard()
{
	if (cards.size() <= 0)
		return;
	for (auto c : cards)
	{
		c->SetVisible(false);
	}
}

void XiZachPlayer::AddRetrieve(char id)
{
	if (cards.size() < 0 || !cards.at(0) ||   cards.size()<=0)
		return;
	float time = 0.05f;
	int idx = 0;
	int numVisiable = 0;
	for (auto c : cards)
	{
		if (c && c->IsVisible())
		{
			numVisiable++;
		}
		
	}
	//numVisiable++;
	std::vector<Vec2> list_pos_move_to;
	list_pos_move_to = GetPosListCard(numVisiable + 1);
	int numP = 0;
	for (auto c : cards)
	{
		if (numP > numVisiable)
			break;
		if (c && c->IsVisible())
		{
			c->SetPosition(list_pos_move_to[numP]);
			if (GameController::myInfo->IDDB == IDDB) {
				CCLOG("AddRetrieve : x:%f-------y:%f", list_pos_move_to[numP].x, list_pos_move_to[numP].y);
			}
			numP++;
		}
		
		
	}
	if (cards[numVisiable])
	{
		cards[numVisiable]->ResetID(id, DECK_TYPE::DECK_BAICAO);
		AnimUtils::RunEffectChiaBaiCao(
			cards[numVisiable],
			list_pos_move_to[numVisiable],
			 time,
			this->IDDB == GameController::myInfo->IDDB,
			nullptr
		);
		cards[numVisiable]->SetVisible(true);
		NODE(cards[numVisiable])->setZOrder(cards.size() - numVisiable);
	}
	
}

void XiZachPlayer::MoveOwner(vector<char> idsCard)
{
	std::vector<Vec2> list_pos_move_to;
	
	list_pos_move_to = GetPosListCard(idsCard.size());
	for (int i = 0; i < idsCard.size(); i++)
	{
		if (cards[i]->IsVisible())
		{
			cards[i]->ResetID(idsCard[i], DECK_BAICAO);
			cards[i]->SetPosition(list_pos_move_to[i]);
			if (GameController::myInfo->IDDB == IDDB) {
				CCLOG("MoveOwner : x:%f-------y:%f", list_pos_move_to[i].x, list_pos_move_to[i].y);
			}
		}
		else
		{
			if (IDDB == GameController::myInfo->IDDB)
			{
				AddRetrieve(-1);
			}
			else
			{
				AddRetrieve(idsCard[i]);
			}

		}
	}

}

ubyte XiZachPlayer::GetCardValueIdx(int idx)
{
	if (cardValue.size() > 0 && idx < (int)cardValue.size())
		return cardValue[idx];
	return 0;
}

bool XiZachPlayer::IsShowFull()
{
	if (cards.size() <= 0)
		return false;
	for (auto c : cards)
	{
		if (c->IsVisible() && c->getID() == -1)
		{
			return false;
		}
		
	}
	return true;
}

int XiZachPlayer::getNumCardShow()
{
	if (cards.size() <= 0)
		return 0;
	int n = 0;
	for (auto c : cards)
	{
		if (c->IsVisible())
		{
			n++;
		}

	}
	return n;
}

vector<Card*> XiZachPlayer::getCardShow()
{
	vector<Card*> ls;
	for (auto c : cards)
	{
		if (c&& c->IsVisible())
		{
			ls.push_back(c);
		}
	}
	return ls;
}

std::vector<Vec2> XiZachPlayer::GetPosListCard(int numCard)
{
	float startX ;
	float startY;
	std::vector<Vec2> list_pos_move_to;
	float padingC,padingX,padingY;
	padingC =cards[0]->GetSize().x;
	if (seat != 0)
	{
		padingC = padingC*0.4f;
	}
	switch (seat)
	{
		case 0:
			startX = p_parent->getWidth() / 2 - (padingC * numCard / 2);
			startY = cards[0]->GetSize().y/2;
			padingX = padingC;
			padingY = 0;
			break;
		case 1:
			startX = cards[0]->GetSize().x/2;
			startY = p_parent->getHieght() / 2 - (padingC * numCard/2);
			padingY = padingC;
			padingX = 0;
			break;
		case 2:
			startX = p_parent->getWidth()/2 - (padingC * numCard / 2) ;
			startY = p_parent->getHieght() - cards[0]->GetSize().y / 2;
			padingX = padingC;
			padingY = 0;
			break;
		case 3:
			startX = p_parent->getWidth()-cards[0]->GetSize().x / 2;
			startY = p_parent->getHieght() / 2 - (padingC * numCard / 2);
			padingY = padingC;
			padingX = 0;
			break;
	default:
		break;
	}
	
	list_pos_move_to = CardUtils::GetCardPosition(startX, startY, padingX, padingY, numCard);
	return list_pos_move_to;
}

