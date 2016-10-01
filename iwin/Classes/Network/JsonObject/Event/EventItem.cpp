#include "EventItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	EventItem:: EventItem ()
	{

	}

	EventItem::~EventItem()
	{

	}

	rapidjson::Document EventItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("eventType", eventType,locator);
		document.AddMember("id", id,locator);
		document.AddMember("award", award.toJson(),locator);
rapidjson::Value vdes(des.c_str(), des.size());
		document.AddMember("des", vdes,locator);
		document.AddMember("numOfItem", numOfItem,locator);
		document.AddMember("isUsedSuccess", isUsedSuccess,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void EventItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("eventType"))
		{
			setEventType(document["eventType"].GetInt());
		}
		if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
			if(document.HasMember("award"))
			{
				const rapidjson::Value& caward = document["award"];
				rapidjson::StringBuffer bufferaward;
				bufferaward.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writeraward(bufferaward);
				caward.Accept(writeraward);
				const char* jsonaward = bufferaward.GetString();
				EventAward daward;
				daward.toData(jsonaward);
				setAward(daward);
			}
		if(document.HasMember("des"))
		{
			setDes(document["des"].GetString());
		}
		if(document.HasMember("numOfItem"))
		{
			setNumOfItem(document["numOfItem"].GetInt());
		}
		if(document.HasMember("isUsedSuccess"))
		{
			setIsUsedSuccess(document["isUsedSuccess"].GetBool());
		}


	}
}
