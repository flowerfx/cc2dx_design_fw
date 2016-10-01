#include "EventAwardList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	EventAwardList:: EventAwardList ()
	{

	}

	EventAwardList::~EventAwardList()
	{

	}

	rapidjson::Document EventAwardList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlistAward(rapidjson::kArrayType);
		for (size_t i = 0; i < listAward.size(); i++)
		{
			arrlistAward.PushBack(listAward.at(i).toJson(), locator);
		}
		document.AddMember("listAward",arrlistAward, locator);
		document.AddMember("eventItemId", eventItemId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void EventAwardList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventAwardList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("eventItemId"))
		{
			setEventItemId(document["eventItemId"].GetInt());
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
			EventAward d;
			d.toData(json);
			listAward.push_back(d);

			}
		}


	}
}
