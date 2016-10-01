#include "EventInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	EventInfo:: EventInfo ()
	{

	}

	EventInfo::~EventInfo()
	{

	}

	rapidjson::Document EventInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlistItems(rapidjson::kArrayType);
		for (size_t i = 0; i < listItems.size(); i++)
		{
			arrlistItems.PushBack(listItems.at(i).toJson(), locator);
		}
		document.AddMember("listItems",arrlistItems, locator);
		rapidjson::Value arrlistAward(rapidjson::kArrayType);
		for (size_t i = 0; i < listAward.size(); i++)
		{
			arrlistAward.PushBack(listAward.at(i).toJson(), locator);
		}
		document.AddMember("listAward",arrlistAward, locator);
		rapidjson::Value arrlistUpgrageSuggestion(rapidjson::kArrayType);
		for (size_t i = 0; i < listUpgrageSuggestion.size(); i++)
		{
			arrlistUpgrageSuggestion.PushBack(listUpgrageSuggestion.at(i).toJson(), locator);
		}
		document.AddMember("listUpgrageSuggestion",arrlistUpgrageSuggestion, locator);
		document.AddMember("eventType", eventType,locator);
rapidjson::Value vlastResult(lastResult.c_str(), lastResult.size());
		document.AddMember("lastResult", vlastResult,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void EventInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("eventType"))
		{
			setEventType(document["eventType"].GetInt());
		}
		if(document.HasMember("lastResult"))
		{
			setLastResult(document["lastResult"].GetString());
		}
		if(document.HasMember("listItems") && !document["listItems"].IsNull() && document["listItems"].IsArray() )
		{
			const rapidjson::Value& arrlistItems = document["listItems"];
			for (rapidjson::SizeType i = 0; i < arrlistItems.Size(); i++)
			{
							const rapidjson::Value& c = arrlistItems[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			EventItem d;
			d.toData(json);
			listItems.push_back(d);

			}
		}
		if(document.HasMember("listAward") && !document["listAward"].IsNull() && document["listAward"].IsArray() )
		{
			const rapidjson::Value& arrlistAward = document["listAward"];
			for (rapidjson::SizeType i = 0; i < arrlistAward.Size(); i++)
			{
							const rapidjson::Value& c = arrlistAward[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			EventAwardList d;
			d.toData(json);
			listAward.push_back(d);

			}
		}
		if(document.HasMember("listUpgrageSuggestion") && !document["listUpgrageSuggestion"].IsNull() && document["listUpgrageSuggestion"].IsArray() )
		{
			const rapidjson::Value& arrlistUpgrageSuggestion = document["listUpgrageSuggestion"];
			for (rapidjson::SizeType i = 0; i < arrlistUpgrageSuggestion.Size(); i++)
			{
							const rapidjson::Value& c = arrlistUpgrageSuggestion[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			UpgrageItem d;
			d.toData(json);
			listUpgrageSuggestion.push_back(d);

			}
		}


	}
}
