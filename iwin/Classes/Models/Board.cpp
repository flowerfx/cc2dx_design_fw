#include "Board.h"
#include "Utils.h"

Board::Board(ubyte _id,
			int _nPlayer,
			bool _isPass,
			int _money,
			std::string _strMoney,
			std::string _strNoPlayer)
{
	type = TYPE::TYPE_4;

	id = _id;
	nPlayer = _nPlayer;
	isPass =_isPass;
	money = _money;
	strMoney = _strMoney;
	strNoPlayer = _strNoPlayer;
}

Board::Board(Board * b)
{
	id = b->id;
	nPlayer = b->nPlayer;
	isPass = b->isPass;
	money = b->money;
	strMoney = b->strMoney;
	strNoPlayer = b->strNoPlayer;
	type = b->type;
}

Board::~Board()
{

}

std::string Board::getMoney() 
{
	if (strMoney == "") 
	{
		 strMoney = Utils::formatNumber(money);
	}
	return strMoney;
}

std::string Board::getNoPlayer() 
{
	if (strNoPlayer == "")
	{
		strNoPlayer = std::to_string(nPlayer);
	}
	return strNoPlayer;
}

