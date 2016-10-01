#include "BetStars.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	BetStars:: BetStars ()
	{

	}

	BetStars::~BetStars()
	{

	}

	rapidjson::Document BetStars::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrdices(rapidjson::kArrayType);
		for (size_t i = 0; i < dices.size(); i++)
		{
			arrdices.PushBack(dices.at(i), locator);
		}
		document.AddMember("dices",arrdices, locator);
		document.AddMember("numStars", numStars,locator);
		document.AddMember("win", win,locator);
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
		document.AddMember("isError", isError,locator);
		document.AddMember("myStars", myStars,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void BetStars::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void BetStars::toData(rapidjson::Document & document)
	{
				if(document.HasMember("numStars"))
		{
			setNumStars(document["numStars"].GetInt64());
		}
		if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt64());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("isError"))
		{
			setIsError(document["isError"].GetBool());
		}
		if(document.HasMember("myStars"))
		{
			setMyStars(document["myStars"].GetInt64());
		}
		if(document.HasMember("dices") && !document["dices"].IsNull() && document["dices"].IsArray() )
		{
			const rapidjson::Value& arrdices = document["dices"];
			for (rapidjson::SizeType i = 0; i < arrdices.Size(); i++)
			{
				dices.push_back(arrdices[i].GetInt());
			}
		}


	}
}
