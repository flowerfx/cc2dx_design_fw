#include "NewCircleInfoRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NewCircleInfoRequest:: NewCircleInfoRequest ()
	{

	}

	NewCircleInfoRequest::~NewCircleInfoRequest()
	{

	}

	rapidjson::Document NewCircleInfoRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("circleId", circleId,locator);
		document.AddMember("screenType", screenType,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NewCircleInfoRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NewCircleInfoRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("circleId"))
		{
			setCircleId(document["circleId"].GetInt());
		}
		if(document.HasMember("screenType"))
		{
			setScreenType(document["screenType"].GetInt());
		}


	}
}
