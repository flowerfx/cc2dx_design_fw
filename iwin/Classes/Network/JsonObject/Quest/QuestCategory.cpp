#include "QuestCategory.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	QuestCategory:: QuestCategory ()
	{
	}

	QuestCategory::~QuestCategory()
	{
	}

	rapidjson::Document QuestCategory::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
rapidjson::Value vimgUrl(imgUrl.c_str(), imgUrl.size());
		document.AddMember("imgUrl", vimgUrl,locator);
		document.AddMember("itemNo", itemNo,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void QuestCategory::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void QuestCategory::toData(rapidjson::Document & document)
	{
		if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("imgUrl"))
		{
			setImgUrl(document["imgUrl"].GetString());
		}
		if(document.HasMember("itemNo"))
		{
			setItemNo(document["itemNo"].GetInt());
		}

	}
}
