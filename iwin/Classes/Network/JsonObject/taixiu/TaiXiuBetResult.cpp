#include "TaiXiuBetResult.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuBetResult::TaiXiuBetResult()
	{
		code = 0;
		message = "";
		betChoice = 0;
		betWin = 0;
		userBetWin = 0;
		totalBetWin = 0;
		totalUserBet = 0;
		minBet = 0;
		maxBet = 0;
	}

	TaiXiuBetResult::~TaiXiuBetResult()
	{

	}

	rapidjson::Document TaiXiuBetResult::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("code", code, locator);
		rapidjson::Value vname(message.c_str(), message.size());
		document.AddMember("message", vname, locator);
		document.AddMember("betChoice", betChoice, locator);
		document.AddMember("betWin", betWin, locator);
		document.AddMember("userBetWin", userBetWin, locator);
		document.AddMember("totalBetWin", totalBetWin, locator);
		document.AddMember("totalUserBet", totalUserBet, locator);
		document.AddMember("minBet", minBet, locator);
		document.AddMember("maxBet", maxBet, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuBetResult::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuBetResult::toData(rapidjson::Document & document)
	{
		if (document.HasMember("code"))
		{
			setCode(document["code"].GetUint());
		}
		if (document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if (document.HasMember("betChoice"))
		{
			setBetChoice(document["betChoice"].GetUint());
		}
		if (document.HasMember("betWin"))
		{
			setBetWin(document["betWin"].GetInt64());
		}
		if (document.HasMember("userBetWin"))
		{
			setUserBetWin(document["userBetWin"].GetInt64());
		}
		if (document.HasMember("totalBetWin"))
		{
			setTotalBetWin(document["totalBetWin"].GetInt64());
		}
		if (document.HasMember("totalUserBet"))
		{
			setTotalUserBet(document["totalUserBet"].GetInt64());
		}
		if (document.HasMember("minBet"))
		{
			setMinBet(document["minBet"].GetInt64());
		}
		if (document.HasMember("maxBet"))
		{
			setMaxBet(document["maxBet"].GetInt64());
		}
	}
}