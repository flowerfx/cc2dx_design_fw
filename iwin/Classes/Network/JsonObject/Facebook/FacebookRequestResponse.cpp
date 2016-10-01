#include "FacebookRequestResponse.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookRequestResponse:: FacebookRequestResponse ()
	{

	}

	FacebookRequestResponse::~FacebookRequestResponse()
	{

	}

	rapidjson::Document FacebookRequestResponse::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrto(rapidjson::kArrayType);
		for (size_t i = 0; i < to.size(); i++)
		{
			arrto.PushBack(rapidjson::Value().SetString(to.at(i).c_str(),to.at(i).size()), locator);
		}
		document.AddMember("to",arrto, locator);
rapidjson::Value vrequest(request.c_str(), request.size());
		document.AddMember("request", vrequest,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookRequestResponse::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookRequestResponse::toData(rapidjson::Document & document)
	{
				if(document.HasMember("request"))
		{
			setRequest(document["request"].GetString());
		}
		if(document.HasMember("to") && !document["to"].IsNull() && document["to"].IsArray() )
		{
			const rapidjson::Value& arrto = document["to"];
			for (rapidjson::SizeType i = 0; i < arrto.Size(); i++)
			{
				to.push_back(arrto[i].GetString());
			}
		}


	}
}
