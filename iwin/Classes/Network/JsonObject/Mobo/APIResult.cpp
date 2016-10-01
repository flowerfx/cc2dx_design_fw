#include "APIResult.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	APIResult:: APIResult ()
	{

	}

	APIResult::~APIResult()
	{

	}

	rapidjson::Document APIResult::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("code", code,locator);
rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void APIResult::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void APIResult::toData(rapidjson::Document & document)
	{
				if(document.HasMember("code"))
		{
			setCode(document["code"].GetInt());
		}
		if(document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
