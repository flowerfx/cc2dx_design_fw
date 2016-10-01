#include "OptionItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	OptionItem:: OptionItem ()
	{

	}

	OptionItem::~OptionItem()
	{

	}

	rapidjson::Document OptionItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vtext(text.c_str(), text.size());
		document.AddMember("text", vtext,locator);
		document.AddMember("isSelected", isSelected,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void OptionItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void OptionItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("text"))
		{
			setText(document["text"].GetString());
		}
		if(document.HasMember("isSelected"))
		{
			setIsSelected(document["isSelected"].GetBool());
		}


	}
}
