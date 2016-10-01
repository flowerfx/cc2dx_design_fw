#include "ChatMessage.h"

ChatMessage::ChatMessage()
{
	fromID = -1;
	avatar = 0;
	fromName = "";
	message = "";
	isRead = isReply = "";
	time = 0;
	typeN = 0;

	toID = -1;
	toavatar = -1;
	toName = "";
}

ChatMessage::ChatMessage(int _fromIDId,
	int _avatar,
	std::string _fromName,
	std::string _message,
	std::string _isRead,
	std::string _isReply,
	int _time,
	int _typeN,
	int _toID,
	int _toavatar,
	std::string _toName) :
						fromID(_fromIDId),
						avatar(_avatar),
						fromName(_fromName),
						message(_message),
						isRead(_isRead),
						isReply(_isReply),
						time(_time),
						typeN(_typeN),
						toID(_toID),
						toavatar(_toavatar),
						toName(_toName)
{
}

ChatMessage::ChatMessage(ChatMessage * chat_msg)
{
	fromID = chat_msg->fromID;
	avatar = chat_msg->avatar;
	fromName = chat_msg->fromName;
	message = chat_msg->message;
	isRead = chat_msg->isRead;
	isReply = chat_msg->isReply;
	time = chat_msg->time;
	typeN = chat_msg->typeN;

	toID = chat_msg->toID;
	toavatar = chat_msg->toavatar;
	toName = chat_msg->toName;
}

ChatMessage::~ChatMessage()
{

}

std::string ChatMessage::toString() {
	return (
		"fromID=" + std::to_string(fromID) +
		" ,avatar=" + std::to_string(avatar) +
		" ,fromName=" + fromName +
		" ,message=" + message +
		" ,isRead=" + isRead +
		" ,isReply=" + isReply +
		" ,time=" + std::to_string(time) +
		" ,typeN=" + std::to_string(typeN) +
		" ,toID=" + std::to_string(toID) +
		" ,toavatar=" + std::to_string(toavatar) +
		" ,toName=" + toName);
}
