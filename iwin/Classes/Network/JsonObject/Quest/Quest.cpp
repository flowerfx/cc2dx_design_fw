#include "Quest.h"
#include "json/writer.h"
namespace iwinmesage
{
	Quest::Quest()
	{
		actionType = QuestType::TYPE_ACCEPT_QUEST;
	}

	Quest::~Quest()
	{

	}

	rapidjson::Document Quest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value arrtextboxInfo(rapidjson::kArrayType);
		for (size_t i = 0; i < textboxInfo.size(); i++)
		{
			arrtextboxInfo.PushBack(rapidjson::Value().SetString(textboxInfo.at(i).c_str(), textboxInfo.at(i).size()), locator);
		}
		document.AddMember("textboxInfo", arrtextboxInfo, locator);
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid, locator);
		document.AddMember("actionType", (int)actionType, locator);
		rapidjson::Value vtitle(title.c_str(), title.size());
		document.AddMember("title", vtitle, locator);
		rapidjson::Value vdescription(description.c_str(), description.size());
		document.AddMember("description", vdescription, locator);
		rapidjson::Value vurlImage(urlImage.c_str(), urlImage.size());
		document.AddMember("urlImage", vurlImage, locator);
		document.AddMember("categoryId", categoryId, locator);
		document.AddMember("maxProgressIndex", maxProgressIndex, locator);
		document.AddMember("currentProgress", currentProgress, locator);
		document.AddMember("optionList", optionList.toJson(), locator);
		document.AddMember("confirmBox", confirmBox.toJson(), locator);
		rapidjson::Value vlink(link.c_str(), link.size());
		document.AddMember("link", vlink, locator);
		rapidjson::Value vbundleId(bundleId.c_str(), bundleId.size());
		document.AddMember("bundleId", vbundleId, locator);
		document.AddMember("screenID", screenID, locator);
		document.AddMember("gameId", gameId, locator);
		rapidjson::Value vbuttonTitle(buttonTitle.c_str(), buttonTitle.size());
		document.AddMember("buttonTitle", vbuttonTitle, locator);
		rapidjson::Value vfacebookAccessToken(facebookAccessToken.c_str(), facebookAccessToken.size());
		document.AddMember("facebookAccessToken", vfacebookAccessToken, locator);
		rapidjson::Value vjsonData(jsonData.c_str(), jsonData.size());
		document.AddMember("jsonData", vjsonData, locator);
		document.AddMember("totalTime", totalTime, locator);
		document.AddMember("remainTime", remainTime, locator);
		document.AddMember("money", money, locator);
		//rapidjson::Value vbonusEventId(bonusEventId.c_str(), bonusEventId.size());
		document.AddMember("bonusEventId", bonusEventId, locator);
		rapidjson::Value vchargeType(chargeType.c_str(), chargeType.size());
		document.AddMember("chargeType", vchargeType, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Quest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Quest::toData(rapidjson::Document & document)
	{
		if (document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if (document.HasMember("actionType"))
		{
			setActionType((QuestType)document["actionType"].GetInt());
		}
		if (document.HasMember("title"))
		{
			setTitle(document["title"].GetString());
		}
		if (document.HasMember("description"))
		{
			setDescription(document["description"].GetString());
		}
		if (document.HasMember("urlImage"))
		{
			setUrlImage(document["urlImage"].GetString());
		}
		if (document.HasMember("categoryId"))
		{
			setCategoryId(document["categoryId"].GetInt());
		}
		if (document.HasMember("maxProgressIndex"))
		{
			setMaxProgressIndex(document["maxProgressIndex"].GetInt());
		}
		if (document.HasMember("currentProgress"))
		{
			setCurrentProgress(document["currentProgress"].GetInt());
		}
		if (document.HasMember("optionList"))
		{
			const rapidjson::Value& coptionList = document["optionList"];
			rapidjson::StringBuffer bufferoptionList;
			bufferoptionList.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writeroptionList(bufferoptionList);
			coptionList.Accept(writeroptionList);
			const char* jsonoptionList = bufferoptionList.GetString();
			OptionList doptionList;
			doptionList.toData(jsonoptionList);
			setoptionList(doptionList);
		}
		if (document.HasMember("confirmBox"))
		{
			const rapidjson::Value& cconfirmBox = document["confirmBox"];
			rapidjson::StringBuffer bufferconfirmBox;
			bufferconfirmBox.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writerconfirmBox(bufferconfirmBox);
			cconfirmBox.Accept(writerconfirmBox);
			const char* jsonconfirmBox = bufferconfirmBox.GetString();
			ConfirmBox dconfirmBox;
			dconfirmBox.toData(jsonconfirmBox);
			setconfirmBox(dconfirmBox);

		}
		if (document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}
		if (document.HasMember("bundleId"))
		{
			setBundleId(document["bundleId"].GetString());
		}
		if (document.HasMember("screenID"))
		{
			setScreenID(document["screenID"].GetInt());
		}
		if (document.HasMember("gameId"))
		{
			setGameId(document["gameId"].GetInt());
		}
		if (document.HasMember("buttonTitle"))
		{
			setButtonTitle(document["buttonTitle"].GetString());
		}
		if (document.HasMember("facebookAccessToken"))
		{
			setFacebookAccessToken(document["facebookAccessToken"].GetString());
		}
		if (document.HasMember("jsonData"))
		{
			setJsonData(document["jsonData"].GetString());
		}
		if (document.HasMember("totalTime"))
		{
			setTotalTime(document["totalTime"].GetInt64());
		}
		if (document.HasMember("remainTime"))
		{
			setRemainTime(document["remainTime"].GetInt64());
		}
		if (document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}
		if (document.HasMember("bonusEventId"))
		{
			setBonusEventId(document["bonusEventId"].GetInt64());
		}
		if (document.HasMember("chargeType"))
		{

			setchargeType(document["chargeType"].GetString());
		}
		if (document.HasMember("textboxInfo") && !document["textboxInfo"].IsNull() && document["textboxInfo"].IsArray())
		{
			const rapidjson::Value& arrtextboxInfo = document["textboxInfo"];
			for (rapidjson::SizeType i = 0; i < arrtextboxInfo.Size(); i++)
			{
				textboxInfo.push_back(arrtextboxInfo[i].GetString());
			}
		}


	}
}
