#include "MyMatch.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MyMatch:: MyMatch ()
	{

	}

	MyMatch::~MyMatch()
	{

	}

	rapidjson::Document MyMatch::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("match", match.toJson(),locator);
		document.AddMember("myTeamID", myTeamID,locator);
		document.AddMember("myTickets", myTickets,locator);
		document.AddMember("winMoney", winMoney,locator);
		document.AddMember("totalBetMoney", totalBetMoney,locator);
		document.AddMember("time", time,locator);
		document.AddMember("canGetWin", canGetWin,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MyMatch::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MyMatch::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
			if(document.HasMember("match"))
			{
				const rapidjson::Value& cmatch = document["match"];
				rapidjson::StringBuffer buffermatch;
				buffermatch.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writermatch(buffermatch);
				cmatch.Accept(writermatch);
				const char* jsonmatch = buffermatch.GetString();
				Match dmatch;
				dmatch.toData(jsonmatch);
				setMatch(dmatch);
			}
		if(document.HasMember("myTeamID"))
		{
			setMyTeamID(document["myTeamID"].GetInt());
		}
		if(document.HasMember("myTickets"))
		{
			setMyTickets(document["myTickets"].GetInt());
		}
		if(document.HasMember("winMoney"))
		{
			setWinMoney(document["winMoney"].GetInt64());
		}
		if(document.HasMember("totalBetMoney"))
		{
			setTotalBetMoney(document["totalBetMoney"].GetInt64());
		}
		if(document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if(document.HasMember("canGetWin"))
		{
			setCanGetWin(document["canGetWin"].GetBool());
		}


	}
}
