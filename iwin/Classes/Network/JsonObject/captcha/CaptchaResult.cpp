#include "CaptchaResult.h"
#include "json/writer.h"
namespace iwinmesage
{

	CaptchaResult:: CaptchaResult ()
	{
		code = CaptchaCheckType::SUCCESS;
	}

	CaptchaResult::~CaptchaResult()
	{

	}

	rapidjson::Document CaptchaResult::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("code", (int)code,locator);
rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CaptchaResult::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CaptchaResult::toData(rapidjson::Document & document)
	{
		if(document.HasMember("code"))
		{
			int current_code = document["code"].GetInt();
			setCode((CaptchaResult::CaptchaCheckType)current_code);
		}
		if(document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}


	}
}
