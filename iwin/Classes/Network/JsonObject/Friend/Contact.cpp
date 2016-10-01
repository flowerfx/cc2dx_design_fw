#include "Contact.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Contact:: Contact ()
	{

	}

	Contact::~Contact()
	{

	}

	rapidjson::Document Contact::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrphones(rapidjson::kArrayType);
		for (size_t i = 0; i < phones.size(); i++)
		{
			arrphones.PushBack(rapidjson::Value().SetString(phones.at(i).c_str(),phones.at(i).size()), locator);
		}
		document.AddMember("phones",arrphones, locator);

		rapidjson::Value arremails(rapidjson::kArrayType);
		for (size_t i = 0; i < emails.size(); i++)
		{
			arremails.PushBack(rapidjson::Value().SetString(emails.at(i).c_str(),emails.at(i).size()), locator);
		}
		document.AddMember("emails",arremails, locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("contactId", contactId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Contact::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Contact::toData(rapidjson::Document & document)
	{
				if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("contactId"))
		{
			setContactId(document["contactId"].GetInt());
		}
		if(document.HasMember("phones") && !document["phones"].IsNull() && document["phones"].IsArray() )
		{
			const rapidjson::Value& arrphones = document["phones"];
			for (rapidjson::SizeType i = 0; i < arrphones.Size(); i++)
			{
				phones.push_back(arrphones[i].GetString());
			}
		}
		if(document.HasMember("emails") && !document["emails"].IsNull() && document["emails"].IsArray() )
		{
			const rapidjson::Value& arremails = document["emails"];
			for (rapidjson::SizeType i = 0; i < arremails.Size(); i++)
			{
				emails.push_back(arremails[i].GetString());
			}
		}


	}
}
