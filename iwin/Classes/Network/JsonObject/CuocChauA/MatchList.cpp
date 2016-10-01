#include "MatchList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MatchList:: MatchList ()
	{

	}

	MatchList::~MatchList()
	{

	}

	rapidjson::Document MatchList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrmatchList(rapidjson::kArrayType);
		for (size_t i = 0; i < matchList.size(); i++)
		{
			arrmatchList.PushBack(matchList.at(i).toJson(), locator);
		}
		document.AddMember("matchList",arrmatchList, locator);
rapidjson::Value vpageToken(pageToken.c_str(), pageToken.size());
		document.AddMember("pageToken", vpageToken,locator);
		document.AddMember("leagueId", leagueId,locator);
		document.AddMember("startTime", startTime,locator);
		document.AddMember("countDownTime", countDownTime,locator);
		document.AddMember("status", status,locator);
rapidjson::Value vinfo(info.c_str(), info.size());
		document.AddMember("info", vinfo,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MatchList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MatchList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("pageToken"))
		{
			setPageToken(document["pageToken"].GetString());
		}
		if(document.HasMember("leagueId"))
		{
			setLeagueId(document["leagueId"].GetInt());
		}
		if(document.HasMember("startTime"))
		{
			setStartTime(document["startTime"].GetInt64());
		}
		if(document.HasMember("countDownTime"))
		{
			setCountDownTime(document["countDownTime"].GetInt64());
		}
		if(document.HasMember("status"))
		{
			setStatus(document["status"].GetInt());
		}
		if(document.HasMember("info"))
		{
			setInfo(document["info"].GetString());
		}
		if(document.HasMember("matchList") && !document["matchList"].IsNull() && document["matchList"].IsArray() )
		{
			const rapidjson::Value& arrmatchList = document["matchList"];
			for (rapidjson::SizeType i = 0; i < arrmatchList.Size(); i++)
			{
							const rapidjson::Value& c = arrmatchList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Match d;
			d.toData(json);
			matchList.push_back(d);

			}
		}


	}
}
