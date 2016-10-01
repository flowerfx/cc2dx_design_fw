/*!
 * \class TienLenScreen
 *
 * \brief 


 *
 * \author tuandq
 * \date May 2016
 */
#ifndef __GAME_SCREEN_TLMN_H__
#define __GAME_SCREEN_TLMN_H__

#include "Screens/GameScreens/GameScreens.h"
using namespace cocos2d;
class ObjectEntity;
class TienLenPlayer;
class RectProcessTime;
class Card;
enum BTN_TIENLEN
{
	BTN_FIRE_CARD = 0,
	BTN_SORT_CARD,
	BTN_SKIP_CARD,
	BTN_COUNT
};
#define NUMBER_CARD_TL 13
class TienLenScreen : public GameScreen
{
private:
	WidgetEntity * p_layout_fire_card;
	WidgetEntity * p_animate_1;
	WidgetEntity * p_animate_2;
	Vec2 _delta_aligne;
	 //
	bool _forceMove3Bich ;
	bool _forceMoveSmallest ;
	bool _isFirstMatch ;

	// Toi luot di cua.
	int _currentPlayer;
	
	std::vector<char> _selectedCardIDs; // chon nhieu de danh bai nao dung cho se nang len
									 // 10px
									 // Loai card dang duoc chon trong bai cua minh.

	// Loại card dang duoc hien thi tren ban.
	char	_currentCardsType ;
	bool	_autoSkip ;

	std::vector<Card*> _list_fire_card;
	std::vector<Card*> _list_last_card;
	std::vector<Card*> _list_end_card;
	bool IsAutoSelect;

	RectProcessTime* p_rect_process;
	std::vector<Vec2> _list_pos_swap_group;
	void EnableButton(BTN_TIENLEN btn_type , bool value);

	void dealCards();
	void createUI();
	
	void CleanMyCard(bool cleanup = false);
	void CleanFireCard();
	void CleanLastCard();
	void CleanEndCard();

	void addLastCard(std::vector<Card*> lstFirecard);
	void runCardFire(std::vector<unsigned char> cards , int whoMove);

	std::vector<unsigned char> doAutoSelect(bool state_skip = false);
	std::vector<unsigned char> getcardFire();

	bool _load_card_finish;
	int _number_card_remain_after_game;
	std::vector<char> _list_card_create;
private:

	WidgetEntity * p_btn_skip;
	WidgetEntity * p_btn_fire;
	WidgetEntity * p_btn_sort;
	WidgetEntity * p_text_wait;
protected:
	virtual void		  HandleAfterInitPlayer() override;
public:
	char _interval;
	static TienLenScreen* getInstance();
	TienLenScreen();
	virtual ~TienLenScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt) override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML) override;
	virtual void resetSeat() override;
	virtual void resetGame() override;

	virtual void OnCardTouchEnd() override;
	virtual void OnCardTouchDown() override;
	virtual void OnCardTouchMove() override;

	virtual	void onSomeOneJoinBoard(int seat, Player * p) override;
	virtual	void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual void ResetReady() override;

	void start(int whoMoveFirst, char interval, std::vector<unsigned char> myCards);
	TienLenPlayer* getMe();
	void layoutMyCard();
	
	void Move(int whoMove, std::vector<unsigned char> cards, int nextMove);
	void doFireWork();
	void doSkip();
	
	void skip(int whoSkip, int nextMove, bool clearCard);
	bool isClearCurrentCard;
	void onGetTimeLeftAutoStart(char timeStart);
	void Finish(int whoFinish, char finishPos, int money, int exp);
	void ForceFinish();
	void StopGame(int whoShowCards, std::vector<char> cardsShow);
	void OnReconnect(
		int interval, 
		int playerId, 
		std::vector<unsigned char>
		movedCards,
		int next_user_id, 
		std::vector<unsigned char> myCards, 
		int timeLimit,
		std::vector<int> userRank, 
		std::vector<unsigned char> posRank,
		std::vector<bool> userSkip);
	void hideAllRank();
	//
	void ResetAllSkip();
};

#endif //__GAME_SCREEN_TLMN_H__

