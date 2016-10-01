#ifndef __XIZACH_PLAYER_H__
#define __XIZACH_PLAYER_H__
#include "Models/Player.h"
class Card;
namespace Utility
{
	namespace UI_Widget
	{
		class WidgetEntity;
	}
}
class XiZachPlayer : public Player
{
private:
	cocos2d::LayerColor* uiInfo;
	Utility::UI_Widget::WidgetEntity* p_parent;
	int seat;
public:
	std::vector<Card*> cards;
	std::vector<char> cardValue;

	XiZachPlayer();
	XiZachPlayer(Player * p);
	virtual ~XiZachPlayer();

	void CreateCard(Utility::UI_Widget::WidgetEntity* parent, cocos2d::Vec2 init_pos, int numCard);
	void ClearCard();
	void DealCard(int seat, int numCard);
	void FlipCard(std::vector<char> ids);
	void HideAllCard();
	void AddRetrieve(char id);
	void MoveOwner(std::vector<char> idsCard);
	ubyte GetCardValueIdx(int idx);
	bool IsShowFull();
	int getNumCardShow();
	std::vector<Card*> getCardShow();
	std::vector<cocos2d::Vec2> GetPosListCard(int numCard);

};

#endif 

