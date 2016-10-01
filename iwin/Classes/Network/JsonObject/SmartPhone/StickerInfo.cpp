#include "StickerInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	StickerInfo:: StickerInfo ()
	{

	}

	StickerInfo::~StickerInfo()
	{

	}

	rapidjson::Document StickerInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vversion(version.c_str(), (unsigned int)version.size());
		document.AddMember("version", vversion,locator);
		document.AddMember("count", count,locator);
rapidjson::Value vlink(link.c_str(), (unsigned int)link.size());
		document.AddMember("link", vlink,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void StickerInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void StickerInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("version"))
		{
			setVersion(document["version"].GetString());
		}
		if(document.HasMember("count"))
		{
			setCount(document["count"].GetInt());
		}
		if(document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}


	}
}
