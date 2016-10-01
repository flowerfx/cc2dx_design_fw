#include "MustRegisterWithToken.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MustRegisterWithToken:: MustRegisterWithToken ()
	{

	}

	MustRegisterWithToken::~MustRegisterWithToken()
	{

	}

	rapidjson::Document MustRegisterWithToken::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtype(type.c_str(), type.size());
		document.AddMember("type", vtype,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MustRegisterWithToken::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MustRegisterWithToken::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
