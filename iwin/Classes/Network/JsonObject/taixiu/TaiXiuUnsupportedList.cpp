#include "TaiXiuUnsupportedList.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUnsupportedList::TaiXiuUnsupportedList()
	{
		//Game không cho mở Tài xỉu
		gameList.clear();
		//Screen không cho mở Tài xỉu
		screenList.clear();

		/**
		* trang thai mo tai xiu win
		* */
		isWinEnabled = true;
		/**
		* trang thai mo tai xiu ruby
		* */
		isRubyEnabled = false;
	}
	TaiXiuUnsupportedList::~TaiXiuUnsupportedList()
	{
		//Game không cho mở Tài xỉu
		gameList.clear();
		//Screen không cho mở Tài xỉu
		screenList.clear();
	}

	rapidjson::Document TaiXiuUnsupportedList::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value arrgameList(rapidjson::kArrayType);
		for (size_t i = 0; i < gameList.size(); i++)
		{
			arrgameList.PushBack(rapidjson::Value().SetInt(gameList[i]), locator);
		}
		document.AddMember("gameList", arrgameList, locator);
		rapidjson::Value arrscreenList(rapidjson::kArrayType);
		for (size_t i = 0; i < screenList.size(); i++)
		{
			arrscreenList.PushBack(rapidjson::Value().SetInt(screenList[i]), locator);
		}
		document.AddMember("screenList", arrscreenList, locator);
		document.AddMember("isWinEnabled", isWinEnabled, locator);
		document.AddMember("isRubyEnabled", isRubyEnabled, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUnsupportedList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUnsupportedList::toData(rapidjson::Document & document)
	{
		if (document.HasMember("gameList") && !document["gameList"].IsNull() && document["gameList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["gameList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				gameList.push_back(arrlist[i].GetInt());
			}
		}
		if (document.HasMember("screenList") && !document["screenList"].IsNull() && document["screenList"].IsArray())
		{
			const rapidjson::Value& arrlist = document["screenList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				screenList.push_back(arrlist[i].GetInt());
			}
		}
		if (document.HasMember("isWinEnabled"))
		{
			setIsWinEnabled(document["isWinEnabled"].GetBool());
		}
		if (document.HasMember("isRubyEnabled"))
		{
			setIsRubyEnabled(document["isRubyEnabled"].GetBool());
		}

	}
}