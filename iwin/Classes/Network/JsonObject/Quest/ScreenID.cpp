#include "ScreenID.h"
#include "json/writer.h"
namespace iwinmesage
{
	ScreenID:: ScreenID ()
	{

	}

	ScreenID::~ScreenID()
	{

	}

	rapidjson::Document ScreenID::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ScreenID::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ScreenID::toData(rapidjson::Document & document)
	{
		

	}
}
