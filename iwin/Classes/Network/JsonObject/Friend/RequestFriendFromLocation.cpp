#include "RequestFriendFromLocation.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestFriendFromLocation:: RequestFriendFromLocation ()
	{

	}

	RequestFriendFromLocation::~RequestFriendFromLocation()
	{

	}

	rapidjson::Document RequestFriendFromLocation::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("location", location.toJson(),locator);
		document.AddMember("page", page,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestFriendFromLocation::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestFriendFromLocation::toData(rapidjson::Document & document)
	{
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
		if(document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}


	}
}
