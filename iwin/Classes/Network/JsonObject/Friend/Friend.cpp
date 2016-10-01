#include "Friend.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Friend:: Friend ()
	{

	}

	Friend::~Friend()
	{

	}

	rapidjson::Document Friend::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
rapidjson::Value vfacebookId(facebookId.c_str(), facebookId.size());
		document.AddMember("facebookId", vfacebookId,locator);
rapidjson::Value vfacebookName(facebookName.c_str(), facebookName.size());
		document.AddMember("facebookName", vfacebookName,locator);
		document.AddMember("money", money,locator);
		document.AddMember("avatarId", avatarId,locator);
		document.AddMember("location", location.toJson(),locator);
		document.AddMember("userId", userId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Friend::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Friend::toData(rapidjson::Document & document)
	{
				if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("facebookId"))
		{
			setFacebookId(document["facebookId"].GetString());
		}
		if(document.HasMember("facebookName"))
		{
			setFacebookName(document["facebookName"].GetString());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}
		if(document.HasMember("avatarId"))
		{
			setAvatarId(document["avatarId"].GetInt());
		}
			if(document.HasMember("location"))
			{
				const rapidjson::Value& clocation = document["location"];
				rapidjson::StringBuffer bufferlocation;
				bufferlocation.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerlocation(bufferlocation);
				clocation.Accept(writerlocation);
				const char* jsonlocation = bufferlocation.GetString();
				Location dlocation;
				dlocation.toData(jsonlocation);
				setLocation(dlocation);
			}
		if(document.HasMember("userId"))
		{
			setUserId(document["userId"].GetInt());
		}


	}
}
