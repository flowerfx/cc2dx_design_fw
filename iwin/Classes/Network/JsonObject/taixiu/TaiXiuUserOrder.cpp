#include "TaiXiuUserOrder.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUserOrder::TaiXiuUserOrder()
	{
		username = "";
		order = 0;
		win = 0;
	}
	TaiXiuUserOrder::TaiXiuUserOrder(std::string name, ubyte ord , s64 w)
	{
		username = name;
		order = ord ;
		win = w;
	}
	TaiXiuUserOrder::~TaiXiuUserOrder()
	{

	}

	rapidjson::Document TaiXiuUserOrder::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value vname(username.c_str(), username.size());
		document.AddMember("username", vname, locator);
		document.AddMember("order", order, locator);
		document.AddMember("win", win, locator);


		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUserOrder::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUserOrder::toData(rapidjson::Document & document)
	{
		if (document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if (document.HasMember("order"))
		{
			setOrder(document["order"].GetUint());
		}
		if (document.HasMember("win"))
		{
			setWin(document["win"].GetInt64());
		}

	}
}