#include "MauBinhService.h"
#include "Network/Core/Message.h"
#include "Network/Global/Globalservice.h"

void MauBinhService::autoBinh(ubyte roomID, ubyte boardID) {
	//Dinh nghia sau.
	iwincore::Message* m = new iwincore::Message(47);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(25);
	GlobalService::sendMessage(m);
}

void MauBinhService::doFire(ubyte roomID, ubyte boardID, unsigned char* cardIDs)
{
	iwincore::Message* m = new iwincore::Message(47);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(30);
	for (int i = 0; i < 13; i++) {
		m->putByte(cardIDs[i]);
	}
	GlobalService::sendMessage(m);

}

