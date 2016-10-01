#include "Guest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Guest:: Guest ()
	{

	}

	Guest::~Guest()
	{

	}

	rapidjson::Document Guest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
		document.AddMember("avatar", avatar,locator);
		document.AddMember("userid", userid,locator);
		document.AddMember("money", money,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Guest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Guest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("avatar"))
		{
			setAvatar(document["avatar"].GetInt());
		}
		if(document.HasMember("userid"))
		{
			setUserid(document["userid"].GetInt());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}


	}
}
