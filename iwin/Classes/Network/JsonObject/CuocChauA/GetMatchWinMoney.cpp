#include "GetMatchWinMoney.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetMatchWinMoney:: GetMatchWinMoney ()
	{

	}

	GetMatchWinMoney::~GetMatchWinMoney()
	{

	}

	rapidjson::Document GetMatchWinMoney::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("myMatchID", myMatchID,locator);
		document.AddMember("money", money,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetMatchWinMoney::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetMatchWinMoney::toData(rapidjson::Document & document)
	{
				if(document.HasMember("myMatchID"))
		{
			setMyMatchID(document["myMatchID"].GetInt());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}


	}
}
