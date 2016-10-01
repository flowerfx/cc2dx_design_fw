#include "FacebookFriendListPage.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookFriendListPage:: FacebookFriendListPage ()
	{

	}

	FacebookFriendListPage::~FacebookFriendListPage()
	{

	}

	rapidjson::Document FacebookFriendListPage::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vnext(next.c_str(), next.size());
		document.AddMember("next", vnext,locator);
		document.AddMember("cursors", cursors.toJson(),locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookFriendListPage::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookFriendListPage::toData(rapidjson::Document & document)
	{
				if(document.HasMember("next"))
		{
			setNext(document["next"].GetString());
		}
			if(document.HasMember("cursors"))
			{
				const rapidjson::Value& ccursors = document["cursors"];
				rapidjson::StringBuffer buffercursors;
				buffercursors.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writercursors(buffercursors);
				ccursors.Accept(writercursors);
				const char* jsoncursors = buffercursors.GetString();
				FacebookFriendListPageCursor dcursors;
				dcursors.toData(jsoncursors);
				setCursors(dcursors);
			}


	}
}
