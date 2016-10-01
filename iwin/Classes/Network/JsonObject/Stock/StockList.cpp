#include "StockList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	StockList:: StockList ()
	{

	}

	StockList::~StockList()
	{

	}

	rapidjson::Document StockList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrcountryList(rapidjson::kArrayType);
		for (size_t i = 0; i < countryList.size(); i++)
		{
			arrcountryList.PushBack(countryList.at(i).toJson(), locator);
		}
		document.AddMember("countryList",arrcountryList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void StockList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void StockList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("countryList") && !document["countryList"].IsNull() && document["countryList"].IsArray() )
		{
			const rapidjson::Value& arrcountryList = document["countryList"];
			for (rapidjson::SizeType i = 0; i < arrcountryList.Size(); i++)
			{
							const rapidjson::Value& c = arrcountryList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Stock d;
			d.toData(json);
			countryList.push_back(d);

			}
		}


	}
}
