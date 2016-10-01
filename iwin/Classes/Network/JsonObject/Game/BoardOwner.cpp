#include "BoardOwner.h"
#include "json/writer.h"
namespace iwinmesage
{
	std::string BoardOwner::J2ME_NAME = "j2me";
std::string BoardOwner::IOS_NAME = "ios";
std::string BoardOwner::ANDROID_NAME = "android";
std::string BoardOwner::DESKTOP_NAME = "desktop";
std::string BoardOwner::WEB_APP_NAME = "web";
std::string BoardOwner::WINDOWS_PHONE_NAME = "WindowsPhone8";
std::string BoardOwner::UNKNOW_NAME = "unknow";

	BoardOwner:: BoardOwner ()
	{

	}

	BoardOwner::~BoardOwner()
	{

	}

	rapidjson::Document BoardOwner::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername,locator);
		document.AddMember("money", money,locator);
		document.AddMember("avatar", avatar,locator);
		document.AddMember("exp", exp,locator);
		document.AddMember("win", win,locator);
		document.AddMember("lose", lose,locator);
		document.AddMember("draw", draw,locator);
		document.AddMember("leave", leave,locator);
		document.AddMember("roomId", roomId,locator);
		document.AddMember("boardId", boardId,locator);
		document.AddMember("password", password,locator);
		document.AddMember("boardMoney", boardMoney,locator);
		document.AddMember("maxBet", maxBet,locator);
		document.AddMember("playing", playing,locator);
		document.AddMember("countPlayer", countPlayer,locator);
		document.AddMember("countMaxPlayer", countMaxPlayer,locator);
		document.AddMember("boardLevel", boardLevel,locator);
rapidjson::Value vplatform(platform.c_str(), platform.size());
		document.AddMember("platform", vplatform,locator);
rapidjson::Value vformatedUsername(formatedUsername.c_str(), formatedUsername.size());
		document.AddMember("formatedUsername", vformatedUsername,locator);
rapidjson::Value vformatedMoney(formatedMoney.c_str(), formatedMoney.size());
		document.AddMember("formatedMoney", vformatedMoney,locator);
rapidjson::Value vformatedWin(formatedWin.c_str(), formatedWin.size());
		document.AddMember("formatedWin", vformatedWin,locator);
rapidjson::Value vformatedBetMoney(formatedBetMoney.c_str(), formatedBetMoney.size());
		document.AddMember("formatedBetMoney", vformatedBetMoney,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void BoardOwner::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void BoardOwner::toData(rapidjson::Document & document)
	{
				if(document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}
		if(document.HasMember("avatar"))
		{
			setAvatar(document["avatar"].GetInt());
		}
		if(document.HasMember("exp"))
		{
			setExp(document["exp"].GetInt());
		}
		if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt());
		}
		if(document.HasMember("lose"))
		{
			setLose(document["lose"].GetInt());
		}
		if(document.HasMember("draw"))
		{
			setDraw(document["draw"].GetInt());
		}
		if(document.HasMember("leave"))
		{
			setLeave(document["leave"].GetInt());
		}
		if(document.HasMember("roomId"))
		{
			setRoomId(document["roomId"].GetInt());
		}
		if(document.HasMember("boardId"))
		{
			setBoardId(document["boardId"].GetInt());
		}
		if(document.HasMember("password"))
		{
			setPassword(document["password"].GetBool());
		}
		if(document.HasMember("boardMoney"))
		{
			setBoardMoney(document["boardMoney"].GetInt());
		}
		if(document.HasMember("maxBet"))
		{
			setMaxBet(document["maxBet"].GetInt());
		}
		if(document.HasMember("playing"))
		{
			setPlaying(document["playing"].GetBool());
		}
		if(document.HasMember("countPlayer"))
		{
			setCountPlayer(document["countPlayer"].GetInt());
		}
		if(document.HasMember("countMaxPlayer"))
		{
			setCountMaxPlayer(document["countMaxPlayer"].GetInt());
		}
		if(document.HasMember("boardLevel"))
		{
			setBoardLevel(document["boardLevel"].GetInt());
		}
		if(document.HasMember("platform"))
		{
			setPlatform(document["platform"].GetString());
		}
		if(document.HasMember("formatedUsername"))
		{
			setFormatedUsername(document["formatedUsername"].GetString());
		}
		if(document.HasMember("formatedMoney"))
		{
			setFormatedMoney(document["formatedMoney"].GetString());
		}
		if(document.HasMember("formatedWin"))
		{
			setFormatedWin(document["formatedWin"].GetString());
		}
		if(document.HasMember("formatedBetMoney"))
		{
			setFormatedBetMoney(document["formatedBetMoney"].GetString());
		}


	}
}
