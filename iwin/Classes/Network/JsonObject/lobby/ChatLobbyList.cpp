#include "ChatLobbyList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ChatLobbyList:: ChatLobbyList ()
	{

	}

	ChatLobbyList::~ChatLobbyList()
	{

	}

	rapidjson::Document ChatLobbyList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("gameId", gameId,locator);
		rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < chatList.size(); i++)
		{
			arrlist.PushBack(chatList.at(i)->toJson(), locator);
		}
				document.AddMember("chatList", arrlist, locator);;
		document.AddMember("isRequest", isRequest,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ChatLobbyList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ChatLobbyList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if (document.HasMember("chatList") && !document["chatList"].IsNull() && document["chatList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["chatList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				const rapidjson::Value& c = arrlist[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();
				ChatLobbyItem* l = new ChatLobbyItem();
				l->toData(json);
				chatList.push_back(l);

			}
		}

		if(document.HasMember("isRequest"))
		{
			setIsRequest(document["isRequest"].GetInt());
		}


	}
}
