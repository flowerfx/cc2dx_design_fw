#include "AvatarCat.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	AvatarCat:: AvatarCat ()
	{

	}

	AvatarCat::~AvatarCat()
	{

	}

	rapidjson::Document AvatarCat::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
rapidjson::Value vimg(img.c_str(), img.size());
		document.AddMember("img", vimg,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void AvatarCat::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void AvatarCat::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("img"))
		{
			setImg(document["img"].GetString());
		}


	}
}
