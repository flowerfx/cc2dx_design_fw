#include "FacebookFriend.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookFriend:: FacebookFriend ()
	{

	}

	FacebookFriend::~FacebookFriend()
	{

	}

	rapidjson::Document FacebookFriend::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("picture", picture.toJson(),locator);
		document.AddMember("selected", selected,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookFriend::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookFriend::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
			if(document.HasMember("picture"))
			{
				const rapidjson::Value& cpicture = document["picture"];
				rapidjson::StringBuffer bufferpicture;
				bufferpicture.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerpicture(bufferpicture);
				cpicture.Accept(writerpicture);
				const char* jsonpicture = bufferpicture.GetString();
				FacebookPicture dpicture;
				dpicture.toData(jsonpicture);
				setPicture(dpicture);
			}
		if(document.HasMember("selected"))
		{
			setSelected(document["selected"].GetBool());
		}


	}
}
