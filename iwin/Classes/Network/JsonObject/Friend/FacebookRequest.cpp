#include "FacebookRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	int FacebookRequest::TYPE_REQUESTED_GIFT_FROM_FRIEND = 1;
int FacebookRequest::TYPE_RECEIVED_GIFT_FROM_FRIEND = 2;
int FacebookRequest::TYPE_REQUESTED_GIFT_FROM_FRIEND_APPROVED = 3;
int FacebookRequest::TYPE_RECEIVED_GIFT_FROM_FRIEND_APPROVED = 4;
int FacebookRequest::TYPE_OPEN_SUCCESS_GIFT = 5;

	FacebookRequest:: FacebookRequest ()
	{

	}

	FacebookRequest::~FacebookRequest()
	{

	}

	rapidjson::Document FacebookRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vrequestId(requestId.c_str(), requestId.size());
		document.AddMember("requestId", vrequestId,locator);
rapidjson::Value vfromFbId(fromFbId.c_str(), fromFbId.size());
		document.AddMember("fromFbId", vfromFbId,locator);
		document.AddMember("type", type,locator);
		document.AddMember("date", (int64_t) date,locator);
		document.AddMember("value", value,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt64());
		}
		if(document.HasMember("requestId"))
		{
			setRequestId(document["requestId"].GetString());
		}
		if(document.HasMember("fromFbId"))
		{
			setFromFbId(document["fromFbId"].GetString());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
			if(document.HasMember("date"))
			{
				/*const rapidjson::Value& cdate = document["date"];
				rapidjson::StringBuffer bufferdate;
				bufferdate.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerdate(bufferdate);
				cdate.Accept(writerdate);
				const char* jsondate = bufferdate.GetString();
				Date ddate;
				ddate.toData(jsondate);
				setDate(ddate);*/
			}
		if(document.HasMember("value"))
		{
			setValue(document["value"].GetInt());
		}


	}
}
