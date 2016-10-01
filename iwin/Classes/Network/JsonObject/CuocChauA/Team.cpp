#include "Team.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Team:: Team ()
	{

	}

	Team::~Team()
	{

	}

	rapidjson::Document Team::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
rapidjson::Value vimgLink(imgLink.c_str(), imgLink.size());
		document.AddMember("imgLink", vimgLink,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Team::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Team::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("imgLink"))
		{
			setImgLink(document["imgLink"].GetString());
		}


	}
}
