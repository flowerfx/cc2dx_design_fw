#include "TaiXiuUserOrder.h"
#include "TaiXiuUserOrderList.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUserOrderList::TaiXiuUserOrderList()
	{
		userOrderList.clear();
	}
	TaiXiuUserOrderList::~TaiXiuUserOrderList()
	{
		for (auto u : userOrderList)
		{
			delete u;
		}
		userOrderList.clear();
	}

	rapidjson::Document TaiXiuUserOrderList::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < userOrderList.size(); i++)
		{
			arrList.PushBack(userOrderList[i]->toJson(), locator);
		}
		document.AddMember("userOrderList", arrList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUserOrderList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUserOrderList::toData(rapidjson::Document & document)
	{
		if (document.HasMember("userOrderList") && !document["userOrderList"].IsNull() && document["userOrderList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["userOrderList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				const rapidjson::Value& c = arrlist[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();

				TaiXiuUserOrder * d = new TaiXiuUserOrder();
				d->toData(json);
				userOrderList.push_back(d);
			}
		}

	}
}