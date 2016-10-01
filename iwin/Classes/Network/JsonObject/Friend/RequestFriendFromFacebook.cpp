#include "RequestFriendFromFacebook.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestFriendFromFacebook:: RequestFriendFromFacebook ()
	{

	}

	RequestFriendFromFacebook::~RequestFriendFromFacebook()
	{

	}

	rapidjson::Document RequestFriendFromFacebook::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vaccessToken(accessToken.c_str(), accessToken.size());
		document.AddMember("accessToken", vaccessToken,locator);
		document.AddMember("page", page,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestFriendFromFacebook::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestFriendFromFacebook::toData(rapidjson::Document & document)
	{
				if(document.HasMember("accessToken"))
		{
			setAccessToken(document["accessToken"].GetString());
		}
		if(document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}


	}
}
