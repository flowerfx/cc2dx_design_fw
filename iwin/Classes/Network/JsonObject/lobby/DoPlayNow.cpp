#include "DoPlayNow.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	DoPlayNow:: DoPlayNow ()
	{

	}

	DoPlayNow::~DoPlayNow()
	{

	}

	rapidjson::Document DoPlayNow::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		document.AddMember("moneyType", moneyType,locator);
		document.AddMember("money", money,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void DoPlayNow::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void DoPlayNow::toData(rapidjson::Document & document)
	{
		if(document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("money"))
		{
			setMoney(document["money"].GetUint());
		}

	}
}
