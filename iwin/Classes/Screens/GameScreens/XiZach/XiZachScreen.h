/*!
* \class XiZachScreen
*
* \brief


*
* \author tuandq
* \date May 2016
*/
#ifndef __GAME_XIZACH_H__
#define __GAME_XIZACH_H__

#include "Screens/GameScreens/GameScreens.h"
#include "UI/AdvanceUI/wLayout.h"

class ObjectEntity;
class SliderBet;
class Cards;
class XiZachPlayer;
class FlopCardUI;
class XiZachScreen : public GameScreen
{
private:

	std::vector<Vec2>			_list_pos_layer_card;
	std::vector<WidgetEntity*>	ls_players_card;
	std::vector<WidgetEntity*>	ls_players_bet_info;

	std::vector<Card*> ls_cards;

	WidgetEntity * p_panel_custom;
	WidgetEntity * p_panel_slider;

	FlopCardUI*		flopCard;

	WidgetEntity*	btn_quick_play;
	WidgetEntity*	btn_bet_money;
	WidgetEntity*	btn_flop_card;
	WidgetEntity*	btn_done;
	LayoutWidget* btn_retrieve;
	SliderBet*		p_slider_bet;
	int whoMove ;
	vector<unsigned char> lsCardValue;
	vector<string> _scores;
	bool _isAllowRetrieveCard = false;

	void DealCardSeat(int s, int num = 2);
	void ShowButtonSetBet(bool param1);

	void flipCard(int seat, vector<char> ids);
protected:
	virtual void		  HandleAfterInitPlayer() override;
public:
	static XiZachScreen* getInstance();

	int totalMoney;

	XiZachScreen();
	virtual ~XiZachScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)	   override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, Utility::UI_Widget::WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)	override;
	virtual void OnTouchMenuBegin(cocos2d::Point p)	 override;

	virtual void onSomeOneJoinBoard(int seat, Player * player) override;
	virtual void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual void OnFadeInComplete()	 override;
	virtual void resetGame() override;

	virtual void OnCardTouchEnd() override;
	virtual void OnCardTouchDown() override;
	virtual void OnCardTouchMove() override;
	virtual void OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player) override;
	virtual void onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney) override;
	virtual void onBetRange(int min, s64 & max, s64 maxMoneyRoom) override;

	void createDynamicCardLayout(Utility::UI_Widget::WidgetEntity* layerCard, int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func);
	void layoutGame();

	void showLbInfoBet();
	void hideAllCard();
	void hideAllFlare();
	void ShowButtonAction(bool btnQuickPlay,bool btnDonde,bool btnFlop,bool btnBet);

	XiZachPlayer* GetMe();

	void ShowQuickPlay(int user, bool isQuickPlay);
	void OnBetMoney(int money, int user);
	void OnStopGame(ubyte numPlayers, vector<vector<int>> cardR);
	void OnStartGame(vector<char> myCards, ubyte interval, int user);
	void SetScores(int user, string des1);
	void OnSetMove(int user, ubyte time);
	void OnSetMoveOwner(int user, vector<char> cardsID);
	void OnReconnect(
		int limit_time, 
		int remain_time, 
		vector<char> myCards, 
		int crMoveID, 
		vector<vector<int>> playerCards, 
		vector<int> playerBetMoneys, 
		int nPlayer, 
		bool isPlaying, 
		bool isOwnerTurn);
	void OnPlaying(ubyte interval, ubyte numPlayers, vector<vector<int>> cards);
	void OnRetrieveCard(int user, ubyte  idCard);
	void ShowButtonRetrieveCard(bool show);
	void OnFlipCard(int id, std::vector<char> cardID);
	void ShowScores(int seat);
	void ShowFubgeCard(int seat,bool isShow);
	void HideAllFudge();
	bool checkNullCard(vector<char> cards);
	
};

#endif //__GAME_XIZACH_H__

