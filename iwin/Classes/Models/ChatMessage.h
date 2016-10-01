#ifndef __IWIN_MESSAGE_H__
#define __IWIN_MESSAGE_H__
#include<string>
#include<vector>

class ChatMessage
{
public:
	int fromID;
	int avatar;
	std::string fromName ;
	std::string message ;
	std::string isRead, isReply;
	long time;
	int typeN;

	int toID ;
	int toavatar;
	std::string toName ;
public:
	std::string toString();
	ChatMessage();
	ChatMessage(int gameId, 
		int avatar, 
		std::string 
		fromName, 
		std::string message, 
		std::string isRead, 
		std::string isReply, 
		int time, 
		int typeN, 
		int toID,
		int toavatar, 
		std::string toName);
	ChatMessage(ChatMessage * chat_msg);
	virtual ~ChatMessage();
	
};

#define IwinMessage ChatMessage

#endif //__IWIN_MESSAGE_H__