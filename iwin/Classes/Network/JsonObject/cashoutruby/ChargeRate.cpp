#include "ChargeRate.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ChargeRate:: ChargeRate ()
	{

	}

	ChargeRate::~ChargeRate()
	{

	}

	rapidjson::Document ChargeRate::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrcards(rapidjson::kArrayType);
		for (size_t i = 0; i < cards.size(); i++)
		{
			arrcards.PushBack(cards.at(i).toJson(), locator);
		}
		document.AddMember("cards",arrcards, locator);
		document.AddMember("telcoId", telcoId,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ChargeRate::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ChargeRate::toData(rapidjson::Document & document)
	{
				if(document.HasMember("telcoId"))
		{
			setTelcoId(document["telcoId"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("cards") && !document["cards"].IsNull() && document["cards"].IsArray() )
		{
			const rapidjson::Value& arrcards = document["cards"];
			for (rapidjson::SizeType i = 0; i < arrcards.Size(); i++)
			{
							const rapidjson::Value& c = arrcards[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Card d;
			d.toData(json);
			cards.push_back(d);

			}
		}


	}
}
