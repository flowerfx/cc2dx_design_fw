#include "PaymentHistoryList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	PaymentHistoryList:: PaymentHistoryList ()
	{

	}

	PaymentHistoryList::~PaymentHistoryList()
	{

	}

	rapidjson::Document PaymentHistoryList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrhistoryList(rapidjson::kArrayType);
		for (size_t i = 0; i < historyList.size(); i++)
		{
			arrhistoryList.PushBack(historyList.at(i).toJson(), locator);
		}
		document.AddMember("historyList",arrhistoryList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PaymentHistoryList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PaymentHistoryList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("historyList") && !document["historyList"].IsNull() && document["historyList"].IsArray() )
		{
			const rapidjson::Value& arrhistoryList = document["historyList"];
			for (rapidjson::SizeType i = 0; i < arrhistoryList.Size(); i++)
			{
							const rapidjson::Value& c = arrhistoryList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			PaymentHistory d;
			d.toData(json);
			historyList.push_back(d);

			}
		}


	}
}
