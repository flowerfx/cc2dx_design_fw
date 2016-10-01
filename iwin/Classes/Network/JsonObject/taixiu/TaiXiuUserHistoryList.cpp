#include "TaiXiuUserHistory.h"
#include "TaiXiuUserHistoryList.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUserHistoryList::TaiXiuUserHistoryList()
	{
		userHistoryList.clear();
	}

	TaiXiuUserHistoryList::~TaiXiuUserHistoryList()
	{
		for (auto u : userHistoryList)
		{
			delete u;
		}
		userHistoryList.clear();
	}


	rapidjson::Document TaiXiuUserHistoryList::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < userHistoryList.size(); i++)
		{
			arrList.PushBack(userHistoryList[i]->toJson(), locator);
		}
		document.AddMember("userHistoryList", arrList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUserHistoryList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUserHistoryList::toData(rapidjson::Document & document)
	{
		if (document.HasMember("userHistoryList") && !document["userHistoryList"].IsNull() && document["userHistoryList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["userHistoryList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				const rapidjson::Value& c = arrlist[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();

				TaiXiuUserHistory * d = new TaiXiuUserHistory();
				d->toData(json);
				userHistoryList.push_back(d);
			}
		}

	}
}

