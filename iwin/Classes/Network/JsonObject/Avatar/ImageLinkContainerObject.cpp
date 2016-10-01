#include "ImageLinkContainerObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ImageLinkContainerObject:: ImageLinkContainerObject ()
	{

	}

	ImageLinkContainerObject::~ImageLinkContainerObject()
	{

	}

	rapidjson::Document ImageLinkContainerObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vavatar_new(avatar_new.c_str(), avatar_new.size());
		document.AddMember("avatar_new", vavatar_new,locator);
rapidjson::Value vitems_new(items_new.c_str(), items_new.size());
		document.AddMember("items_new", vitems_new,locator);
rapidjson::Value vavatar(avatar.c_str(), avatar.size());
		document.AddMember("avatar", vavatar,locator);
rapidjson::Value vitems(items.c_str(), items.size());
		document.AddMember("items", vitems,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ImageLinkContainerObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ImageLinkContainerObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("avatar_new"))
		{
			setAvatar_new(document["avatar_new"].GetString());
		}
		if(document.HasMember("items_new"))
		{
			setItems_new(document["items_new"].GetString());
		}
		if(document.HasMember("avatar"))
		{
			setAvatar(document["avatar"].GetString());
		}
		if(document.HasMember("items"))
		{
			setItems(document["items"].GetString());
		}


	}
}
