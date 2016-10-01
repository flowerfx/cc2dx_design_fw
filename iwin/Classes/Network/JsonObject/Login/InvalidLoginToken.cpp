#include "InvalidLoginToken.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	InvalidLoginToken:: InvalidLoginToken ()
	{

	}

	InvalidLoginToken::~InvalidLoginToken()
	{

	}

	rapidjson::Document InvalidLoginToken::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtype(type.c_str(), type.size());
		document.AddMember("type", vtype,locator);
rapidjson::Value vreason(reason.c_str(), reason.size());
		document.AddMember("reason", vreason,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void InvalidLoginToken::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void InvalidLoginToken::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("reason"))
		{
			setReason(document["reason"].GetString());
		}


	}
}
