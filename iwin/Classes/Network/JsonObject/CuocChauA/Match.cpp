#include "Match.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Match:: Match ()
	{

	}

	Match::~Match()
	{

	}

	rapidjson::Document Match::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("leagueId", leagueId,locator);
		document.AddMember("id", id,locator);
		document.AddMember("owner", owner.toJson(),locator);
		document.AddMember("visitor", visitor.toJson(),locator);
		document.AddMember("ownerGoals", ownerGoals,locator);
		document.AddMember("visitorGoals", visitorGoals,locator);
		document.AddMember("ownerTickets", ownerTickets,locator);
		document.AddMember("visitorTickets", visitorTickets,locator);
		document.AddMember("time", time,locator);
		document.AddMember("ownerRate", ownerRate,locator);
		document.AddMember("visitorRate", visitorRate,locator);
		document.AddMember("downTime", downTime,locator);
		document.AddMember("winTeamID", winTeamID,locator);
		document.AddMember("matchStatus", matchStatus,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Match::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Match::toData(rapidjson::Document & document)
	{
				if(document.HasMember("leagueId"))
		{
			setLeagueId(document["leagueId"].GetInt());
		}
		if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
			if(document.HasMember("owner"))
			{
				const rapidjson::Value& cowner = document["owner"];
				rapidjson::StringBuffer bufferowner;
				bufferowner.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerowner(bufferowner);
				cowner.Accept(writerowner);
				const char* jsonowner = bufferowner.GetString();
				Team downer;
				downer.toData(jsonowner);
				setOwner(downer);
			}
			if(document.HasMember("visitor"))
			{
				const rapidjson::Value& cvisitor = document["visitor"];
				rapidjson::StringBuffer buffervisitor;
				buffervisitor.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writervisitor(buffervisitor);
				cvisitor.Accept(writervisitor);
				const char* jsonvisitor = buffervisitor.GetString();
				Team dvisitor;
				dvisitor.toData(jsonvisitor);
				setVisitor(dvisitor);
			}
		if(document.HasMember("ownerGoals"))
		{
			setOwnerGoals(document["ownerGoals"].GetInt());
		}
		if(document.HasMember("visitorGoals"))
		{
			setVisitorGoals(document["visitorGoals"].GetInt());
		}
		if(document.HasMember("ownerTickets"))
		{
			setOwnerTickets(document["ownerTickets"].GetInt());
		}
		if(document.HasMember("visitorTickets"))
		{
			setVisitorTickets(document["visitorTickets"].GetInt());
		}
		if(document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if(document.HasMember("ownerRate"))
		{
			setOwnerRate(document["ownerRate"].GetDouble());
		}
		if(document.HasMember("visitorRate"))
		{
			setVisitorRate(document["visitorRate"].GetDouble());
		}
		if(document.HasMember("downTime"))
		{
			setDownTime(document["downTime"].GetInt64());
		}
		if(document.HasMember("winTeamID"))
		{
			setWinTeamID(document["winTeamID"].GetInt());
		}
		if(document.HasMember("matchStatus"))
		{
			setMatchStatus(document["matchStatus"].GetInt());
		}


	}
}
