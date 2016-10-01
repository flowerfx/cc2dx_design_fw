#include "PaymentInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	PaymentInfo:: PaymentInfo ()
	{

	}

	PaymentInfo::~PaymentInfo()
	{

	}

	rapidjson::Document PaymentInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("cardId", cardId,locator);
		document.AddMember("vnd", vnd,locator);
		document.AddMember("amount", amount,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PaymentInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PaymentInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("cardId"))
		{
			setCardId(document["cardId"].GetInt());
		}
		if(document.HasMember("vnd"))
		{
			setVnd(document["vnd"].GetInt());
		}
		if(document.HasMember("amount"))
		{
			setAmount(document["amount"].GetInt());
		}


	}
}
