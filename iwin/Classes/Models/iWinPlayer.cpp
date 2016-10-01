#include "iWinPlayer.h"

iWinPlayer::iWinPlayer()
{
	IDDB = -1;
	name = "";
	avatar = 0;
	time = 0;
	lastMEssage = "";
}

iWinPlayer::iWinPlayer(
	int _IDDB,
	int _avatar,
	std::string _name,
	std::string _message,
	int _time) :
		IDDB(_IDDB),
		avatar(_avatar),
		name(_name),
		lastMEssage(_message),
		time(_time)
{
}

iWinPlayer::iWinPlayer(iWinPlayer * player)
{
	IDDB = player->IDDB;
	name = player->name;
	avatar = player->avatar;
	time = player->time;
	lastMEssage = player->lastMEssage;
}

iWinPlayer::~iWinPlayer()
{

}

std::string iWinPlayer::toString() {
	return (
		"IDDB=" + std::to_string(IDDB) +
		" ,name=" + name + 
		" ,avatar=" + std::to_string(avatar) +
		" ,time=" + std::to_string(time) +
		" ,lastMEssage=" + lastMEssage
		);
}
