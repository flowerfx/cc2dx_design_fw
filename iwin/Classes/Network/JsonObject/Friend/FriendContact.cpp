#include "FriendContact.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FriendContact:: FriendContact ()
	{

	}

	FriendContact::~FriendContact()
	{

	}

	rapidjson::Document FriendContact::toJson()
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
rapidjson::Value vphoneNumber(phoneNumber.c_str(), (size_t)phoneNumber.size());
		document.AddMember("phoneNumber", vphoneNumber,locator);
rapidjson::Value vname(name.c_str(), (size_t)name.size());
		document.AddMember("name", vname,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FriendContact::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FriendContact::toData(rapidjson::Document & document)
	{
				if(document.HasMember("phoneNumber"))
		{
			setPhoneNumber(document["phoneNumber"].GetString());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
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
