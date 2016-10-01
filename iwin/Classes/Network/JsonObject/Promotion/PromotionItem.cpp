#include "PromotionItem.h"
#include "json/writer.h"
namespace iwinmesage
{

	PromotionItem::PromotionItem()
	{

	}

	PromotionItem::~PromotionItem()
	{

	}

	rapidjson::Document PromotionItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		document.AddMember("id", id, locator);
		rapidjson::Value vchargeType(chargeType.c_str(), chargeType.size());
		document.AddMember("chargeType", vchargeType, locator);
		rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname, locator);
		rapidjson::Value vdes(des.c_str(), des.size());
		document.AddMember("des", vdes, locator);
		rapidjson::Value vactionType(actionType.c_str(), actionType.size());
		document.AddMember("actionType", vactionType, locator);
		rapidjson::Value vimgUrl(imgUrl.c_str(), imgUrl.size());
		document.AddMember("imgUrl", vimgUrl, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PromotionItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PromotionItem::toData(rapidjson::Document & document)
	{
		if (document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if (document.HasMember("chargeType"))
		{
			setchargeType(document["chargeType"].GetString());
		}
		if (document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if (document.HasMember("des"))
		{
			setDes(document["des"].GetString());
		}
		if (document.HasMember("actionType"))
		{

			setactionType(document["actionType"].GetString());
		}
		if (document.HasMember("imgUrl"))
		{
			setImgUrl(document["imgUrl"].GetString());
		}


	}
}
