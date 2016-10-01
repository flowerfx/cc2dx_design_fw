#include "CaptchaInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CaptchaInfo:: CaptchaInfo ()
	{

	}

	CaptchaInfo::~CaptchaInfo()
	{

	}

	rapidjson::Document CaptchaInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid,locator);
rapidjson::Value vimage(image.c_str(), image.size());
		document.AddMember("image", vimage,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CaptchaInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CaptchaInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if(document.HasMember("image"))
		{
			setImage(document["image"].GetString());
		}


	}
}
