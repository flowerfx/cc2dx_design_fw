#include "OpenFacebookRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	OpenFacebookRequest:: OpenFacebookRequest ()
	{

	}

	OpenFacebookRequest::~OpenFacebookRequest()
	{

	}

	rapidjson::Document OpenFacebookRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrids(rapidjson::kArrayType);
		for (size_t i = 0; i < ids.size(); i++)
		{
			arrids.PushBack(ids.at(i), locator);
		}
		document.AddMember("ids",arrids, locator);
rapidjson::Value vfbId(fbId.c_str(), fbId.size());
		document.AddMember("fbId", vfbId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void OpenFacebookRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void OpenFacebookRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("fbId"))
		{
			setFbId(document["fbId"].GetString());
		}
		if(document.HasMember("ids") && !document["ids"].IsNull() && document["ids"].IsArray() )
		{
			const rapidjson::Value& arrids = document["ids"];
			for (rapidjson::SizeType i = 0; i < arrids.Size(); i++)
			{
				ids.push_back(arrids[i].GetInt64());
			}
		}


	}
}
