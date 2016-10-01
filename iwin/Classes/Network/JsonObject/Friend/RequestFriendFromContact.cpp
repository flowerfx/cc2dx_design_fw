#include "RequestFriendFromContact.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestFriendFromContact:: RequestFriendFromContact ()
	{

	}

	RequestFriendFromContact::~RequestFriendFromContact()
	{

	}

	rapidjson::Document RequestFriendFromContact::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrcontacts(rapidjson::kArrayType);
		for (size_t i = 0; i < contacts.size(); i++)
		{
			arrcontacts.PushBack(contacts.at(i).toJson(), locator);
		}
		document.AddMember("contacts",arrcontacts, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestFriendFromContact::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestFriendFromContact::toData(rapidjson::Document & document)
	{
				if(document.HasMember("contacts") && !document["contacts"].IsNull() && document["contacts"].IsArray() )
		{
			const rapidjson::Value& arrcontacts = document["contacts"];
			for (rapidjson::SizeType i = 0; i < arrcontacts.Size(); i++)
			{
							const rapidjson::Value& c = arrcontacts[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Contact d;
			d.toData(json);
			contacts.push_back(d);

			}
		}


	}
}
