#include "UserRanking.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	UserRanking:: UserRanking ()
	{

	}

	UserRanking::~UserRanking()
	{

	}

	rapidjson::Document UserRanking::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("index", index,locator);
rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
		document.AddMember("point", point,locator);
		document.AddMember("won", won,locator);
		document.AddMember("lose", lose,locator);
		document.AddMember("draw", draw,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void UserRanking::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void UserRanking::toData(rapidjson::Document & document)
	{
				if(document.HasMember("index"))
		{
			setIndex(document["index"].GetInt());
		}
		if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("point"))
		{
			setPoint(document["point"].GetInt());
		}
		if(document.HasMember("won"))
		{
			setWon(document["won"].GetInt());
		}
		if(document.HasMember("lose"))
		{
			setLose(document["lose"].GetInt());
		}
		if(document.HasMember("draw"))
		{
			setDraw(document["draw"].GetInt());
		}


	}
}
