#include "RubyToVNDInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RubyToVNDInfo:: RubyToVNDInfo ()
	{

	}

	RubyToVNDInfo::~RubyToVNDInfo()
	{

	}

	rapidjson::Document RubyToVNDInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrrates(rapidjson::kArrayType);
		for (size_t i = 0; i < rates.size(); i++)
		{
			arrrates.PushBack(rates.at(i).toJson(), locator);
		}
		document.AddMember("rates",arrrates, locator);
		document.AddMember("minRubyLeft", minRubyLeft,locator);
		document.AddMember("maxCardChange", maxCardChange,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RubyToVNDInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RubyToVNDInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("minRubyLeft"))
		{
			setMinRubyLeft(document["minRubyLeft"].GetInt());
		}
		if(document.HasMember("maxCardChange"))
		{
			setMaxCardChange(document["maxCardChange"].GetInt());
		}
		if(document.HasMember("rates") && !document["rates"].IsNull() && document["rates"].IsArray() )
		{
			const rapidjson::Value& arrrates = document["rates"];
			for (rapidjson::SizeType i = 0; i < arrrates.Size(); i++)
			{
							const rapidjson::Value& c = arrrates[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			ChargeRate d;
			d.toData(json);
			rates.push_back(d);

			}
		}


	}
}
