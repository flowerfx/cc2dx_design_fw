
#include "Screens/GamePlayMgr.h"
#include "Screens/Object/Card.h"
#include "Screens/GameScreens/GameScreens.h"

#include "AnimUtils.h"
#include "FlopCardUI.h"
#include "InputManager.h"
USING_NS_CC;
FlopCardUI::FlopCardUI()
{
	cardFlop = Card::NewInstance(-1, DECK_TYPE::DECK_BAICAO);
	cardFlop->SetActionCommandWithName("FLOP_BAI_CAO");

	cardValue = Card::NewInstance(-1, DECK_TYPE::DECK_BAICAO);
	cardValue->SetActionCommandWithName("FLOP_BAI_CAO");

	_action_flop = nullptr;
}

FlopCardUI::~FlopCardUI()
{
	removeAllChildren();

	delete cardFlop;
	cardFlop = nullptr;
	delete cardValue;
	cardValue = nullptr;

	_card_value.clear();
}

void FlopCardUI::setValueCards(std::vector<unsigned char> ids)
{
	_card_value = ids;
	PASSERT2(_card_value.size() <= 3, "wrong size card value");
	index = 0;
	cardValue->ResetID(_card_value[0], DECK_TYPE::DECK_BAICAO);
	cardFlop->SetPosition(cardValue->GetPosition());

}

bool FlopCardUI::init()
{
	LayerColor::init();
	this->setContentSize(GetGameSize());
	addChild(NODE(cardValue));
	addChild(NODE(cardFlop));
	return true;
}

void FlopCardUI::update(float dt)
{
	if (isVisible())
	{
		HandleTouch(dt);
	}
	cardFlop->Update(dt);
	cardValue->Update(dt);
}

void FlopCardUI::HandleTouch(float dt)
{
	if (InputMgr->GetNumberTouch() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();

		if (current_state_touch == Control::STATE_PRESS)
		{
			//click outside so visible this
			cocos2d::Vec2 wPos = NODE(cardFlop)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
			auto rect = cardFlop->GetBound();
			if (rect.containsPoint(wPos)) 
			{
				NODE(cardFlop)->setTag(1);
				_previous_pos_touch = wPos;
			}
			else
			{
				this->setVisible(false);
				GamePlay->GetCurrentGameScreen()->SetEnableTouch(true);
			}

		}
		else if (current_state_touch == Control::STATE_HOLD)
		{
			if (NODE(cardFlop)->getTag() == 1)
			{
				cocos2d::Vec2 wPos = NODE(cardFlop)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
				Vec2 cur_pos_t = wPos;
				Vec2 delta_pos = Vec2(cur_pos_t.x - _previous_pos_touch.x, cur_pos_t.y - _previous_pos_touch.y);
				Vec2 cur_pos_card = cardFlop->GetPosition();
				CCLOG("BAI CAO : DELTA POS : [%f - %f] ", delta_pos.x, delta_pos.y);
				Vec2 new_pos = Vec2(cur_pos_card.x + delta_pos.x, cur_pos_card.y + delta_pos.y);
				cardFlop->SetPosition(new_pos);
				_previous_pos_touch = cur_pos_t;

			}
		}

	}
	else if (InputMgr->GetNumberTouchRelease() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_RELEASE)
		{
			if (NODE(cardFlop)->getTag() == 1)
			{
				auto pos_flop = cardFlop->GetPosition();
				auto pos_value = cardValue->GetPosition();
				float distance = sqrt(
					((pos_flop.x - pos_value.x) * (pos_flop.x - pos_value.x)) +
					((pos_flop.y - pos_value.y) * (pos_flop.y - pos_value.y)));
				
				if (distance > cardFlop->GetSize().x * 0.15f)
				{
					index++;

					AnimUtils::RunEffectFlopCardReturn(cardFlop, cardValue->GetPosition(), CallFunc::create([this]() {
						this->cardFlop->EventActionFinish();
						if (this->_card_value.size() > 0 && this->index < (int)(this->_card_value.size()) && this->index >= 0)
						{
							this->cardValue->ResetID(this->_card_value[index], DECK_TYPE::DECK_BAICAO);
						}
						if ((size_t)index >= _card_value.size())
						{
							if (this->_action_flop)
							{
								CCLOG("FLOP CARD index : %d with action = false,", index);
								this->_action_flop(false);
							}
							//return;
						}
					}));

					if((size_t)index <= _card_value.size())
					{
						if (_action_flop)
						{
							CCLOG("FLOP CARD index : %d with action = true,", index);
							_action_flop(true);
						}
					}

				}
				else
				{
					AnimUtils::RunEffectFlopCardReturn(cardFlop, cardValue->GetPosition(), nullptr);
				}
			}
			NODE(cardFlop)->setTag(0);
		}
		InputMgr->OnClearTouchReleaseList();
	}

}

void FlopCardUI::SetAction(const std::function<void(bool)> & call_back)
{
	_action_flop = call_back;
}