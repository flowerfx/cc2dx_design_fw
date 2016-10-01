#include "ErrorInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ErrorInfo:: ErrorInfo ()
	{

	}

	ErrorInfo::~ErrorInfo()
	{

	}

	rapidjson::Document ErrorInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vcmd(cmd.c_str(), cmd.size());
		document.AddMember("cmd", vcmd,locator);
rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ErrorInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ErrorInfo::toData(rapidjson::Document & document)
	{
				if(document.HasMember("cmd"))
		{
			setCmd(document["cmd"].GetString());
		}
		if(document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}


	}
}
