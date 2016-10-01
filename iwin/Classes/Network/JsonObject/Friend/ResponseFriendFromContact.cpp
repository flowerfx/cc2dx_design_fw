#include "ResponseFriendFromContact.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseFriendFromContact:: ResponseFriendFromContact ()
	{

	}

	ResponseFriendFromContact::~ResponseFriendFromContact()
	{

	}

	rapidjson::Document ResponseFriendFromContact::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrfriendContacts(rapidjson::kArrayType);
		for (size_t i = 0; i < friendContacts.size(); i++)
		{
			arrfriendContacts.PushBack(friendContacts.at(i).toJson(), locator);
		}
		document.AddMember("friendContacts",arrfriendContacts, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseFriendFromContact::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseFriendFromContact::toData(rapidjson::Document & document)
	{
				if(document.HasMember("friendContacts") && !document["friendContacts"].IsNull() && document["friendContacts"].IsArray() )
		{
			const rapidjson::Value& arrfriendContacts = document["friendContacts"];
			for (rapidjson::SizeType i = 0; i < arrfriendContacts.Size(); i++)
			{
							const rapidjson::Value& c = arrfriendContacts[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			FriendContact d;
			d.toData(json);
			friendContacts.push_back(d);

			}
		}


	}
}
