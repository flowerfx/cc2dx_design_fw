#ifndef __COMMON_H__
#define __COMMON_H__
#include <string>
#include <vector>
#include <functional>
#include "platform/CCPlatformMacros.h"
//#include "Screens/Object/Card.h"
class Card;
#if defined OS_IOS || defined OS_ANDROID_ || defined OS_MAC
typedef char byte;
#endif
typedef unsigned char  ubyte;
typedef std::vector<Card*> VecCard;
typedef std::vector<unsigned char>	VecByte;
typedef std::vector<int>	VecInt;
typedef long long long64;
typedef std::function<void()> Function;

typedef int  s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef short s16;
typedef unsigned short u16;
//typedef char s8;
//typedef unsigned char u8;

#define K_VEC_REMOVE(vec,value) if(!vec.empty()) vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end())
#define K_VEC_CONTAIN(vec,value) (!vec.empty() && (std::find(vec.begin(), vec.end(), value) != vec.end()))
#define K_VEC_INSERT(vec1, vec2) if(!vec2.empty()) vec1.insert(vec1.end(), vec2.begin(), vec2.end())

enum AccountTypes
{
	invalid = -1,
	iwin = 1,
	facebook,
	google,
	quest,
	mobo,
	appleId
};

struct GameResult
{
	ubyte result;
	int ID;
	std::string name;
	int avatar;
	int d_elo;
	int d_exp;
	int d_money;
};

class GameObjectData
{
public:
	int gameId;
	int user;
	int room;

};



#endif //__COMMON_H__