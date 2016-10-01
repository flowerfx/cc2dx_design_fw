#include "GameEvent.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GameEvent:: GameEvent ()
	{

	}

	GameEvent::~GameEvent()
	{

	}

	rapidjson::Document GameEvent::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("gameId", gameId,locator);
rapidjson::Value vbannerImg(bannerImg.c_str(), bannerImg.size());
		document.AddMember("bannerImg", vbannerImg,locator);
rapidjson::Value vpopupImg(popupImg.c_str(), popupImg.size());
		document.AddMember("popupImg", vpopupImg,locator);
rapidjson::Value vpopupIcoImg(popupIcoImg.c_str(), popupIcoImg.size());
		document.AddMember("popupIcoImg", vpopupIcoImg,locator);
		document.AddMember("screenId", screenId,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GameEvent::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GameEvent::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if(document.HasMember("bannerImg"))
		{
			setBannerImg(document["bannerImg"].GetString());
		}
		if(document.HasMember("popupImg"))
		{
			setPopupImg(document["popupImg"].GetString());
		}
		if(document.HasMember("popupIcoImg"))
		{
			setPopupIcoImg(document["popupIcoImg"].GetString());
		}
		if(document.HasMember("screenId"))
		{
			setScreenId(document["screenId"].GetInt());
		}


	}
}
