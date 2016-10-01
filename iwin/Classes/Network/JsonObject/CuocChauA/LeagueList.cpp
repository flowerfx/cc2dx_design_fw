#include "LeagueList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LeagueList:: LeagueList ()
	{

	}

	LeagueList::~LeagueList()
	{

	}

	rapidjson::Document LeagueList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrleagueList(rapidjson::kArrayType);
		for (size_t i = 0; i < leagueList.size(); i++)
		{
			arrleagueList.PushBack(leagueList.at(i).toJson(), locator);
		}
		document.AddMember("leagueList",arrleagueList, locator);
		document.AddMember("currentRound", currentRound,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LeagueList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LeagueList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("currentRound"))
		{
			setCurrentRound(document["currentRound"].GetInt());
		}
		if(document.HasMember("leagueList") && !document["leagueList"].IsNull() && document["leagueList"].IsArray() )
		{
			const rapidjson::Value& arrleagueList = document["leagueList"];
			for (rapidjson::SizeType i = 0; i < arrleagueList.Size(); i++)
			{
							const rapidjson::Value& c = arrleagueList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			League d;
			d.toData(json);
			leagueList.push_back(d);

			}
		}


	}
}
