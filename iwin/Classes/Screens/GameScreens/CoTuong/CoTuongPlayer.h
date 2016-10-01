#ifndef __COTUONG_PLAYER_H__
#define __COTUONG_PLAYER_H__
#include "Models/Player.h"
class CoTuongPlayer : public Player
{
	CC_SYNTHESIZE(int, _quotaTime, QuotaTime)
public:

	CoTuongPlayer();
	CoTuongPlayer(Player* p);
	virtual ~CoTuongPlayer();
	virtual void reset();
};

#endif 

