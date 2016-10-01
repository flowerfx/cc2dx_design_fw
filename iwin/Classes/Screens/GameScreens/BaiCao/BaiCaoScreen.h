/*!
 * \class TienLenScreen
 *
 * \brief 


 *
 * \author tuandq
 * \date May 2016
 */
#ifndef __GAME_BAICAO_H__
#define __GAME_BAICAO_H__

#include "Screens/GameScreens/GameScreens.h"
#define NUM_CAO 3
class ObjectEntity;
class TienLenPlayer;
class SliderBet;
class Cards;
class BaiCaoPlayer;
class FlopCardUI;
class BaiCaoScreen : public GameScreen
{
private:
	std::vector<char> _list_card_flip;

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
	SliderBet*		p_slider_bet;
protected:
	void DealCardSeat(int s);
	void ShowButtonSetBet(bool param1);
	void flipCard(int seat, vector<char> ids);
	void OnShowBetSlider(bool show);
	void livePlayer();
	void resetQuickPlay();

	virtual void		  HandleAfterInitPlayer() override;
public:
	static BaiCaoScreen* getInstance();

	int totalMoney;

	BaiCaoScreen();
	virtual ~BaiCaoScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)	   override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, Utility::UI_Widget::WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)	override;
	virtual void OnTouchMenuBegin(cocos2d::Point p)	 override;

	virtual void onSomeOneJoinBoard(int seat, Player * player) override;
	virtual void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual void resetGame() override;
	
	virtual void OnCardTouchEnd() override;
	virtual void OnCardTouchDown() override;
	virtual void OnCardTouchMove() override;
	virtual void OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player) override;
	virtual void onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney) override;
	virtual void onBetRange(int min, s64 & max, s64 maxMoneyRoom) override;
	virtual void onSomeOneQuickPlay(int playerID, bool isQuickPlay) override;
	virtual void doSetMoney(int money) override;

	void createDynamicCardLayout(Utility::UI_Widget::WidgetEntity* layerCard, int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func);
	void layoutGame();
	
	void OnBetMoney(int money, int id);
	void ChangeBetMoney(int money, ubyte roomID, ubyte boardID);
	void OnStopGame(ubyte bPlayer, vector<vector<int>> cardR);
	void ShowButtonAction();
	void OnScores(int who1, std::string des1);
	void OnPlaying(ubyte interval, ubyte numPlayer, int cards[6][4]);
	void OnStartGame(vector<ubyte> myCards, ubyte interval1);
	void OnReconnect(
		int limitTime, 
		int remainTime, 
		std::vector< std::vector<int>> playerCards, 
		std::vector<int> playerBetMoneys, int nPlayer, ubyte myCards[], 
		int totalMoney, 
		bool isPlaying);

	void OnFlipCard(int id, std::vector<char> cardID);
	void doFlopCard(std::vector<char> ids);

	void showLbInfoBet();
	void hideAllCard();
	void hideAllFlare();

	void onGetTimeLeft(float timeLeft);
	BaiCaoPlayer* GetMe();
};

#endif //__GAME_BAICAO_H__

