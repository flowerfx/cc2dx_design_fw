#include "TaiXiuUserHistory.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUserHistory::TaiXiuUserHistory()
	{
		/**
		* id của ván chơi
		*/
		matchId = 0;
		/**
		* thời gian đặt cược
		*/
		time = 0;
		/**
		* cửa đặt
		*/
		betChoice = 0;
		/**
		* kết quả
		*/
		resultType = 0;
		/**
		* số nút xúc xắc + tài/xỉu (vd: 1-3-1:Xỉu)
		*/
		result = "";
		/**
		* số win đặt cược
		*/
		betWin = 0;
		/**
		* số win trả ngược lại khi có vấn đề
		*/
		returnWin = 0;
		/**
		* số win nhận được
		*/
		receivedWin = 0;
	}

	TaiXiuUserHistory::~TaiXiuUserHistory()
	{

	}

	rapidjson::Document TaiXiuUserHistory::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("matchId", matchId, locator);
		document.AddMember("time", time, locator);
		document.AddMember("betChoice", betChoice, locator);
		document.AddMember("resultType", resultType, locator);

		rapidjson::Value vname(result.c_str(), result.size());
		document.AddMember("result", vname, locator);

		document.AddMember("betWin", betWin, locator);
		document.AddMember("returnWin", returnWin, locator);
		document.AddMember("receivedWin", receivedWin, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUserHistory::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUserHistory::toData(rapidjson::Document & document)
	{
		if (document.HasMember("matchId"))
		{
			setMatchId(document["matchId"].GetInt64());
		}
		if (document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if (document.HasMember("betChoice"))
		{
			setBetChoice(document["betChoice"].GetUint());
		}
		if (document.HasMember("resultType"))
		{
			setResultType(document["resultType"].GetUint());
		}
		if (document.HasMember("result"))
		{
			setResult(document["result"].GetString());
		}
		if (document.HasMember("betWin"))
		{
			setBetWin(document["betWin"].GetInt64());
		}
		if (document.HasMember("returnWin"))
		{
			setReturnWin(document["returnWin"].GetInt64());
		}
		if (document.HasMember("receivedWin"))
		{
			setReceivedWin(document["receivedWin"].GetInt64());
		}
	}
}