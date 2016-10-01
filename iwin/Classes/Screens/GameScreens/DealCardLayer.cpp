
#include "Screens/Object/Card.h"
#include "Screens/ScreenManager.h"
#include "DealCardLayer.h"
bool DealCardLayer::init()
{
	Layer::init();
	setPosition(Vec2::ZERO);
	 
	return true;
}

Card* DealCardLayer::getFreeCard()
{
	Card* free_card = nullptr;  
	for (auto c : _list_cards)
	{
		if (_RES(c)->getNumberOfRunningActions() == 0)
		{
			free_card = c;
			break;
		}
	}
	if (!free_card)
	{
		free_card = Card::NewInstance(0, DECK_TYPE::DECK_MAUBINH);
		free_card->SetPosition(Vec2::ZERO);
		NODE(free_card)->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		free_card->SetStatus(false);
		addChild(NODE(free_card));
		_list_cards.push_back(free_card);
	}
	return free_card;
}

void DealCardLayer::dealCard(float delay, Card* c, std::function<void()> actionDone)
{
	c->SetVisible(false);
	Vec2 cur_card_size = c->GetSize();
	Vec2 desPos = c->GetPosition() + Vec2(cur_card_size.x / 2, cur_card_size.y / 2);
	float des_scale = c->GetScale().x;
	int des_rotate = c->GetAngle();
	cocos2d::Size game_size = GetGameSize();
	Vec2 startPos = Vec2(game_size.width / 2, game_size.height / 2);
	Card* free_card = getFreeCard();
	Vec2 card_size = free_card->GetSize();

	free_card->SetPosition(startPos);
	free_card->SetSize(_card_size);
	free_card->SetAngle(0);
	free_card->SetVisible(true);

	NODE(free_card)->runAction(Sequence::create( DelayTime::create(delay),
		Spawn::create(MoveTo::create(.2f, desPos), ScaleTo::create(.2f, des_scale), RotateBy::create(.2f, des_rotate) , nullptr) ,
		ScaleTo::create(.1f, .0f, des_scale), 
		CallFunc::create([free_card, actionDone]() {
		free_card->SetVisible(false);
		actionDone();
	}), nullptr));

}

void DealCardLayer::moveCard(Card* c, Vec2 des_pos, Vec2 des_size, const std::function<void()>& actionDone)
{
	Card* free_card = getFreeCard();
	Vec2 cur_card_size = c->GetSize();
	Vec2 top_right = NODE(c)->getParent()->convertToWorldSpace(c->GetPosition() + c->GetSize());
	Vec2 bot_left = NODE(c)->getParent()->convertToWorldSpace(c->GetPosition());
	Vec2 card_size = top_right - bot_left;
	c->SetVisible(false);
	free_card->SetVisible(true);
	free_card->SetStatus(true);
	free_card->ResetID(c->getID(), (DECK_TYPE)c->GetDeckType());
	free_card->SetSize(card_size);
	free_card->SetAngle(c->GetAngle());
	free_card->SetPosition(bot_left + Vec2(card_size.x / 2, card_size.y / 2));
	float scalex = (des_size.x * c->GetScale().x) / cur_card_size.x;
	float scaley = (des_size.y * c->GetScale().y) / cur_card_size.y;
	Vec2 target_pos = des_pos + Vec2(des_size.x / 2, des_size.y / 2);

	NODE(free_card)->runAction(Sequence::create(Spawn::create(MoveTo::create(0.2f, target_pos), ScaleTo::create(0.2f, scalex, scaley), nullptr),
		CallFunc::create([free_card, actionDone]()
	{
		free_card->SetVisible(false);
		actionDone();
	}), nullptr));

}
