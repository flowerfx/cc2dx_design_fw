#include "PromotionGetWin.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	PromotionGetWin:: PromotionGetWin ()
	{

	}

	PromotionGetWin::~PromotionGetWin()
	{

	}

	rapidjson::Document PromotionGetWin::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("isSuccess", isSuccess,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PromotionGetWin::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PromotionGetWin::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("isSuccess"))
		{
			setIsSuccess(document["isSuccess"].GetBool());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
