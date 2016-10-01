#include "MyStockList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MyStockList:: MyStockList ()
	{

	}

	MyStockList::~MyStockList()
	{

	}

	rapidjson::Document MyStockList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrstockList(rapidjson::kArrayType);
		for (size_t i = 0; i < stockList.size(); i++)
		{
			arrstockList.PushBack(stockList.at(i).toJson(), locator);
		}
		document.AddMember("stockList",arrstockList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MyStockList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MyStockList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("stockList") && !document["stockList"].IsNull() && document["stockList"].IsArray() )
		{
			const rapidjson::Value& arrstockList = document["stockList"];
			for (rapidjson::SizeType i = 0; i < arrstockList.Size(); i++)
			{
							const rapidjson::Value& c = arrstockList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			MyStock d;
			d.toData(json);
			stockList.push_back(d);

			}
		}


	}
}
