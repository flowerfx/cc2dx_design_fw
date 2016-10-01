#include "LobbyItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LobbyItem:: LobbyItem ()
	{

	}

	LobbyItem::~LobbyItem()
	{

	}

	rapidjson::Document LobbyItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		document.AddMember("moneyType", moneyType,locator);
		document.AddMember("boardsWaitingCount", boardsWaitingCount,locator);
		document.AddMember("usersWaitingCount", usersWaitingCount,locator);
		document.AddMember("money", money,locator);
		document.AddMember("isEnable", isEnable,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LobbyItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LobbyItem::toData(rapidjson::Document & document)
	{
		if(document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if(document.HasMember("boardsWaitingCount"))
		{
			setBoardsWaitingCount(document["boardsWaitingCount"].GetInt());
		}
		if(document.HasMember("usersWaitingCount"))
		{
			setUsersWaitingCount(document["usersWaitingCount"].GetInt());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}
		if(document.HasMember("isEnable"))
		{
			setIsEnable(document["isEnable"].GetBool());
		}


	}
}
