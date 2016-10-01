#ifndef __GAME_SCREEN_MB_H__
#define __GAME_SCREEN_MB_H__

#include "Screens/GameScreens/GameScreens.h"
#include "Common/Common.h"

class ObjectEntity;
class MauBinhPlayer;
class Result;
class IwinScale9Sprite;
class DealCardLayer;
class LabelBG;
class TimeBarProcess;
class Card;
#define MAUBINH_MAX_CARD 13
#define MAUBINH_NUM_ACES 4

class MauBinhResult : public Layout
{
private:
	std::vector<Card*> _cards;
	Card*				_aces[MAUBINH_NUM_ACES];
	Scale9Sprite*				_card_hl_win;
	Scale9Sprite*				_card_hl_lose;
	TextBMFont*						_lbl_chi;
	Layout*							_transparent;
	LabelBG*						_label_result;

	CC_SYNTHESIZE(Result*, _result, Result);
	int					_size_3;
	int					_size_5;
	void _layoutCards();
	void _sortZ();
	void showHLChiThuong(int startIdx, int endIdx, int chi, int hl_w);
	void showHL(int chi, Vec2 origin, int hl_w);
	void updateTextHL(int chi);
public:	
	virtual ~MauBinhResult();
	virtual bool init();
			void setCards(std::vector<Card*> cards);
			void setCardsState(bool state);
			void showChi1();
			void showChi2();
			void showChi3();
			void showChiAce();
			void showTotal();
			void showMauBinh();
			void reset();
	static MauBinhResult* create();
};


class GroupCard
{
private:
	Layout*							_layout;
	Scale9Sprite*				_hightlight;
	Sprite*						_arrow;
	std::vector<Card*>	_cards;
	int					_limit_num_card;

	bool				_focus;
	cocos2d::Size		_real_size;
	CC_SYNTHESIZE( int, _offset, Offset);

	CC_SYNTHESIZE_READONLY(TextBMFont*, _lbl_type_chi, LblTypeChi);

public:
	GroupCard(const char* name, int num_cards);
	virtual ~GroupCard();
	void				init(const char* name, int num_cards);
	void				addCard(Card* card);
	void				addCards(std::vector<Card*> cards);
	void				removeCard(Card* card);
	void				doLayout();
	void				empty();
	std::vector<Card*>	getCards();
	Card*				getCardHL(Card* card);
	Layout*				getLayout();
	bool				isFull();
	int					getAvailableSlot();
	void				setFocus(bool focus);
	bool				isFocus();
	void				replace(Card* card1, Card* card2);
	void				swap(Card* card1, Card* card2);
	void				showGroup(bool visible);
	cocos2d::Size		getRealSize();
};

class GroupCardDrag : public Layout
{
private:
	std::vector<Card*>			_cards;
public:
	virtual bool init();
	void setCards(std::vector<Card*> cards);
	void reset();
	static GroupCardDrag* create();
};

struct MaubinhAction 
{
	CallFunc*	func;
	float		delay_time;
};

class MauBinhPlayerUI
{
public:
	std::vector<Card*> cards;
	MauBinhResult*		layout_result;
	bool				isUsing;
};

class MauBinhScreen : public GameScreen
{
private:
	WidgetEntity*				_btnAutoBinh;
	WidgetEntity*				_btnFinishBinh;
	WidgetEntity*				_btnSort;

	TimeBarProcess*				_binh_timebar;

	WidgetEntity*				_lock_bg;


	GroupCard*					_group1;
	GroupCard*					_group2;
	GroupCard*					_group3;

	Sprite*						_img_arrow;
	Sprite*						_img_lung;


	std::vector<Card*>		_list_cards;

	std::vector<MaubinhAction>	_list_action_result;

	GroupCardDrag*				_group_drag;

	bool						_stopCheckingMove;


	std::vector<MauBinhPlayerUI*>		_pool_player_ui;
	DealCardLayer*				_deal_card_layer;

	
	CC_SYNTHESIZE(int, playingTime, PlayingTime)
	CC_SYNTHESIZE(int, timeMode, TimeMode)
	CC_SYNTHESIZE(std::string, _txtSapHam, TextSapHam)
	CC_SYNTHESIZE(int, myCardType, MyCardType)
	CC_SYNTHESIZE(bool, _lock_trans, LockTrans)

	static  MauBinhScreen* _instance;
protected:
	virtual void HandleAfterInitPlayer() override;
	
public:
	static MauBinhScreen* getInstance();
	MauBinhScreen();
	virtual ~MauBinhScreen();


	virtual bool InitGame(int game_id) override;

	virtual int Update(float dt)override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnBeginFadeIn()override;
	virtual void CreatePLayer(std::vector<Player*> playerInfos) override;
	
	virtual void onSomeOneLeaveBoard(int leave, int newOwner)override;
	virtual void onSomeOneJoinBoard(int seat, Player* player)override;

	void onStartGame(ubyte roomID, ubyte boardID, ubyte* myCards, int interval, char maubinhType);
	void onAutoBinh(ubyte* cards);
	void onFinishBinh(int userId);
	void onResult(std::vector<Result*> results);
	void onBinhTimer(int interval, int remain);

	void onPlayingTime(int time, int timeType);

	void onBonusMoneyBinh(int userId, long money, std::string des);

