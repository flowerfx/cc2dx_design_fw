#include "GoogleCallbackPurchaseData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GoogleCallbackPurchaseData:: GoogleCallbackPurchaseData ()
	{

	}

	GoogleCallbackPurchaseData::~GoogleCallbackPurchaseData()
	{

	}

	rapidjson::Document GoogleCallbackPurchaseData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vorderId(orderId.c_str(), orderId.size());
		document.AddMember("orderId", vorderId,locator);
rapidjson::Value vpackageName(packageName.c_str(), packageName.size());
		document.AddMember("packageName", vpackageName,locator);
rapidjson::Value vproductId(productId.c_str(), productId.size());
		document.AddMember("productId", vproductId,locator);
		document.AddMember("purchaseTime", purchaseTime,locator);
		document.AddMember("purchaseState", purchaseState,locator);
rapidjson::Value vdeveloperPayload(developerPayload.c_str(), developerPayload.size());
		document.AddMember("developerPayload", vdeveloperPayload,locator);
rapidjson::Value vpurchaseToken(purchaseToken.c_str(), purchaseToken.size());
		document.AddMember("purchaseToken", vpurchaseToken,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GoogleCallbackPurchaseData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GoogleCallbackPurchaseData::toData(rapidjson::Document & document)
	{
				if(document.HasMember("orderId"))
		{
			setOrderId(document["orderId"].GetString());
		}
		if(document.HasMember("packageName"))
		{
			setPackageName(document["packageName"].GetString());
		}
		if(document.HasMember("productId"))
		{
			setProductId(document["productId"].GetString());
		}
		if(document.HasMember("purchaseTime"))
		{
			setPurchaseTime(document["purchaseTime"].GetInt64());
		}
		if(document.HasMember("purchaseState"))
		{
			setPurchaseState(document["purchaseState"].GetInt());
		}
		if(document.HasMember("developerPayload"))
		{
			setDeveloperPayload(document["developerPayload"].GetString());
		}
		if(document.HasMember("purchaseToken"))
		{
			setPurchaseToken(document["purchaseToken"].GetString());
		}


	}
}
