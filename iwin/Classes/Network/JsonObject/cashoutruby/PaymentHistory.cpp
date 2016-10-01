#include "PaymentHistory.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	PaymentHistory:: PaymentHistory ()
	{

	}

	PaymentHistory::~PaymentHistory()
	{

	}

	rapidjson::Document PaymentHistory::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("logId", logId,locator);
		document.AddMember("status", status,locator);
rapidjson::Value vtime(time.c_str(), time.size());
		document.AddMember("time", vtime,locator);
rapidjson::Value vitem(item.c_str(), item.size());
		document.AddMember("item", vitem,locator);
rapidjson::Value vstatusDes(statusDes.c_str(), statusDes.size());
		document.AddMember("statusDes", vstatusDes,locator);
rapidjson::Value vdetail(detail.c_str(), detail.size());
		document.AddMember("detail", vdetail,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PaymentHistory::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PaymentHistory::toData(rapidjson::Document & document)
	{
				if(document.HasMember("logId"))
		{
			setLogId(document["logId"].GetInt());
		}
		if(document.HasMember("status"))
		{
			setStatus(document["status"].GetInt());
		}
		if(document.HasMember("time"))
		{
			setTime(document["time"].GetString());
		}
		if(document.HasMember("item"))
		{
			setItem(document["item"].GetString());
		}
		if(document.HasMember("statusDes"))
		{
			setStatusDes(document["statusDes"].GetString());
		}
		if(document.HasMember("detail"))
		{
			setDetail(document["detail"].GetString());
		}


	}
}
