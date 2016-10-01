#include "ReviewApp.h"
#include "json/writer.h"
namespace iwinmesage
{
	int ReviewApp::IOS = 0;
int ReviewApp::OS_MAC = 1;
int ReviewApp::OS_ANDROID = 2;
int ReviewApp::OS_WINDOWS = 3;

	ReviewApp:: ReviewApp ()
	{

	}

	ReviewApp::~ReviewApp()
	{

	}

	rapidjson::Document ReviewApp::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vbundle(bundle.c_str(), bundle.size());
		document.AddMember("bundle", vbundle,locator);
rapidjson::Value vversion(version.c_str(), version.size());
		document.AddMember("version", vversion,locator);
		document.AddMember("status", status,locator);
		document.AddMember("platform", platform,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ReviewApp::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ReviewApp::toData(rapidjson::Document & document)
	{
				if(document.HasMember("bundle"))
		{
			setBundle(document["bundle"].GetString());
		}
		if(document.HasMember("version"))
		{
			setVersion(document["version"].GetString());
		}
		if(document.HasMember("status"))
		{
			setStatus(document["status"].GetInt());
		}
		if(document.HasMember("platform"))
		{
			setPlatform(document["platform"].GetInt());
		}


	}
}
