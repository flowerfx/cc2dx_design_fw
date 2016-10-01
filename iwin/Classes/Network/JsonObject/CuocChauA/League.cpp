#include "League.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	League:: League ()
	{

	}

	League::~League()
	{

	}

	rapidjson::Document League::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("leagueId", leagueId,locator);
rapidjson::Value vimgUrl(imgUrl.c_str(), imgUrl.size());
		document.AddMember("imgUrl", vimgUrl,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void League::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void League::toData(rapidjson::Document & document)
	{
				if(document.HasMember("leagueId"))
		{
			setLeagueId(document["leagueId"].GetInt());
		}
		if(document.HasMember("imgUrl"))
		{
			setImgUrl(document["imgUrl"].GetString());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}


	}
}
