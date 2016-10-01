#include "EventAward.h"
#include "json/writer.h"
namespace iwinmesage
{
	int EventAward::WIN = 1;
int EventAward::LUCKY_CIRCLE_POINT = 2;
int EventAward::FIRE_CRACKER_POINT = 3;
int EventAward::GREETING = 4;
int EventAward::IPHONE6 = 5;
int EventAward::ZENPHONE5 = 6;

	EventAward:: EventAward ()
	{

	}

	EventAward::~EventAward()
	{

	}

	rapidjson::Document EventAward::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("type", type,locator);
rapidjson::Value vvalue(value.c_str(), value.size());
		document.AddMember("value", vvalue,locator);
rapidjson::Value vdes(des.c_str(), des.size());
		document.AddMember("des", vdes,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void EventAward::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventAward::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if(document.HasMember("value"))
		{
			setValue(document["value"].GetString());
		}
		if(document.HasMember("des"))
		{
			setDes(document["des"].GetString());
		}


	}
}
