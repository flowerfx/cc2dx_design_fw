#include "NewLuckyGetTiketsObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NewLuckyGetTiketsObject:: NewLuckyGetTiketsObject ()
	{

	}

	NewLuckyGetTiketsObject::~NewLuckyGetTiketsObject()
	{

	}

	rapidjson::Document NewLuckyGetTiketsObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("screenId", screenId,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NewLuckyGetTiketsObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NewLuckyGetTiketsObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("screenId"))
		{
			setScreenId(document["screenId"].GetInt());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
