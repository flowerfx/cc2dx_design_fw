#include "LuckyCircleResultListItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LuckyCircleResultListItem:: LuckyCircleResultListItem ()
	{

	}

	LuckyCircleResultListItem::~LuckyCircleResultListItem()
	{

	}

	rapidjson::Document LuckyCircleResultListItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("itemId", itemId,locator);
rapidjson::Value vgiftName(giftName.c_str(), giftName.size());
		document.AddMember("giftName", vgiftName,locator);
		document.AddMember("numberOfItem", numberOfItem,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LuckyCircleResultListItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LuckyCircleResultListItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("itemId"))
		{
			setItemId(document["itemId"].GetInt());
		}
		if(document.HasMember("giftName"))
		{
			setGiftName(document["giftName"].GetString());
		}
		if(document.HasMember("numberOfItem"))
		{
			setNumberOfItem(document["numberOfItem"].GetInt());
		}


	}
}
