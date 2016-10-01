#include "EventItemId.h"
#include "json/writer.h"
namespace iwinmesage
{
	int EventItemId::PHUC = 1;
int EventItemId::LOC = 2;
int EventItemId::THO = 3;
int EventItemId::TRONG_SAT = 4;
int EventItemId::TRONG_DONG = 5;
int EventItemId::TRONG_BAC = 6;
int EventItemId::TRONG_VANG = 7;
int EventItemId::BUA = 8;

	EventItemId:: EventItemId ()
	{

	}

	EventItemId::~EventItemId()
	{

	}

	rapidjson::Document EventItemId::toJson()
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
	void EventItemId::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventItemId::toData(rapidjson::Document & document)
	{
		

	}
}
