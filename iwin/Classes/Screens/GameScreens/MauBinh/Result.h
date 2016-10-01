#ifndef __RESULT_H__
#define __RESULT_H__
#include "Common/Common.h"
#include "Screens/Object/Card.h"

class Result
{
public:
	static const ubyte LUNG = -2;
	static const ubyte NOT_MAU_BINH = -1;
	static const ubyte SIX_PAIR = 0;
	static const ubyte THREE_STRAIGHT = 1;
	static const ubyte THREE_FLUSH = 2;
	static const ubyte SAME_COLOR_12 = 3;
	static const ubyte SIX_PAIR_WITH_THREE = 4;
	static const ubyte STRAIGHT_13 = 5;
	static const ubyte FOUR_OF_THREE = 6;
	static const ubyte SAME_COLOR_13 = 7;

	/**
	* IC của user.
	*/
	int userID;
	// If win mau binh, then don't care remaining info.
	int winChiMauBinh;
	// Win chi of 1st set.
	int winChi01;
	// Win chi of 2nd set.
	int winChi02;
	// Win chi of 3rd set.
	int winChi03;
	// Win chi by Ace.
	int winChiAce;
	int mauBinhType;
	// Multiple koefficient:
	// 1 default.
	// 2 if win 3 set (an sap ham).
	// 4 if win 3 set of 3 players (an sap ham ca 3 nha).
	int multiK;
	int winMoney;
	int winChi;
	std::vector<Card*> cards;
	ubyte cardIds[13];
	//Array<Card> cardsChi1;
	//Array<Card> cardsChi2;
	//Array<Card> cardsChi3;

	Result() {
		winChiMauBinh = 0;
		winChi01 = 0;
		winChi02 = 0;
		winChi03 = 0;
		winChiAce = 0;
		multiK = 1;
	}

	/**
	* Get sum of win chi.
	*
	* @return sum of win chi.
	*/
	int getWinChi() {
		// If win or lose mau binh.
		if (winChiMauBinh != 0) {
			return winChiMauBinh;
		}

		return (winChi01 + winChi02 + winChi03) * multiK + winChiAce;
	}

	bool isLoseThreeSet() {
		return (multiK == 2 || multiK == 4) && winChi01 < 0; // Cannot use this.getWinChi, because it includes Ace chi.
	}

};


#endif //__RESULT_H__