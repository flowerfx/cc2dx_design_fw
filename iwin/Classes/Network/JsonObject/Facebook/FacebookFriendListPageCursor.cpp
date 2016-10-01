#include "FacebookFriendListPageCursor.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookFriendListPageCursor:: FacebookFriendListPageCursor ()
	{

	}

	FacebookFriendListPageCursor::~FacebookFriendListPageCursor()
	{

	}

	rapidjson::Document FacebookFriendListPageCursor::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vbefore(before.c_str(), before.size());
		document.AddMember("before", vbefore,locator);
rapidjson::Value vafter(after.c_str(), after.size());
		document.AddMember("after", vafter,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookFriendListPageCursor::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookFriendListPageCursor::toData(rapidjson::Document & document)
	{
				if(document.HasMember("before"))
		{
			setBefore(document["before"].GetString());
		}
		if(document.HasMember("after"))
		{
			setAfter(document["after"].GetString());
		}


	}
}
