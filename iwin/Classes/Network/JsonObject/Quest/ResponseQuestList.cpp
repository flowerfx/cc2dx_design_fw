#include "ResponseQuestList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseQuestList:: ResponseQuestList ()
	{

	}

	ResponseQuestList::~ResponseQuestList()
	{

	}

	rapidjson::Document ResponseQuestList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrquests(rapidjson::kArrayType);
		for (size_t i = 0; i < quests.size(); i++)
		{
			arrquests.PushBack(quests.at(i).toJson(), locator);
		}
		document.AddMember("quests",arrquests, locator);
		document.AddMember("categoryId", categoryId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseQuestList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseQuestList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("categoryId"))
		{
			setCategoryId(document["categoryId"].GetInt());
		}
		if(document.HasMember("quests") && !document["quests"].IsNull() && document["quests"].IsArray() )
		{
			const rapidjson::Value& arrquests = document["quests"];
			for (rapidjson::SizeType i = 0; i < arrquests.Size(); i++)
			{
				const rapidjson::Value& c = arrquests[i];	
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();
				Quest * d = new Quest();
				d->toData(json);
				quests.push_back(*d);

			}
		}


	}
}
