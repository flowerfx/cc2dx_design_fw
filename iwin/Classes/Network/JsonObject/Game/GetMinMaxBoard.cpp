#include "GetMinMaxBoard.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetMinMaxBoard:: GetMinMaxBoard ()
	{

	}

	GetMinMaxBoard::~GetMinMaxBoard()
	{

	}

	rapidjson::Document GetMinMaxBoard::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("gameId", gameId,locator);
		document.AddMember("roomID", roomID,locator);
		document.AddMember("boardID", boardID,locator);
		document.AddMember("minMoney", minMoney,locator);
		document.AddMember("maxMoney", maxMoney,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetMinMaxBoard::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetMinMaxBoard::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if(document.HasMember("roomID"))
		{
			setRoomID(document["roomID"].GetInt());
		}
		if(document.HasMember("boardID"))
		{
			setBoardID(document["boardID"].GetInt());
		}
		if(document.HasMember("minMoney"))
		{
			setMinMoney(document["minMoney"].GetInt());
		}
		if(document.HasMember("maxMoney"))
		{
			setMaxMoney(document["maxMoney"].GetInt());
		}


	}
}
