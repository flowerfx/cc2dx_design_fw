#include "GooglePurchaseData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GooglePurchaseData:: GooglePurchaseData ()
	{

	}

	GooglePurchaseData::~GooglePurchaseData()
	{

	}

	rapidjson::Document GooglePurchaseData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vproductId(productId.c_str(), productId.size());
		document.AddMember("productId", vproductId,locator);
rapidjson::Value vdeveloperPayload(developerPayload.c_str(), developerPayload.size());
		document.AddMember("developerPayload", vdeveloperPayload,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GooglePurchaseData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GooglePurchaseData::toData(rapidjson::Document & document)
	{
				if(document.HasMember("productId"))
		{
			setProductId(document["productId"].GetString());
		}
		if(document.HasMember("developerPayload"))
		{
			setDeveloperPayload(document["developerPayload"].GetString());
		}


	}
}
