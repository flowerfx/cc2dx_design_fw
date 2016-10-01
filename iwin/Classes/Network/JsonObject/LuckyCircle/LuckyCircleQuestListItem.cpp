#include "LuckyCircleQuestListItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LuckyCircleQuestListItem:: LuckyCircleQuestListItem ()
	{

	}

	LuckyCircleQuestListItem::~LuckyCircleQuestListItem()
	{

	}

	rapidjson::Document LuckyCircleQuestListItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), (size_t)name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("order", order,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LuckyCircleQuestListItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LuckyCircleQuestListItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("order"))
		{
			setOrder(document["order"].GetInt());
		}


	}
}
