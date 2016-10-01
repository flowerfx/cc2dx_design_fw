#include "InviteUser.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	InviteUser:: InviteUser ()
	{

	}

	InviteUser::~InviteUser()
	{

	}

	rapidjson::Document InviteUser::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("avatar", avatar,locator);
		document.AddMember("money", money,locator);
		document.AddMember("selected", selected,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void InviteUser::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void InviteUser::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("avatar"))
		{
			setAvatar(document["avatar"].GetInt());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}
		if(document.HasMember("selected"))
		{
			setSelected(document["selected"].GetBool());
		}


	}
}
