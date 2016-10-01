#include "EventType.h"
#include "json/writer.h"
namespace iwinmesage
{
	int EventType::NEWYEAR_ENVELOPE = 1;
int EventType::DOMINO_RANKING = 2;
int EventType::DOMINO_RECEIVE_CARD = 3;
int EventType::DANG_TRONG_DEN_HUNG = 4;

	EventType:: EventType ()
	{

	}

	EventType::~EventType()
	{

	}

	rapidjson::Document EventType::toJson()
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
	void EventType::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void EventType::toData(rapidjson::Document & document)
	{
		

	}
}
