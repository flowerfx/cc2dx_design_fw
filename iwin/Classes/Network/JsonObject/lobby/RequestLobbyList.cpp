#include "RequestLobbyList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestLobbyList:: RequestLobbyList ()
	{

	}

	RequestLobbyList::~RequestLobbyList()
	{

	}

	rapidjson::Document RequestLobbyList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("gameId", gameId,locator);
		document.AddMember("moneyType", moneyType,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestLobbyList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestLobbyList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if(document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}


	}
}
