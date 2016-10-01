#include "UpgrageItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	UpgrageItem:: UpgrageItem ()
	{

	}

	UpgrageItem::~UpgrageItem()
	{

	}

	rapidjson::Document UpgrageItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < list.size(); i++)
		{
			arrlist.PushBack(list.at(i).toJson(), locator);
		}
		document.AddMember("list",arrlist, locator);
		document.AddMember("resultItem", resultItem.toJson(),locator);
		document.AddMember("eventType", eventType,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void UpgrageItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void UpgrageItem::toData(rapidjson::Document & document)
	{
					if(document.HasMember("resultItem"))
			{
				const rapidjson::Value& cresultItem = document["resultItem"];
				rapidjson::StringBuffer bufferresultItem;
				bufferresultItem.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerresultItem(bufferresultItem);
				cresultItem.Accept(writerresultItem);
				const char* jsonresultItem = bufferresultItem.GetString();
				EventItem dresultItem;
				dresultItem.toData(jsonresultItem);
				setResultItem(dresultItem);
			}
		if(document.HasMember("eventType"))
		{
			setEventType(document["eventType"].GetInt());
		}
		if(document.HasMember("list") && !document["list"].IsNull() && document["list"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["list"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
							const rapidjson::Value& c = arrlist[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			EventItem d;
			d.toData(json);
			list.push_back(d);

			}
		}


	}
}
