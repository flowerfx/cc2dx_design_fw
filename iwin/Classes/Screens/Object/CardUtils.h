#ifndef __CARD_UTILS_H__
#define __CARD_UTILS_H__

#include <vector>
#include "cocos2d.h"
#include "Common/common.h"
#include "Screens/Object/Card.h"
#include "Common/GameController.h"
USING_NS_CC;

class Card;
class TienLenPlayer;
//enum GameType;

class CardUtils
{
private:
	static int demDoiThong(std::vector<unsigned char> bai);
	static void setSelect(Card* c, bool isSelect);
public:

	enum CardCombo
	{
		TYPE_INVALID = -1,
		TYPE_1 = 0,
		TYPE_SANH,
		TYPE_2,
		TYPE_3,
		TYPE_3DOITHONG,
		TYPE_4DOITHONG,
		TYPE_TUQUY,
		TYPE_2TUQUY,
	};

	static int ConvertToCardID(NAME_CARD name, ELEMENT_CARD element, GameType gameType);

	static std::pair<NAME_CARD, ELEMENT_CARD> ConvertToCardName(int id, GameType gameType);

	//id1 is > id2 or otherwise
	bool CompareCard(int id1, int id2, GameType gameType);

	static std::map<NAME_CARD , std::map<ELEMENT_CARD, int>> GetNumberCardSameName(std::vector<Card*> cards);

	static std::map<ELEMENT_CARD, std::map<NAME_CARD, int>> GetNumberCardSameType(std::vector<Card*> cards);

	static std::vector<unsigned char> FindSeries(std::vector<Card*> cards, int number_series, int highestNumber, GameType gameType, int begin_idx = 0);

	static int GetNameSizeID(NAME_CARD card, GameType gameType);

	/**
	* Hàm đổi vị trí 2 con bài. Nếu đến cuối thì đổi vòng lên đầu, nếu đầu thì
	* vòng cuối. Viec kiem tra dau cuoi da duoc kiem tra trong ham.
	*
	* @param cIndex
	* @param nIndex
	*/
	static bool swap2Cards(std::vector<Card*>* cards, int cIndex, int nIndex);

	/**
	* Sort bài tăng dần.
	*/
	static void sortByOrder(std::vector<Card*>& cards);

	/**
	* sort bai tang dan theo kieu game catte
	*
	* @param cards
	*/
	static void sortByOrderTypeCatte(std::vector<Card*>* cards);

	/**
	* sort bai tang dan theo kieu game catte
	*
	* @param cards
	*/
	static void sortByOrderTypeCatte(unsigned char* cards, int len);

	/**
	* Sort theo chất.
	*
	* @param cards
	*/
	static void sortBySuit(std::vector<Card*>* cards);

	/**
	* dinh nghia lai lon la gi trong viec sap xep dong chat tang dan lon hon la
	* 1 nho hon la 0
	*/
	static int compareBySuitOrder(Card* o1, Card* o2);

	/**
	* Sort theo chất và tăng dần
	* doi voi tien len se dem heo ra ngoai
	* doi voi mau binh khong dem at ra ngoai
	* @param cards
	*/
	static void sortBySuitOrder(std::vector<Card*>& cards);


	static void sort(std::vector<unsigned char>& bai);

	static void sortTypeCatte(std::vector<unsigned char> bai);

	static int setCardPosition(int xCenter, int y, int maxWidth, std::vector<Card*> cards);

	static bool checkForceFinish(std::vector<Card*>* cards, bool newgame);

	static CardCombo getType(std::vector<unsigned char> bai);

	static bool is1(std::vector<unsigned char> bai);
	static bool is2TuQuy(std::vector<unsigned char> bai);
	static bool isSanh(std::vector<unsigned char> bai);
	static bool IsHeo(std::vector<unsigned char> cards);
	static bool isDoi(std::vector<unsigned char> bai);
	static bool is3la(std::vector<unsigned char> bai);
	static bool isTuQuy(std::vector<unsigned char> bai);
	static bool is3DoiThong(std::vector<unsigned char> bai);
	static bool is4DoiThong(std::vector<unsigned char> bai);
	static int is5DoiThong(std::vector<unsigned char> bai);
	static bool available(std::vector<unsigned char> bai_sapdanh, int type_bai_sapdanh, std::vector<unsigned char> baidanh, int type_baidanh);
	static bool availableXam(std::vector<unsigned char> bai_sapdanh, int type_bai_sapdanh, std::vector<unsigned char> baidanh, int type_baidanh);

	static std::vector<unsigned char> autoSelect(std::vector<unsigned char> currentCards, TienLenPlayer* player);

