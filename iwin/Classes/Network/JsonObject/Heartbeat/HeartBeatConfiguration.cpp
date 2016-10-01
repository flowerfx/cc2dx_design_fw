#include "HeartBeatConfiguration.h"
#include "json/writer.h"
namespace iwinmesage
{
	int HeartBeatConfiguration::WEAK_TIME_IN_SECOND = 10;
int HeartBeatConfiguration::SLOW_TIME_IN_SECOND = 5;
int HeartBeatConfiguration::NORMAL_TIME_IN_SECOND = 1;

	HeartBeatConfiguration:: HeartBeatConfiguration ()
	{

	}

	HeartBeatConfiguration::~HeartBeatConfiguration()
	{

	}

	rapidjson::Document HeartBeatConfiguration::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("on", on,locator);
		document.AddMember("interval", interval,locator);
		document.AddMember("timeout", timeout,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void HeartBeatConfiguration::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void HeartBeatConfiguration::toData(rapidjson::Document & document)
	{
				if(document.HasMember("on"))
		{
			setOn(document["on"].GetBool());
		}
		if(document.HasMember("interval"))
		{
			setInterval(document["interval"].GetInt64());
		}
		if(document.HasMember("timeout"))
		{
			setTimeout(document["timeout"].GetInt64());
		}


	}
}
