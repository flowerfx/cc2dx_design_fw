#include "RequestFeature.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestFeature:: RequestFeature ()
	{

	}

	RequestFeature::~RequestFeature()
	{

	}

	rapidjson::Document RequestFeature::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlistButton(rapidjson::kArrayType);
		for (size_t i = 0; i < listButton.size(); i++)
		{
			arrlistButton.PushBack(listButton.at(i).toJson(), locator);
		}
		document.AddMember("listButton",arrlistButton, locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
rapidjson::Value vbuttonOk(buttonOk.c_str(), buttonOk.size());
		document.AddMember("buttonOk", vbuttonOk,locator);
rapidjson::Value vbuttonCancel(buttonCancel.c_str(), buttonCancel.size());
		document.AddMember("buttonCancel", vbuttonCancel,locator);
		document.AddMember("featureId", featureId,locator);
		document.AddMember("gameId", gameId,locator);
		document.AddMember("type", type,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestFeature::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestFeature::toData(rapidjson::Document & document)
	{
				if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("buttonOk"))
		{
			setButtonOk(document["buttonOk"].GetString());
		}
		if(document.HasMember("buttonCancel"))
		{
			setButtonCancel(document["buttonCancel"].GetString());
		}
		if(document.HasMember("featureId"))
		{
			setFeatureId(document["featureId"].GetInt());
		}
		if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if(document.HasMember("listButton") && !document["listButton"].IsNull() && document["listButton"].IsArray() )
		{
			const rapidjson::Value& arrlistButton = document["listButton"];
			for (rapidjson::SizeType i = 0; i < arrlistButton.Size(); i++)
			{
							const rapidjson::Value& c = arrlistButton[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			ButtonFeature d;
			d.toData(json);
			listButton.push_back(d);

			}
		}


	}
}
