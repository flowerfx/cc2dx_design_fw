#include "TicketPrice.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	TicketPrice:: TicketPrice ()
	{

	}

	TicketPrice::~TicketPrice()
	{

	}

	rapidjson::Document TicketPrice::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("price", price,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TicketPrice::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TicketPrice::toData(rapidjson::Document & document)
	{
				if(document.HasMember("price"))
		{
			setPrice(document["price"].GetInt64());
		}


	}
}
