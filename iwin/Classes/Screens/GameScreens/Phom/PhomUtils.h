#ifndef __PHOM_UTILS_H__
#define __PHOM_UTILS_H__
#include "Common/Common.h"

struct Phom;

class PhomUtils
{
public:
	static bool isSameValue(ubyte id0, ubyte id1);
	static bool isStraightAndSameType(ubyte id0, ubyte id1, int d);
	static bool isContains(VecCard cards, ubyte id);
	static bool canEat(ubyte wantEatID, VecByte currentEatCards, VecByte currentDealCards, VecByte& needCards, std::vector<Phom>& newPhoms, VecByte& newFreeCards);
	static std::vector<Phom> findPhoms(VecByte dealcards, VecByte eatCards, VecByte& freeCards, bool isAddMoreCardIntoPhom);
	static std::vector<Phom> findMaxPhom3WithOneCard(ubyte cardID, VecByte freeCard);
	static bool checkPhomHaveSameCard(Phom phom1, Phom phom2);
	static bool checkPhomHaveSameCard(Phom phom1, Phom phom2, Phom phom3);
	static int sumValuePhom(Phom phom);
	static int getPhomType(VecByte cards3);
	static std::vector<Phom> findPhom_(VecByte dealcards, VecByte& freeCards);
	static void addMoreCardIntoPhom(std::vector<Phom>& phoms, VecByte& freeCard);
	static VecByte findCardCanAddPhom(int typePhom, VecByte cards);
	static bool canFire(ubyte cardID, VecByte dealCards, VecByte eatCards, VecByte& myFreeCard, std::vector<Phom>& newPhoms, VecByte& newFreeCards);
	static bool canMakePhoms(VecByte cards, VecByte currentEatCards, VecByte currentDealCards, std::vector<Phom>& newPhoms, VecByte& newFreeCards, bool isAddMorePhom);
	static int getPhomTypeAlone(VecByte& cards, VecByte eatCards);
};

#endif // __PHOM_UTILS_H__