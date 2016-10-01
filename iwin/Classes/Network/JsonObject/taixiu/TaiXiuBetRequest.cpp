#include "TaiXiuBetRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuBetRequest::TaiXiuBetRequest()
	{
		username = "";
		betWin = 0;
		betChoice = 0;
	}

	TaiXiuBetRequest::TaiXiuBetRequest(std::string name, s64 bet, unsigned int choice)
	{
		username = name;
		betWin = bet;
		betChoice = choice;
	}

	TaiXiuBetRequest::~TaiXiuBetRequest()
	{

	}

	rapidjson::Document TaiXiuBetRequest::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value vname(username.c_str(), username.size());
		document.AddMember("username", vname, locator);
		document.AddMember("betWin", betWin, locator);
		document.AddMember("betChoice", betChoice, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuBetRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuBetRequest::toData(rapidjson::Document & document)
	{
		if (document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if (document.HasMember("betWin"))
		{
			setBetWin(document["betWin"].GetInt64());
		}
		if (document.HasMember("betChoice"))
		{
			setBetChoice(document["betChoice"].GetUint());
		}
	}
}