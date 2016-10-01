#include "Stock.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Stock:: Stock ()
	{

	}

	Stock::~Stock()
	{

	}

	rapidjson::Document Stock::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("stockIndex", stockIndex,locator);
		document.AddMember("pricePerStock", pricePerStock,locator);
rapidjson::Value vimgLink(imgLink.c_str(), imgLink.size());
		document.AddMember("imgLink", vimgLink,locator);
		document.AddMember("difference", difference,locator);
		document.AddMember("canBuy", canBuy,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Stock::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Stock::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("stockIndex"))
		{
			setStockIndex(document["stockIndex"].GetDouble());
		}
		if(document.HasMember("pricePerStock"))
		{
			setPricePerStock(document["pricePerStock"].GetInt64());
		}
		if(document.HasMember("imgLink"))
		{
			setImgLink(document["imgLink"].GetString());
		}
		if(document.HasMember("difference"))
		{
			setDifference(document["difference"].GetInt());
		}
		if(document.HasMember("canBuy"))
		{
			setCanBuy(document["canBuy"].GetBool());
		}


	}
}
