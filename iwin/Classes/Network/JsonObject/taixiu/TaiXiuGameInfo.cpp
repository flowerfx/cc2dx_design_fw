#include "TaiXiuGameInfo.h"
#include "json/writer.h"

namespace iwinmesage
{
	TaiXiuGameInfo::TaiXiuGameInfo()
	{
		/**
		* thời gian (ms) đặt cược còn lại
		* */
		timeBetRemain = 0;
		/**
		* thời gian (ms) xem kết quả còn lại
		* */
		timeShowResultRemain = 0;
		/**
		* tổng số user đặt cược vào cửa Tài
		* */
		totalUserTai = 0;
		/**
		* tổng số user đặt cược vào cửa Xỉu
		* */
		totalUserXiu = 0;
		/**
		* tổng số win đặt cửa Tài
		* */
		totalWinTai = 0;
		/**
		* tổng số win đặt cửa Xỉu
		* */
		totalWinXiu = 0;
		/**
		* số win của user gửi request đã đặt cửa Tài
		* */
		winTai = 0;
		/**
		* số win của user gửi request đã đặt cửa Xỉu
		* */
		winXiu = 0;
		/**
		* số nút của 3 xúc xắc
		* */
		dice1 = dice2 = dice3 = 0;
		/**
		* kết quả tài/xỉu (0,1)
		* */
		result = 0;
		/**
		* số win user được nhận
		* */
		winReceived = 0;
		/**
		* số win đặt cược tối thiểu
		* */
		minBet = 0;
		/**
		* số win đặt cược tối đa
		* */
		maxBet = 0;
		/**
		* Mảng chứa lịch sử kết quả
		* */
		historyList.clear();
		/**
		* tổng thời gian đặt cược
		* */
		timeBetTotal = 0;
		/**
		* tổng thời gian show kết quả
		* */
		timeShowResultTotal = 0;
	}

	TaiXiuGameInfo::~TaiXiuGameInfo()
	{
		historyList.clear();
	}


	rapidjson::Document TaiXiuGameInfo::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("timeBetRemain", timeBetRemain, locator);
		document.AddMember("timeShowResultRemain", timeShowResultRemain, locator);
		document.AddMember("totalUserTai", totalUserTai, locator);
		document.AddMember("totalUserXiu", totalUserXiu, locator);
		document.AddMember("totalWinTai", totalWinTai, locator);
		document.AddMember("totalWinXiu", totalWinXiu, locator);
		document.AddMember("winTai", winTai, locator);
		document.AddMember("winXiu", winXiu, locator);

		document.AddMember("dice1", dice1, locator);
		document.AddMember("dice2", dice2, locator);
		document.AddMember("dice3", dice3, locator);

		document.AddMember("result", result, locator);
		document.AddMember("winReceived", winReceived, locator);
		document.AddMember("minBet", minBet, locator);
		document.AddMember("maxBet", maxBet, locator);
		rapidjson::Value arrhistoryList(rapidjson::kArrayType);
		for (size_t i = 0; i < historyList.size(); i++)
		{
			arrhistoryList.PushBack(rapidjson::Value().SetUint(historyList[i]), locator);
		}
		document.AddMember("historyList", arrhistoryList, locator);

		document.AddMember("timeBetTotal", timeBetTotal, locator);
		document.AddMember("timeShowResultTotal", timeShowResultTotal, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuGameInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuGameInfo::toData(rapidjson::Document & document)
	{
		if (document.HasMember("timeBetRemain"))
		{
			setTimeBetRemain(document["timeBetRemain"].GetInt64());
		}
		if (document.HasMember("timeShowResultRemain"))
		{
			setTimeShowResultRemain(document["timeShowResultRemain"].GetInt64());
		}
		if (document.HasMember("totalUserTai"))
		{
			setTotalUserTai(document["totalUserTai"].GetInt());
		}
		if (document.HasMember("totalUserXiu"))
		{
			setTotalUserXiu(document["totalUserXiu"].GetInt());
		}
		if (document.HasMember("totalWinTai"))
		{
			setTotalWinTai(document["totalWinTai"].GetInt64());
		}
		if (document.HasMember("totalWinXiu"))
		{
			setTotalWinXiu(document["totalWinXiu"].GetInt64());
		}
		if (document.HasMember("winTai"))
		{
			setWinTai(document["winTai"].GetInt64());
		}
		if (document.HasMember("winXiu"))
		{
			setWinXiu(document["winXiu"].GetInt64());
		}
		if (document.HasMember("dice1"))
		{
			setDice1(document["dice1"].GetUint());
		}
		if (document.HasMember("dice2"))
		{
			setDice2(document["dice2"].GetUint());
		}
		if (document.HasMember("dice3"))
		{
			setDice3(document["dice3"].GetUint());
		}
		if (document.HasMember("result"))
		{
			setResult(document["result"].GetUint());
		}
		if (document.HasMember("winReceived"))
		{
			setWinReceived(document["winReceived"].GetInt64());
		}
		if (document.HasMember("minBet"))
		{
			setMinBet(document["minBet"].GetInt64());
		}
		if (document.HasMember("maxBet"))
		{
			setMaxBet(document["maxBet"].GetInt64());
		}
		if (document.HasMember("historyList") && !document["historyList"].IsNull() && document["historyList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["historyList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				historyList.push_back(arrlist[i].GetUint());
			}
		}
		if (document.HasMember("timeBetTotal"))
		{
			setTimeBetTotal(document["timeBetTotal"].GetInt());
		}
		if (document.HasMember("timeShowResultTotal"))
		{
			setTimeShowResultTotal(document["timeShowResultTotal"].GetInt());
		}
	}
}