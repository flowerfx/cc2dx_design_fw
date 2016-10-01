#include "ButtonFeature.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ButtonFeature:: ButtonFeature ()
	{

	}

	ButtonFeature::~ButtonFeature()
	{

	}

	rapidjson::Document ButtonFeature::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtext(text.c_str(), text.size());
		document.AddMember("text", vtext,locator);
		document.AddMember("featureId", featureId,locator);
		document.AddMember("gameId", gameId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ButtonFeature::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ButtonFeature::toData(rapidjson::Document & document)
	{
				if(document.HasMember("text"))
		{
			setText(document["text"].GetString());
		}
		if(document.HasMember("featureId"))
		{
			setFeatureId(document["featureId"].GetInt());
		}
		if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}


	}
}
