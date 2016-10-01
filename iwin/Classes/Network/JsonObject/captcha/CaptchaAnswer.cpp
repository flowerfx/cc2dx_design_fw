#include "CaptchaAnswer.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CaptchaAnswer:: CaptchaAnswer ()
	{

	}

	CaptchaAnswer::~CaptchaAnswer()
	{

	}

	rapidjson::Document CaptchaAnswer::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid,locator);
rapidjson::Value vanswer(answer.c_str(), answer.size());
		document.AddMember("answer", vanswer,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CaptchaAnswer::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CaptchaAnswer::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if(document.HasMember("answer"))
		{
			setAnswer(document["answer"].GetString());
		}


	}
}
