#include "ChatLobbyItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ChatLobbyItem:: ChatLobbyItem ()
	{

	}

	ChatLobbyItem::~ChatLobbyItem()
	{

	}

	rapidjson::Document ChatLobbyItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("userId", userId,locator);
rapidjson::Value vuserName(userName.c_str(), userName.size());
		document.AddMember("userName", vuserName,locator);
		document.AddMember("userType", userType,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
		document.AddMember("chatTime", chatTime,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ChatLobbyItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ChatLobbyItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("userId"))
		{
			setUserId(document["userId"].GetInt());
		}
		if(document.HasMember("userName"))
		{
			setUserName(document["userName"].GetString());
		}
		if(document.HasMember("userType"))
		{
			setUserType(document["userType"].GetInt());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("chatTime"))
		{
			setChatTime(document["chatTime"].GetInt64());
		}


	}
}
