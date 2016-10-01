#include "CancelGetCard.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CancelGetCard:: CancelGetCard ()
	{

	}

	CancelGetCard::~CancelGetCard()
	{

	}

	rapidjson::Document CancelGetCard::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("logId", logId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CancelGetCard::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CancelGetCard::toData(rapidjson::Document & document)
	{
				if(document.HasMember("logId"))
		{
			setLogId(document["logId"].GetInt());
		}


	}
}
