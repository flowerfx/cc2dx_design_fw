#include "LobbyChatMessage.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LobbyChatMessage:: LobbyChatMessage ()
	{

	}

	LobbyChatMessage::~LobbyChatMessage()
	{

	}

	rapidjson::Document LobbyChatMessage::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("gameId", gameId,locator);
		document.AddMember("userId", userId,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LobbyChatMessage::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LobbyChatMessage::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if(document.HasMember("userId"))
		{
			setUserId(document["userId"].GetInt());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
