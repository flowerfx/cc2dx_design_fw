#ifndef __ACHIEVEMENT_H__
#define __ACHIEVEMENT_H__
#include<string>
#include<vector>

class Achievement
{
public:
	 char gameId;
	 int exp, win, lose, draw, givenUp, eloNumber, eloItemId;
	 std::string elo;
	Achievement(char gameId, int exp, int win, int lose, int draw,int givenUp, int eloNumber, std::string elo, int eloItemId);
	Achievement(Achievement * achieve);
	virtual ~Achievement();
	
};

#endif //__ACHIEVEMENT_H__