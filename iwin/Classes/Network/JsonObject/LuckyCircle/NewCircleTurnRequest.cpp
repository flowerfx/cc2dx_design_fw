#include "NewCircleTurnRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NewCircleTurnRequest:: NewCircleTurnRequest ()
	{

	}

	NewCircleTurnRequest::~NewCircleTurnRequest()
	{

	}

	rapidjson::Document NewCircleTurnRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("circleId", circleId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NewCircleTurnRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NewCircleTurnRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("circleId"))
		{
			setCircleId(document["circleId"].GetInt());
		}


	}
}
