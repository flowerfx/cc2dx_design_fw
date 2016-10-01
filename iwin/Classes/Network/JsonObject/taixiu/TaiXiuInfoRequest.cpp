#include "TaiXiuInfoRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuInfoRequest::TaiXiuInfoRequest()
	{
		userName = "";
	}

	TaiXiuInfoRequest::TaiXiuInfoRequest(std::string name)
	{
		userName = name;
	}


	TaiXiuInfoRequest::~TaiXiuInfoRequest()
	{

	}

	rapidjson::Document TaiXiuInfoRequest::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value vname(userName.c_str(), userName.size());
		document.AddMember("userName", vname, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuInfoRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuInfoRequest::toData(rapidjson::Document & document)
	{
		if (document.HasMember("userName"))
		{
			setUserName(document["userName"].GetString());
		}
	}
}