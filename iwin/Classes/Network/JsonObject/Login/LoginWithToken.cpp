#include "LoginWithToken.h"
#include "json/writer.h"
namespace iwinmesage
{
	std::string LoginWithToken::TYPE_FACEBOOK = "facebook";
std::string LoginWithToken::TYPE_GOOGLE = "google";
std::string LoginWithToken::TYPE_MOBO = "mobo";
std::string LoginWithToken::TYPE_CHANGE_SERVER = "change_server";
std::string LoginWithToken::TYPE_APPLEID = "appleId";

	LoginWithToken:: LoginWithToken ()
	{

	}

	LoginWithToken::~LoginWithToken()
	{

	}

	rapidjson::Document LoginWithToken::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtype(type.c_str(), type.size());
		document.AddMember("type", vtype,locator);
rapidjson::Value vtoken(token.c_str(), token.size());
		document.AddMember("token", vtoken,locator);
rapidjson::Value vversion(version.c_str(), version.size());
		document.AddMember("version", vversion,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LoginWithToken::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LoginWithToken::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("token"))
		{
			setToken(document["token"].GetString());
		}
		if(document.HasMember("version"))
		{
			setVersion(document["version"].GetString());
		}


	}
}
