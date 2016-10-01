#include "ServerControlButtonObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ServerControlButtonObject:: ServerControlButtonObject ()
	{

	}

	ServerControlButtonObject::~ServerControlButtonObject()
	{

	}

	rapidjson::Document ServerControlButtonObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("payment", payment.toJson(),locator);
rapidjson::Value vimg(img.c_str(), img.size());
		document.AddMember("img", vimg,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ServerControlButtonObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ServerControlButtonObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
			if(document.HasMember("payment"))
			{
				const rapidjson::Value& cpayment = document["payment"];
				rapidjson::StringBuffer bufferpayment;
				bufferpayment.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerpayment(bufferpayment);
				cpayment.Accept(writerpayment);
				const char* jsonpayment = bufferpayment.GetString();
				Payment dpayment;
				dpayment.toData(jsonpayment);
				setPayment(dpayment);
			}
		if(document.HasMember("img"))
		{
			setImg(document["img"].GetString());
		}


	}
}
