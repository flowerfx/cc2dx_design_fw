#include "GetQuestList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetQuestList:: GetQuestList ()
	{

	}

	GetQuestList::~GetQuestList()
	{

	}

	rapidjson::Document GetQuestList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("contactStatus", contactStatus,locator);
		document.AddMember("notificationStatus", notificationStatus,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetQuestList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetQuestList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("contactStatus"))
		{
			setContactStatus(document["contactStatus"].GetBool());
		}
		if(document.HasMember("notificationStatus"))
		{
			setNotificationStatus(document["notificationStatus"].GetBool());
		}


	}
}
