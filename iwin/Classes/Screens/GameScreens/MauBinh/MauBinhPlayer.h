#ifndef __MAUBINH_PLAYER_H__
#define __MAUBINH_PLAYER_H__
#include "Models/Player.h"
class MauBinhPlayerUI;

class MauBinhPlayer : public Player
{

public:
	MauBinhPlayerUI* ui;

	MauBinhPlayer();
	MauBinhPlayer(Player* p);
	virtual ~MauBinhPlayer();
	virtual void reset();
};

#endif 

