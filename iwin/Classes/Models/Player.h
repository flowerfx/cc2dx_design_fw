#ifndef __PLAYER1_H__
#define __PLAYER1_H__

#include "User.h"

class Player
{
public:
	long IDDB;
	std::string name;
	int group;
	std::string showName;
	long	exp;
	u64	money;
	u64   ruby;
	int	avatar;
	bool isReady;
	int giftID;
	s16 roomID;
	s16 boardID;
	s16 gameID;
	bool hasBoardPassword;
	ubyte	finishPosition;
	User*	userProfile;
	std::string shortMoneyString;
	std::string longMoneyString;
	std::string onlineInfo;
	int lv;
	int percent;
	int quotaTime;
	int elo;
	bool isQuickPlay;
	bool	isActived;
public:
	static const int PLAYER_OFFLINE = -1;
	static const int PLAYER_ONLINE = 0;
	static const int PLAYER_PLAYING = 1;

public:
	Player();
	Player(Player * p);
	void CopyValue(Player * p);
	virtual ~Player();

	std::string getLevelDetail();
	bool		equals(Player obj);
	std::string toString();
	void		setMoney(s64 amount);
	void		addMoney(s64 amount);
	void		setRuby(s64 amount);
	std::string getDescription();
	std::string getSubDescription();
	std::string getContent();
	bool isLoggedIn();
	void setExp(int exp);
	int getPercent();
	int getLevel();
	long getExp();
	s64 getMoney();
	void setName(std::string new_name);
	void setUserProfile(User* data);
	virtual void reset();
};

#endif //__PLAYER_H__