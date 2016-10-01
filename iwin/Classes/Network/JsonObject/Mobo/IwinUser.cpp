#include "IwinUser.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	IwinUser:: IwinUser ()
	{

	}

	IwinUser::~IwinUser()
	{

	}

	rapidjson::Document IwinUser::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
		document.AddMember("userid", userid,locator);
		document.AddMember("win", win,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void IwinUser::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void IwinUser::toData(rapidjson::Document & document)
	{
				if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("userid"))
		{
			setUserid(document["userid"].GetInt());
		}
		if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt64());
		}


	}
}
