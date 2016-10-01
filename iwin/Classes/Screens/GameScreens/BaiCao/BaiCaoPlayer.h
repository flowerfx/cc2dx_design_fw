#ifndef __BAICAO_PLAYER_H__
#define __BAICAO_PLAYER_H__
#include "Models/Player.h"
class Card;
namespace Utility
{
	namespace UI_Widget
	{
		class WidgetEntity;
	}
}
class BaiCaoPlayer : public Player
{
private:
	cocos2d::LayerColor* uiInfo;
	Utility::UI_Widget::WidgetEntity* p_parent;
protected:
	bool IdAlreadyFlip(ubyte id);
public:
	std::vector<Card*> cards;
    std::vector<unsigned char> cardValue;
	std::vector<unsigned char> id_already_flip;
	
	BaiCaoPlayer();
	BaiCaoPlayer(Player * p);
	virtual ~BaiCaoPlayer();

	void CreateCard(Utility::UI_Widget::WidgetEntity* parent, cocos2d::Vec2 init_pos);
	void ClearCard();
	void DealCard();
	void FlipCard(std::vector<char> ids);
	void HideAllCard();

	ubyte GetCardValueIdx(int idx);
	std::vector<unsigned char> GetIdAlreadyFlip();
};

#endif 

