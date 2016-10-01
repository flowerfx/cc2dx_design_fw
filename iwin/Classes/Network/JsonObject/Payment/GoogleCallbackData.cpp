#include "GoogleCallbackData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GoogleCallbackData:: GoogleCallbackData ()
	{

	}

	GoogleCallbackData::~GoogleCallbackData()
	{

	}

	rapidjson::Document GoogleCallbackData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("purchaseData", purchaseData.toJson(),locator);
rapidjson::Value vdataSignature(dataSignature.c_str(), dataSignature.size());
		document.AddMember("dataSignature", vdataSignature,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GoogleCallbackData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GoogleCallbackData::toData(rapidjson::Document & document)
	{
					if(document.HasMember("purchaseData"))
			{
				const rapidjson::Value& cpurchaseData = document["purchaseData"];
				rapidjson::StringBuffer bufferpurchaseData;
				bufferpurchaseData.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerpurchaseData(bufferpurchaseData);
				cpurchaseData.Accept(writerpurchaseData);
				const char* jsonpurchaseData = bufferpurchaseData.GetString();
				GoogleCallbackPurchaseData dpurchaseData;
				dpurchaseData.toData(jsonpurchaseData);
				setPurchaseData(dpurchaseData);
			}
		if(document.HasMember("dataSignature"))
		{
			setDataSignature(document["dataSignature"].GetString());
		}


	}
}
