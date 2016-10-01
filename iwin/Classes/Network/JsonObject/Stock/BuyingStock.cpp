#include "BuyingStock.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	BuyingStock:: BuyingStock ()
	{

	}

	BuyingStock::~BuyingStock()
	{

	}

	rapidjson::Document BuyingStock::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("numberOfStock", numberOfStock,locator);
		document.AddMember("countryID", countryID,locator);
		document.AddMember("isSucceed", isSucceed,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void BuyingStock::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void BuyingStock::toData(rapidjson::Document & document)
	{
				if(document.HasMember("numberOfStock"))
		{
			setNumberOfStock(document["numberOfStock"].GetInt());
		}
		if(document.HasMember("countryID"))
		{
			setCountryID(document["countryID"].GetInt());
		}
		if(document.HasMember("isSucceed"))
		{
			setIsSucceed(document["isSucceed"].GetBool());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
