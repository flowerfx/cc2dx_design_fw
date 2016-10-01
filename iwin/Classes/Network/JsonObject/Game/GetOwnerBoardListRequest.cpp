#include "GetOwnerBoardListRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetOwnerBoardListRequest:: GetOwnerBoardListRequest ()
	{

	}

	GetOwnerBoardListRequest::~GetOwnerBoardListRequest()
	{

	}

	rapidjson::Document GetOwnerBoardListRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("win", win,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetOwnerBoardListRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetOwnerBoardListRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt());
		}


	}
}
