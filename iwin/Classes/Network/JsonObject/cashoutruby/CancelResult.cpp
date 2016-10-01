#include "CancelResult.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CancelResult:: CancelResult ()
	{

	}

	CancelResult::~CancelResult()
	{

	}

	rapidjson::Document CancelResult::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("code", code,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CancelResult::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CancelResult::toData(rapidjson::Document & document)
	{
				if(document.HasMember("code"))
		{
			setCode(document["code"].GetInt());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