	static bool isHeoEnd(std::vector<Card*> cards, std::vector<Card*> selectedCards);
	static std::vector<unsigned char> findDoubleSequence(int highestCard, std::vector<Card*> cards, int noPair);
	/**
	* Tìm và chọn bài để đánh tứ quý hiện tại
	*
	* @param highestCards
	*            : quân bài hiện hành
	* @param player
	*            : người chơi sẽ tìm
	* @return true : có ; false : không thấy
	*/
	static std::vector<unsigned char> findAndSelectQuart(int highestCards, std::vector<Card*> cards);

	/**
	* Tìm tứ quý 2
	*
	* @param highestCards
	*            : quân bài hiện hành
	* @param player
	*            : người chơi sẽ tìm
	* @return true : có ; false : không thấy
	*/
	static std::vector<unsigned char> findAndSelectQuart2(std::vector<Card*> cards);

	/**
	* Tìm và chọn 1 đôi
	*
	* @param highestCard
	*            : Quan bai lon nhat can vuot qua. -1 neu khong co.
	* @param player
	*            : Nguoi choi can tim va chon giup
	* @return true : nếu đã tìm và chọn được, false : nếu không tìm thấy đôi
	*         nào
	*/
	static std::vector<unsigned char> findAndSelectTwin(int highestCard, std::vector<Card*> cards);

	/**
	* Tìm 3 lá cùng số ( 3 cây ) cho người chơi player
	*
	* @param highestCard
	*            : lá cao nhat hiện tại
	* @param player
	*            : người chơi được tìm
	* @return : true nếu có 3 lá lớn hơn, false : nếu không tìm thấy
	*/
private:
	static std::vector<unsigned char> findAndSelectTriple(int highestCard, std::vector<Card*> cards);

	static std::vector<unsigned char> findAndSelectTripleMB(std::vector<unsigned char> highestCard, std::vector<Card*> cards);

	/**
	* Tìm và chọn lóc để đánh
	*
	* @param highestCard
	*            : La bai lon nhat can vuot qua. = -1 : khong co la nao.
	* @param player
	*            : Nguoi choi can tim va chon giup
	* @return : true : tìm được ; false : không tìm thấy
	*/
	static std::vector<unsigned char> findAndSelectSeries(std::vector<unsigned char> currentCard, int highestCard, std::vector<Card*> cards, int length);

public:

	/**
	* dem so con heo trong bai
	*
	* @param cards
	*            mang card cua myCards
	* @return
	*/
	static ubyte numHeoinCards(std::vector<Card*> cards);

	static VecByte convertCardsToIds(std::vector<Card*> cards);


	static void sortByValue_CRChB(bool asc, VecByte& cards);

	static void sortByValue_CRChB_Cards(bool asc, std::vector<Card*>& cards);

	static  std::vector<cocos2d::Vec2> AligneCardPosition(float leftX, float y, float desX, int number_cards, Vec2 size_card, bool aligneX = true);

	static  std::vector<cocos2d::Vec2> GetCardPosition(float leftX, float y, float desX, float desY,int sizeCard);

	static  Card* getCardId(std::vector<Card*> cards,char id);
	static  std::vector<Card*> getCardId(std::vector<Card*> cards, std::vector<unsigned char> lsid);
	

	static int compareWithStraight(std::vector<Card*> cards1, std::vector<Card*> cards2);
	static void layoutListCardFromLeft(std::vector<Card*> list_cards, int offset_x, Vec2 pos);
	static void doAlignLeftListCard(std::vector<Card*> list_cards, int offset_x, Vec2 pos, int start_id, int end_id);
	static std::vector<Vec2> calculateCardListPosCenter(Vec2 card_size, int num_card, Vec2 center, float percent_overlap);
	static Vec2 calculateCardPosCenter(Vec2 card_size, int num_card, Vec2 center, float percent_overlap);
	static void layoutListCardCenter(std::vector<Card*> list_cards, Vec2 center, float percent_overlap);
	static void layoutListCardCenter_Vertical(std::vector<Card*> list_cards, Vec2 center, float percent_overlap);
	static bool checkCardTouch(Card* card, Vec2 touch);
	//@PDT: just for test
	static const int num_card_test = 52;
	static std::vector<unsigned char> _test_cards;
	static void initCardTest();
	static ubyte getCardTestRandom();
	static ubyte getCardTest(int number, int type);
	static ubyte getCardTest(int value);
	static std::vector<unsigned char> autoSelecWhenTouchLTMN(TienLenPlayer* player, std::vector<unsigned char> cardSelect, std::vector<unsigned char> curentCards);


};

#endif // __CARD_UTILS_H__