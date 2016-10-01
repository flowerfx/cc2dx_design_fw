#ifndef __PHOM_SERVICE_H__
#define __PHOM_SERVICE_H__
//#include <vector>
#include "Common/Common.h"

struct Phom;

class PhomService
{
public:
	static void refreshPhoms(ubyte  roomID, ubyte  boardID, std::vector<Phom> currentPhoms);
	static std::vector<char> convertPhomArray(std::vector<Phom> phoms);
	static void fire(ubyte  roomID, ubyte  boardID, ubyte  cardID);
	static void getCard(ubyte  roomID, ubyte  boardID);
	static void eatCard(ubyte  roomID, ubyte  boardID, ubyte  eatCardID, VecByte cards);
	static void downPhom(int roomID, int boardID, std::vector<Phom> currentPhoms);
	static void sendCard(ubyte  roomID, ubyte  boardID, VecByte currentFreeCards);
};


#endif //__PHOM_SERVICE_H__