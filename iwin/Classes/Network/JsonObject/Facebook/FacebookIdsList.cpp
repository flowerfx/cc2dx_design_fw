#include "FacebookIdsList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookIdsList:: FacebookIdsList ()
	{

	}

	FacebookIdsList::~FacebookIdsList()
	{

	}

	rapidjson::Document FacebookIdsList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrids(rapidjson::kArrayType);
		for (size_t i = 0; i < ids.size(); i++)
		{
			arrids.PushBack(rapidjson::Value().SetString(ids.at(i).c_str(),ids.at(i).size()), locator);
		}
		document.AddMember("ids",arrids, locator);
rapidjson::Value vmyId(myId.c_str(), myId.size());
		document.AddMember("myId", vmyId,locator);
rapidjson::Value vac(ac.c_str(), ac.size());
		document.AddMember("ac", vac,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookIdsList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookIdsList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("myId"))
		{
			setMyId(document["myId"].GetString());
		}
		if(document.HasMember("ac"))
		{
			setAc(document["ac"].GetString());
		}
		if(document.HasMember("ids") && !document["ids"].IsNull() && document["ids"].IsArray() )
		{
			const rapidjson::Value& arrids = document["ids"];
			for (rapidjson::SizeType i = 0; i < arrids.Size(); i++)
			{
				ids.push_back(arrids[i].GetString());
			}
		}


	}
}
