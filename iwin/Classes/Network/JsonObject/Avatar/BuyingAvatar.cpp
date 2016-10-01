#include "BuyingAvatar.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	BuyingAvatar:: BuyingAvatar ()
	{

	}

	BuyingAvatar::~BuyingAvatar()
	{

	}

	rapidjson::Document BuyingAvatar::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("isSucceed", isSucceed,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
		document.AddMember("money", money,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void BuyingAvatar::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void BuyingAvatar::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("isSucceed"))
		{
			setIsSucceed(document["isSucceed"].GetBool());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}


	}
}
