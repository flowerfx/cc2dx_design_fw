#ifndef __BOARD_H__
#define __BOARD_H__
#include "Common/Common.h"

class Board 
{
public:
	enum TYPE
	{
		TYPE_4 = 4,
		TYPE_2 = 2,
		TYPE_6 = 6
	};
	 ubyte id;
	 int nPlayer;
	 bool isPass;
	 int money;
	 std::string strMoney;
private:
	 std::string strNoPlayer;
public:
	/**
	 * Gia tri mac dinh la 4.
	 */
	int type;

	std::string getMoney();
	std::string getNoPlayer();
	Board(ubyte _id,
		int _nPlayer,
		bool _isPass,
		int _money,
		std::string _strMoney,
		std::string _strNoPlayer);
	Board(Board * b);
	virtual ~Board();

};

#endif // __BOARD_H__
