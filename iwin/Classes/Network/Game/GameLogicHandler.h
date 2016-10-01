#ifndef __GAME_LOGIC_HANDLER_H__
#define __GAME_LOGIC_HANDLER_H__

#include <vector>
#include "../Core/IMessagehandler.h"

class Message;
class Player;
namespace iwinmesage
{
	class GetMinMaxBoard;
}
class GameLogicHandler
{
public:
	GameLogicHandler();
	virtual ~GameLogicHandler();
	static bool isJoinGame;
	static IMessagehandler* gameHandlers;
	void onShowGameScreen(bool isReconnect, s16 roomId, s16 boardId, int ownerId, int money, std::vector<Player*> players);
	void onKicked(ubyte roomID, ubyte boardID, int kicked, std::string reason);
	void onBonusMoney(ubyte roomID, ubyte boardID, int playerID, int money, std::string des, s64 currMoney);

	/**
	* Set min bet va max bet cho user dang choi game.
	*
	* @param object
	*/
	void onSetBetRange(iwinmesage::GetMinMaxBoard * object);

	void onChatInBoard(ubyte roomid, ubyte boarid, std::string msg, int fromid);

	void onSystemChat(ubyte roomid, ubyte boarid, std::string msg);

	void onReceivedWaitTimeInfo(int sec);
};

#endif // __GAME_LOGIC_HANDLER_H__