#ifndef __IWIN_MESSAGE_H__
#define __IWIN_MESSAGE_H__
#include<string>
#include<vector>

class iWinPlayer
{
public:
	 int IDDB ;
	 std::string name;
	 int avatar;
	 long time;
	 std::string lastMEssage ;
public:
	std::string toString();
	iWinPlayer();
	iWinPlayer(
		int IDDB,
		int avatar, 
		std::string name,
		std::string message, 
		int time 
		);
	iWinPlayer(iWinPlayer * chat_msg);
	virtual ~iWinPlayer();
	
};

#define IwinMessage ChatMessage

#endif //__IWIN_MESSAGE_H__