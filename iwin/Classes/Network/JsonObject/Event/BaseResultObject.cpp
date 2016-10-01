#include "BaseResultObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	BaseResultObject:: BaseResultObject ()
	{

	}

	BaseResultObject::~BaseResultObject()
	{

	}

	rapidjson::Document BaseResultObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
		document.AddMember("isSuccess", isSuccess,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void BaseResultObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void BaseResultObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("isSuccess"))
		{
			setIsSuccess(document["isSuccess"].GetBool());
		}


	}
}
