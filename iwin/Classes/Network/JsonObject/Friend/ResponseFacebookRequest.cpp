#include "ResponseFacebookRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseFacebookRequest:: ResponseFacebookRequest ()
	{

	}

	ResponseFacebookRequest::~ResponseFacebookRequest()
	{

	}

	rapidjson::Document ResponseFacebookRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrfacebookRequests(rapidjson::kArrayType);
		for (size_t i = 0; i < facebookRequests.size(); i++)
		{
			arrfacebookRequests.PushBack(facebookRequests.at(i).toJson(), locator);
		}
		document.AddMember("facebookRequests",arrfacebookRequests, locator);
		document.AddMember("maxGiftOpen", maxGiftOpen,locator);
rapidjson::Value vdescription(description.c_str(), description.size());
		document.AddMember("description", vdescription,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseFacebookRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseFacebookRequest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("maxGiftOpen"))
		{
			setMaxGiftOpen(document["maxGiftOpen"].GetInt());
		}
		if(document.HasMember("description"))
		{
			setDescription(document["description"].GetString());
		}
		if(document.HasMember("facebookRequests") && !document["facebookRequests"].IsNull() && document["facebookRequests"].IsArray() )
		{
			const rapidjson::Value& arrfacebookRequests = document["facebookRequests"];
			for (rapidjson::SizeType i = 0; i < arrfacebookRequests.Size(); i++)
			{
							const rapidjson::Value& c = arrfacebookRequests[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			FacebookRequest d;
			d.toData(json);
			facebookRequests.push_back(d);

			}
		}


	}
}
