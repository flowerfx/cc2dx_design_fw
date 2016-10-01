#include "LoginWithTokenSuccess.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LoginWithTokenSuccess:: LoginWithTokenSuccess ()
	{

	}

	LoginWithTokenSuccess::~LoginWithTokenSuccess()
	{

	}

	rapidjson::Document LoginWithTokenSuccess::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtype(type.c_str(), type.size());
		document.AddMember("type", vtype,locator);
		document.AddMember("id", id,locator);
rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LoginWithTokenSuccess::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LoginWithTokenSuccess::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}


	}
}
