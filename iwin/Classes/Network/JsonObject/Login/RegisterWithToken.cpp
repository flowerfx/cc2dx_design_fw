#include "RegisterWithToken.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RegisterWithToken:: RegisterWithToken ()
	{

	}

	RegisterWithToken::~RegisterWithToken()
	{

	}

	rapidjson::Document RegisterWithToken::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vtype(type.c_str(), type.size());
		document.AddMember("type", vtype,locator);
rapidjson::Value vtoken(token.c_str(), token.size());
		document.AddMember("token", vtoken,locator);
rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
rapidjson::Value vpass(pass.c_str(), pass.size());
		document.AddMember("pass", vpass,locator);
rapidjson::Value vphone(phone.c_str(), phone.size());
		document.AddMember("phone", vphone,locator);
rapidjson::Value vversion(version.c_str(), version.size());
		document.AddMember("version", vversion,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RegisterWithToken::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RegisterWithToken::toData(rapidjson::Document & document)
	{
				if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("token"))
		{
			setToken(document["token"].GetString());
		}
		if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("pass"))
		{
			setPass(document["pass"].GetString());
		}
		if(document.HasMember("phone"))
		{
			setPhone(document["phone"].GetString());
		}
		if(document.HasMember("version"))
		{
			setVersion(document["version"].GetString());
		}


	}
}
