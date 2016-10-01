#include "UserInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	UserInfo:: UserInfo ()
	{

	}

	UserInfo::~UserInfo()
	{

	}

	rapidjson::Document UserInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vfullname(fullname.c_str(), fullname.size());
		document.AddMember("fullname", vfullname,locator);
rapidjson::Value vphone(phone.c_str(), phone.size());
		document.AddMember("phone", vphone,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void UserInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void UserInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("fullname"))
		{
			setFullname(document["fullname"].GetString());
		}
		if(document.HasMember("phone"))
		{
			setPhone(document["phone"].GetString());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
