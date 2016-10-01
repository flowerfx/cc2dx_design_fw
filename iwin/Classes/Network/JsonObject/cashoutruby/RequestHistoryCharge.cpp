#include "RequestHistoryCharge.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestHistoryCharge:: RequestHistoryCharge ()
	{

	}

	RequestHistoryCharge::~RequestHistoryCharge()
	{

	}

	rapidjson::Document RequestHistoryCharge::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("pageIndex", pageIndex,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestHistoryCharge::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestHistoryCharge::toData(rapidjson::Document & document)
	{
				if(document.HasMember("pageIndex"))
		{
			setPageIndex(document["pageIndex"].GetInt());
		}


	}
}
