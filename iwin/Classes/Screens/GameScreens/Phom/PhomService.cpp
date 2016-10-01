#include "PhomService.h"
#include "PhomScreen.h"
#include "Network/Core/Message.h"
#include "Network/Global/Globalservice.h"

void PhomService::refreshPhoms(ubyte roomID, ubyte boardID, std::vector<Phom> currentPhoms)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::RESET_PHOM_EAT);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(currentPhoms.size());
	std::vector<char> byte_array = convertPhomArray(currentPhoms);
	if (!byte_array.empty()) {
		for (size_t i = 0; i < byte_array.size(); i++) {
			m->putByte(byte_array[i]);
		}
	}
	GlobalService::sendMessage(m);
}

/**
* chuyển đổi sang mảng được sắp xếp mỗi phỏm cách nhau 1 phần tử -1
*
* @param phoms
* @return
*/
std::vector<char> PhomService::convertPhomArray(std::vector<Phom> phoms) {
	std::vector<char> rs;
	if (phoms.size() > 0) {
		//tính kích thước mảng
		Phom p;
		int num = 0;
		for (size_t i = 0; i < phoms.size(); i++) {
			p = phoms.at(i);
			num += p._cards.size();
		}
		num += phoms.size();//cộng thêm số phỏm để chứ -1
						  // add từng lá bài vô, hết 1 phỏm thì add -1
		
		int k = 0;
		for (size_t i = 0; i < phoms.size(); i++) {
			p = phoms.at(i);
			for (size_t u = 0; u < p._cards.size(); u++) {
				rs.push_back(p._cards.at(u));
			}
			rs.push_back(-1);
		}
	}
	return rs;
}

void PhomService::fire(ubyte roomID, ubyte boardID, ubyte cardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(cardID);
	GlobalService::sendMessage(m);
	//Log.log(TAG + "Đánh", Card.card(cardID, Card.TYPE_BAICAO_XIZACH_PHOM));
}

void PhomService::getCard(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_CARD);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
	//Log.log(TAG + "Rút bài", "");
}

/**
* ăn bài
*
* @param roomID
* @param boardID
* @param eatCardID
*            : con mình muốn ăn
* @param cards
*            : 2 lá kết hợp
*/
void PhomService::eatCard(ubyte roomID, ubyte boardID, ubyte eatCardID,
	VecByte cards) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::EAT_CARD);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(eatCardID);
	for (size_t i = 0; i < cards.size(); i++) {
		m->putByte(cards.at(i));
	}
	GlobalService::sendMessage(m);
	//Log.log(TAG + "Ăn", Card.card(eatCardID, Card.TYPE_BAICAO_XIZACH_PHOM)
	//	+ " ,2 lá kết hợp : " + cards.toString());
}

/**
* hạ phỏm
*
* @param roomID
* @param boardID
* @param currentPhoms
*            : các phỏm hiện tại
*/
void PhomService::downPhom(int roomID, int boardID, std::vector<Phom> currentPhoms) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::HA_PHOM);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(currentPhoms.size());

	for (size_t i = 0; i < currentPhoms.size(); i++)
	{
		for (size_t ii = 0; ii < currentPhoms[i]._cards.size(); ii++)
		{
			m->putByte(currentPhoms[i]._cards[ii]);
		}
		m->putByte(-1);
	}
	GlobalService::sendMessage(m);
	//Log.log(TAG + "Hạ phỏm", currentPhoms.toString());
}

/**
* gửi bài
*
* @param roomID
* @param boardID
* @param currentFreeCards
*            : bài rác hiện tại
*/
void PhomService::sendCard(ubyte roomID, ubyte boardID, VecByte currentFreeCards) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ADD_CARD);
	m->putByte(roomID);
	m->putByte(boardID);
	for (size_t i = 0; i < currentFreeCards.size(); i++) {
		m->putByte(currentFreeCards.at(i));
	}
	GlobalService::sendMessage(m);
	//Log.log(TAG + "Gửi bài",
	//	Card.cards(currentFreeCards, Card.TYPE_BAICAO_XIZACH_PHOM));
}