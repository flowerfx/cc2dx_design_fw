#include "GetStockDividend.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetStockDividend:: GetStockDividend ()
	{

	}

	GetStockDividend::~GetStockDividend()
	{

	}

	rapidjson::Document GetStockDividend::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("myStockID", myStockID,locator);
		document.AddMember("countryID", countryID,locator);
		document.AddMember("dividend", dividend,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetStockDividend::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetStockDividend::toData(rapidjson::Document & document)
	{
				if(document.HasMember("myStockID"))
		{
			setMyStockID(document["myStockID"].GetInt());
		}
		if(document.HasMember("countryID"))
		{
			setCountryID(document["countryID"].GetInt());
		}
		if(document.HasMember("dividend"))
		{
			setDividend(document["dividend"].GetInt64());
		}


	}
}
