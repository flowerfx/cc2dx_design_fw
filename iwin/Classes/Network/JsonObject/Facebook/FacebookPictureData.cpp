#include "FacebookPictureData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookPictureData:: FacebookPictureData ()
	{

	}

	FacebookPictureData::~FacebookPictureData()
	{

	}

	rapidjson::Document FacebookPictureData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("is_silhouette", is_silhouette,locator);
rapidjson::Value vurl(url.c_str(), url.size());
		document.AddMember("url", vurl,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookPictureData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookPictureData::toData(rapidjson::Document & document)
	{
				if(document.HasMember("is_silhouette"))
		{
			setIs_silhouette(document["is_silhouette"].GetBool());
		}
		if(document.HasMember("url"))
		{
			setUrl(document["url"].GetString());
		}


	}
}
