#include "FacebookMyInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookMyInfo:: FacebookMyInfo ()
	{

	}

	FacebookMyInfo::~FacebookMyInfo()
	{

	}

	rapidjson::Document FacebookMyInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid,locator);
rapidjson::Value vemail(email.c_str(), email.size());
		document.AddMember("email", vemail,locator);
rapidjson::Value vfirst_name(first_name.c_str(), first_name.size());
		document.AddMember("first_name", vfirst_name,locator);
rapidjson::Value vgender(gender.c_str(), gender.size());
		document.AddMember("gender", vgender,locator);
rapidjson::Value vlast_name(last_name.c_str(), last_name.size());
		document.AddMember("last_name", vlast_name,locator);
rapidjson::Value vlink(link.c_str(), link.size());
		document.AddMember("link", vlink,locator);
rapidjson::Value vlocale(locale.c_str(), locale.size());
		document.AddMember("locale", vlocale,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
rapidjson::Value vtimezone(timezone.c_str(), timezone.size());
		document.AddMember("timezone", vtimezone,locator);
rapidjson::Value vupdated_time(updated_time.c_str(), updated_time.size());
		document.AddMember("updated_time", vupdated_time,locator);
		document.AddMember("verified", verified,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookMyInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookMyInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if(document.HasMember("email"))
		{
			setEmail(document["email"].GetString());
		}
		if(document.HasMember("first_name"))
		{
			setFirst_name(document["first_name"].GetString());
		}
		if(document.HasMember("gender"))
		{
			setGender(document["gender"].GetString());
		}
		if(document.HasMember("last_name"))
		{
			setLast_name(document["last_name"].GetString());
		}
		if(document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}
		if(document.HasMember("locale"))
		{
			setLocale(document["locale"].GetString());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("timezone"))
		{
			setTimezone(document["timezone"].GetString());
		}
		if(document.HasMember("updated_time"))
		{
			setUpdated_time(document["updated_time"].GetString());
		}
		if(document.HasMember("verified"))
		{
			setVerified(document["verified"].GetBool());
		}


	}
}
