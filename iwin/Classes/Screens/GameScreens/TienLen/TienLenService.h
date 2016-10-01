#ifndef __TIENLEN_SERVICE_H__
#define __TIENLEN_SERVICE_H__
#include "../../../Network/Global/Globalservice.h"
#include "../../../Network/Core/Message.h"
#include "../../../Common/GameController.h"
#include "../../../Network/IwinProtocol.h"
class TienLenService
{

public:
	TienLenService();
	~TienLenService();

	 static void move(ubyte roomID, ubyte boardID,std::vector<char> cards) {
		iwincore::Message* m;
		switch (GameController::getCurrentGameType()) {
		case GAMEID_TIENLEN:
		case GAMEID_TIENLEN_MB:
        {

			m = new iwincore::Message(IwinProtocol::MOVE);
			m->putByte(roomID);
			m->putByte(boardID);
			m->putByte(cards.size());
			for (size_t i = 0; i < cards.size(); i++) {
				m->putByte(cards.at(i));
			}
			break;
                
        }
		case GAMEID_TIENLEN_SOLO:
        {
			m = new iwincore::Message(IwinProtocol::TIENLENSOLO_COMMAND);
			m->putByte(roomID);
			m->putByte(boardID);
			m->putByte(2);
			m->putByte(cards.size());
			for (size_t i = 0; i < cards.size(); i++) {
				m->putByte(cards.at(i));
			}
			break;
        }
        default:
            break;
		}

		GlobalService::sendMessage(m);
	}

	 static void skip(ubyte roomID, ubyte boardID) {

		 iwincore::Message* m;
		switch (GameController::getCurrentGameType()) {
		case GAMEID_TIENLEN:
		case GAMEID_TIENLEN_MB:

			m = new iwincore::Message(IwinProtocol::SKIP_TIENLEN);
			m->putByte(roomID);
			m->putByte(boardID);
			break;
		case GAMEID_TIENLEN_SOLO:
			m = new iwincore::Message(IwinProtocol::TIENLENSOLO_COMMAND);
			m->putByte(roomID);
			m->putByte(boardID);
			m->putByte(3);
			break;
		}
		GlobalService::sendMessage(m);
	}

	 static void forceFinish(ubyte roomID, ubyte boardID) {
		iwincore::Message* m = new iwincore::Message(IwinProtocol::FORCE_FINISH);
		m->putByte(roomID);
		m->putByte(boardID);
		GlobalService::sendMessage(m);
	}
};

#endif //__TIENLEN_SERVICE_H__