	void onTextSapHam(std::string textSapHam);

	void setBoardInfo(int playingTime, int restTime);
	void OnReconnect(ubyte isStatus, VecByte myCards, ubyte limitTime, ubyte restTime, char maubinhType);
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML) override;

	
	virtual void OnCardTouchEnd() override;
	virtual void OnCardTouchDown() override;
	virtual void OnCardTouchMove() override;

	void onHandleCardDown(Card* card, Touch* touch);
	void onHandleCardMove(Card* card, Touch* touch);
	void onHandleCardEnd(Card* card, Touch* touch);
	void onHandleCardCancel(Card* card, Touch* touch);
private:
	void doLayoutGroup(int offset_y, float scale);
	void doLayoutCardComplete();
	std::vector<Card*> getAllCardInGroups();
	void doAlignCenterListCard(bool suggest = true);
	void doAutoBinh();
	void initFake();
	void showLayoutGroup(bool visible);
	void showGroupButton(bool autoBinh, bool finishBinh, bool sort);
	void showCard();
	void showChi1();
	void showChi2();
	void showChi3();
	void showChiAce();
	void showChiResult();
	void playResult();
	void addActionInStack(CallFunc* func, float delayTime);
	void showLayoutComplete(int playerId, bool up);
	void updateGroupDragPos(Vec2 pos);
	void updateGroupDrag();
	void checkGroup(GroupCard* group, Vec2 pos);
	void hideLayoutResult();
	std::vector<Card*> getCardsSelected();
	void resetTouch(bool resetListCard);
	void layoutStartGame(bool deal_or_not = true);
	void addCardToGroup(GroupCard* group, std::vector<Card*> cards);
	void addCardHintToGroup(GroupCard* group);
	void addCardSelectToGroup(GroupCard* group);
	bool checkInternalGroup(GroupCard* group, Card* card, Vec2 pos);
	void swapCardIn2Group(GroupCard* group1, Card* card1, GroupCard* group2, Card* card2);
	Card* findHLCardIn3GroupExcept(Card* card);
	ubyte getHandType(std::vector<Card*> cards, bool isSort);
	
	void stopGame();
	void layoutDealCard();
	void deal();
	void showDealCard(Node* card);
	void showTransparent(bool visible, int opacity = 100);
	void updateLung();
	MauBinhPlayer* getMe();

	MauBinhPlayerUI* getUnusedPlayerUI();

	void showArrow(GroupCard* group);
	void hideArrow();

	void doneDealCard();

	void endResult();
	void showLockScreen();
	void hideLockScreen();
	void reset();
	void onTouchEvent(Ref* ref, Widget::TouchEventType eventType);

	// Logic
	std::vector<Card*> findFlushStraight(std::vector<Card*> cards);
	std::vector<Card*> findFourOfKind(std::vector<Card*> cards);
	std::vector<Card*> findThreeOfKind(std::vector<Card*> cards);
	std::vector<Card*> findFlush(std::vector<Card*> cards);
	std::vector<Card*> findStraight(std::vector<Card*> cards);
	std::vector<Card*> findTwoCard(std::vector<Card*> cards, int type);
	std::vector<Card*> findTowPair(std::vector<Card*> cards);
	std::vector<Card*> findStraightTwoSameSuit(std::vector<Card*> cards);
	std::vector<Card*> findAndGetCard(std::vector<Card*> cards, ubyte chat, ubyte value);
	std::vector<Card*> findStraightTwoNotSameSuit(std::vector<Card*> cards);
	/**
	* tim thang nho nhat trong va khong thuoc card, va kh nam trong doi hay ba
	* cay cardFree: là card sumNumber: so card min can tim
	*/
	std::vector<Card*> cardMinAndNotContandCardSmartOneCard(std::vector<Card*> cardNotSame, std::vector<Card*> cardFree);
	/**
	* so card giong gia tri trong card
	*
	*/
	int numberCardSameValueInCard(std::vector<Card*> cards, ubyte idCard);
	void updateSuggest();
	void enableHint(std::vector<Card*> cards, bool enable = true);
	bool isHaveValue(std::vector<Card*> cards, ubyte value);
	bool isSameChat(std::vector<Card*> cards);
	int numberTwoCard(std::vector<Card*> cards);
	// kiem tra xem co ba cay tu con do kh
	bool haveThreeCardWithId(std::vector<Card*> cards, int idCard);
	std::vector<Card*> cardMinAndNotContandCard(std::vector<Card*> card, std::vector<Card*> cardFree, int sumNumber);
	std::vector<Card*> findMaxCard(std::vector<Card*> cards, int number);
	int compareWith(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithSameType(std::vector<Card*> card1, std::vector<Card*> card2, int type);
	int compareCardByCard(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithOnePair(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithTwoPair(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithThreeOfKind(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithFullHouse(std::vector<Card*> cards1, std::vector<Card*> cards2);
	int compareWithStraight(std::vector<Card*> cards1, std::vector<Card*> cards2);
	bool checkLung(std::vector<Card*> firstHand, std::vector<Card*> middleHand, std::vector<Card*> lastHand);
};

#define GetMauBinhScreen() MauBinhScreen::getInstance()

#endif //__GAME_SCREEN_TLMN_H__

