#include "ConfirmBox.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ConfirmBox:: ConfirmBox ()
	{

	}

	ConfirmBox::~ConfirmBox()
	{

	}

	rapidjson::Document ConfirmBox::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vconfirmTitle(confirmTitle.c_str(), confirmTitle.size());
		document.AddMember("confirmTitle", vconfirmTitle,locator);
rapidjson::Value vconfirmMsg(confirmMsg.c_str(), confirmMsg.size());
		document.AddMember("confirmMsg", vconfirmMsg,locator);
rapidjson::Value vyesButton(yesButton.c_str(), yesButton.size());
		document.AddMember("yesButton", vyesButton,locator);
rapidjson::Value vnoButton(noButton.c_str(), noButton.size());
		document.AddMember("noButton", vnoButton,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ConfirmBox::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ConfirmBox::toData(rapidjson::Document & document)
	{
				if(document.HasMember("confirmTitle"))
		{
			setConfirmTitle(document["confirmTitle"].GetString());
		}
		if(document.HasMember("confirmMsg"))
		{
			setConfirmMsg(document["confirmMsg"].GetString());
		}
		if(document.HasMember("yesButton"))
		{
			setYesButton(document["yesButton"].GetString());
		}
		if(document.HasMember("noButton"))
		{
			setNoButton(document["noButton"].GetString());
		}


	}
}
