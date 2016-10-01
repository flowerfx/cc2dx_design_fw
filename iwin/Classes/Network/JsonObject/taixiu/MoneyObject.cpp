#include "MoneyObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	MoneyObject::MoneyObject() {
		moneyLong = 0;
		moneyShort = "0";
	}

	MoneyObject::MoneyObject(s64 _moneyLong, std::string _moneyShort) {
		moneyLong = _moneyLong;
		moneyShort = _moneyShort;
	}

	MoneyObject::~MoneyObject() {

	}

	void MoneyObject::setMoneyObject(s64 _moneyLong, std::string _moneyShort) {
		moneyLong = _moneyLong;
		moneyShort = _moneyShort;
	}

	rapidjson::Document MoneyObject::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value vmoneyShort(moneyShort.c_str(), moneyShort.size());
		document.AddMember("moneyShort", vmoneyShort, locator);
		document.AddMember("moneyLong", moneyLong, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MoneyObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MoneyObject::toData(rapidjson::Document & document)
	{
		if (document.HasMember("moneyLong"))
		{
			setMoneyLong(document["moneyLong"].GetInt64());
		}
		if (document.HasMember("moneyShort"))
		{
			setMoneyShort(document["moneyShort"].GetString());
		}
	}
}
