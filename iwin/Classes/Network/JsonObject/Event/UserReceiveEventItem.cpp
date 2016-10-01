#include "UserReceiveEventItem.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	UserReceiveEventItem:: UserReceiveEventItem ()
	{

	}

	UserReceiveEventItem::~UserReceiveEventItem()
	{

	}

	rapidjson::Document UserReceiveEventItem::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("userId", userId,locator);
rapidjson::Value vuserName(userName.c_str(), userName.size());
		document.AddMember("userName", vuserName,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
rapidjson::Value vimgUrl(imgUrl.c_str(), imgUrl.size());
		document.AddMember("imgUrl", vimgUrl,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void UserReceiveEventItem::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void UserReceiveEventItem::toData(rapidjson::Document & document)
	{
				if(document.HasMember("userId"))
		{
			setUserId(document["userId"].GetInt());
		}
		if(document.HasMember("userName"))
		{
			setUserName(document["userName"].GetString());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("imgUrl"))
		{
			setImgUrl(document["imgUrl"].GetString());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}


	}
}
