#include "RequestFacebookRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestFacebookRequest:: RequestFacebookRequest ()
	{

	}

	RequestFacebookRequest::~RequestFacebookRequest()
	{

	}

	rapidjson::Document RequestFacebookRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlistFbIdRequest(rapidjson::kArrayType);
		for (size_t i = 0; i < listFbIdRequest.size(); i++)
		{
			arrlistFbIdRequest.PushBack(rapidjson::Value().SetString(listFbIdRequest.at(i).c_str(),listFbIdRequest.at(i).size()), locator);
		}
		document.AddMember("listFbIdRequest",arrlistFbIdRequest, locator);
rapidjson::Value vrequestId(requestId.c_str(), requestId.size());
		document.AddMember("requestId", vrequestId,locator);
rapidjson::Value vaccessToken(accessToken.c_str(), accessToken.size());
		document.AddMember("accessToken", vaccessToken,locator);
		document.AddMember("type", type,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestFacebookRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestFacebookRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("requestId"))
		{
			setRequestId(document["requestId"].GetString());
		}
		if(document.HasMember("accessToken"))
		{
			setAccessToken(document["accessToken"].GetString());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if(document.HasMember("listFbIdRequest") && !document["listFbIdRequest"].IsNull() && document["listFbIdRequest"].IsArray() )
		{
			const rapidjson::Value& arrlistFbIdRequest = document["listFbIdRequest"];
			for (rapidjson::SizeType i = 0; i < arrlistFbIdRequest.Size(); i++)
			{
				listFbIdRequest.push_back(arrlistFbIdRequest[i].GetString());
			}
		}


	}
}
