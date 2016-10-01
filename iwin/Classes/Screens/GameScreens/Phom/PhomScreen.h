#ifndef __PHOM_SCREEN_H__
#define __PHOM_SCREEN_H__
#include "Common/Common.h"
#include "cocos2d.h"
#include "Screens/GameScreens/GameScreens.h"

class Card;
class CardPhomBox;
class DealCardLayer;
class SpecialCardsBound;
namespace Utility {
	namespace UI_Widget {
		class ButtonWidget;
	}
}


static const int PHOM_3 = 1;
static const int PHOM_SANH = 2;
static const int NONE = -1;

struct Phom
{

public:
	VecByte			_cards;
	int				_type;
	Phom();
	Phom(int type, VecByte cards);
	bool containCard(byte card);
};


class PhomScreen : public GameScreen
{
private:
	std::vector<Card*>			_card_pools;
	VecByte						_myFreeCards;
	VecByte						_myEatCards;
	VecByte						_myFiredCards;
	VecByte						_myDealCards;
	VecByte						_needCards;
	std::vector<Phom>			_myPhoms;
	std::vector<Phom>			_myPhomsDown;


	int							_totalTimes;

	WidgetEntity *				p_panel_game_card;

	CardPhomBox*				_cardPhomBox[4];

	ButtonWidget*				p_btn_change_phom;
	ButtonWidget*				p_btn_get_card;
	ButtonWidget*				p_btn_eat_card;
	ButtonWidget*				p_btn_fire_card;
	ButtonWidget*				p_btn_down_phom;
	ButtonWidget*				p_btn_send_card;
	ButtonWidget*				p_btn_create_phom;
	std::vector<ButtonWidget*>	_list_btns;
	DealCardLayer*				_deal_card_layer;


	int							_cur_move_player;
	int							_last_move_player;
	bool						_isDownMyPhom;
	int							_card_get_now;
	int							_cur_fire_cardID;
	int							_down_first_player;

	bool						_on_reconnect;
	bool						_is_get_start;
	int							_userIdU;

	/**
	* điểm thắng là điểm nhỏ nhất,khởi tạo là 1000 cho lớn vì thằng thắng là
	* thằng nhỏ điểm nhất
	*/
	int							_winScore, _winnerID;

	/**
	* cái này dùng để check xem trả về finish thằng cuối cùng chưa để chạy hiệu
	* ứng âm thanh
	*/
	int							_countFinishPlayer;

	/**
	* lưu các phỏm đã được gửi tới cho các player dùng trong trường hợp tái
	* phỏm thì thêm lại các lá bài gửi vô cho nó,tránh tình trạng mất bài
	*
	* ứng với userid này là danh sách các danh sách lá bài phỏm đã được gửi bài
	* vào
	*/
	std::map<int, std::vector<std::vector<unsigned char>>> _userSendPhoms;

	std::vector<SpecialCardsBound*>			_card_bounds;

	/**
	* Xác định trước đó mình làm gì mà lỗi (onMoveError)
	*/
	byte									_last_action;

protected:
	virtual void HandleAfterInitPlayer() override;
public:
	// //// các trạng thái hành động
	static const int ACTION_NONE = -1;
	static const int ACTION_FIRE = 0;
	static const int ACTION_GETCARD = 1;
	static const int ACTION_EAT = 2;
	static const int ACTION_DOWN_PHOM = 3;
	static const int ACTION_SEND_CARD = 4;
	static const int ACTION_CHANGE_PHOM = 5;
	static const int ACTION_CREATE_PHOM = 6;

public:

	PhomScreen();
	virtual ~PhomScreen();


	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)override;
	virtual void OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML) override;
	virtual void InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec * dec, int idx);
	virtual void OnBeginFadeIn()override;
	virtual void OnFadeInComplete()override;

	//virtual void CreatePLayer(std::vector<Player*> playerInfos);

	virtual void OnCardTouchDown();
	virtual void OnCardTouchMove();
	virtual void OnCardTouchEnd();

	void onStartGame(ubyte interval, VecByte myCards, int moveFirst, int downFirst);
	void onFire(int currentPlayer, int nextPlayer, int fireCardID, int numCards);
	void onGetCard(int userID, byte cardID);
	void onEatCard(int downFirst);
	void setMovePlayer(int userID);
	void onDownPhom(std::vector<Phom> phoms);
	void onFinish(int finishPlayer, int freeCardScore, VecByte freeCards, int moneyPlayer);
	void onSendCard(std::vector<int> userIDs, VecByte userAddCards, std::vector<std::vector<unsigned char>> userAddPhomCards);
	void OnCardCanSend(VecByte cards);
	void onMoveError(std::string info);
	void OnReconnect(byte interval, int currentMoveTimeRemain,
		int currentPlayer, int lastMovePlayer, int downFirst,
		byte currentFireCardID, std::vector<int> userIDs,
		std::vector<std::vector<unsigned char>> userFiredCards, std::vector<std::vector<unsigned char>> userEatCards,
		std::vector<std::vector<Phom>> userPhomCards, VecByte myCards);
	void onU(int userID, byte u, VecByte cards);

	Card* getCard(int cardId);
	int getPanelBottomY();
	void setDownFirstPlayer(int userID);
	void showGameButton(bool changePhom, bool getCard, bool eat, bool fire, bool downPhom, bool sendCard);
	DealCardLayer* getDealLayer();
	SpecialCardsBound* getFreeSpecialCardBound();
	void clearAllCardBound();
private:
	void initFake();
	void layoutListBtn();


	void updateAndLayoutMyPhomBox();
	void sendMyPhomToServer(byte roomID, byte boardID, std::vector<Phom> currentPhoms, std::map<int, std::vector<std::vector<unsigned char>>> userSendPhoms);
	bool checkHaveEatCardInAllPhom(std::vector<Phom> phoms, VecByte card);
	bool checkHaveEatCard(std::vector<Phom> phoms, byte card);
	bool checkHaveEatCard(Phom phom);
	void addMyCardsToMyBox(bool addSendCard);
	void removeCardInPhomFreeCard();

	void sortFreeCard();
	void removeSameValue(VecByte& byte_array);
	void checkandfixhidecard();
	VecByte addMoreCardIntoCa(VecByte resultDoi, VecByte resultCaSanh, VecByte& resultFree);
	VecByte addcardInOnCa(byte idCard, VecByte& freeCard, int type);
	VecByte cardCanMakeCa(VecByte idCard);
	void addSendPhomsToCardBox(int userID);
	bool checkCanEat(int lastUserNumFiredCards);
	void resetGame();

	void actionFire();
	void actionGetCard();
	void actionEat();
	void actionDownPhom();

	bool isNotChay();
	bool checkSelectCardHaveAllEat();
	std::vector<Phom> getPhomInCardSelect();
	void setInfo(int seat, int type);
	void setScore(int seat, int score);

	void sendCardSend();
	bool checkContainPhom(std::vector<Phom> phoms, Phom phom);

};


#endif // __PHOM_SCREEN_H__