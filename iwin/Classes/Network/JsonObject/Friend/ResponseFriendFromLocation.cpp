#include "ResponseFriendFromLocation.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseFriendFromLocation:: ResponseFriendFromLocation ()
	{

	}

	ResponseFriendFromLocation::~ResponseFriendFromLocation()
	{

	}

	rapidjson::Document ResponseFriendFromLocation::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrfriends(rapidjson::kArrayType);
		for (size_t i = 0; i < friends.size(); i++)
		{
			arrfriends.PushBack(friends.at(i).toJson(), locator);
		}
		document.AddMember("friends",arrfriends, locator);
		document.AddMember("totalPage", totalPage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseFriendFromLocation::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseFriendFromLocation::toData(rapidjson::Document & document)
	{
				if(document.HasMember("totalPage"))
		{
			setTotalPage(document["totalPage"].GetInt());
		}
		if(document.HasMember("friends") && !document["friends"].IsNull() && document["friends"].IsArray() )
		{
			const rapidjson::Value& arrfriends = document["friends"];
			for (rapidjson::SizeType i = 0; i < arrfriends.Size(); i++)
			{
							const rapidjson::Value& c = arrfriends[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Friend d;
			d.toData(json);
			friends.push_back(d);

			}
		}


	}
}
