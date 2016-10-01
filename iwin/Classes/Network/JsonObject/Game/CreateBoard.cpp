#include "CreateBoard.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CreateBoard:: CreateBoard ()
	{

	}

	CreateBoard::~CreateBoard()
	{

	}

	rapidjson::Document CreateBoard::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("money", money,locator);
		document.AddMember("maxPlayer", maxPlayer,locator);
rapidjson::Value vpassword(password.c_str(), password.size());
		document.AddMember("password", vpassword,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CreateBoard::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CreateBoard::toData(rapidjson::Document & document)
	{
				if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt());
		}
		if(document.HasMember("maxPlayer"))
		{
			setMaxPlayer(document["maxPlayer"].GetInt());
		}
		if(document.HasMember("password"))
		{
			setPassword(document["password"].GetString());
		}


	}
}
