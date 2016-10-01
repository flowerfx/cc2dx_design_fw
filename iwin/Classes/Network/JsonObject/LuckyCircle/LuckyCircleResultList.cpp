#include "LuckyCircleResultList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LuckyCircleResultList:: LuckyCircleResultList ()
	{

	}

	LuckyCircleResultList::~LuckyCircleResultList()
	{

	}

	rapidjson::Document LuckyCircleResultList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < list.size(); i++)
		{
			arrlist.PushBack(list.at(i).toJson(), locator);
		}
		document.AddMember("list",arrlist, locator);
		document.AddMember("circleId", circleId,locator);
		document.AddMember("tiketsObject", tiketsObject.toJson(),locator);
		document.AddMember("userPoint", userPoint,locator);
rapidjson::Value vresultString(resultString.c_str(), resultString.size());
		document.AddMember("resultString", vresultString,locator);
		document.AddMember("bonusMoney", bonusMoney,locator);
		document.AddMember("bonusType", bonusType,locator);
		document.AddMember("winsDown", winsDown,locator);
		document.AddMember("ticketsDown", ticketsDown,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LuckyCircleResultList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LuckyCircleResultList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("circleId"))
		{
			setCircleId(document["circleId"].GetInt());
		}
			if(document.HasMember("tiketsObject"))
			{
				const rapidjson::Value& ctiketsObject = document["tiketsObject"];
				rapidjson::StringBuffer buffertiketsObject;
				buffertiketsObject.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writertiketsObject(buffertiketsObject);
				ctiketsObject.Accept(writertiketsObject);
				const char* jsontiketsObject = buffertiketsObject.GetString();
				NewLuckyGetTiketsObject dtiketsObject;
				dtiketsObject.toData(jsontiketsObject);
				setTiketsObject(dtiketsObject);
			}
		if(document.HasMember("userPoint"))
		{
			setUserPoint(document["userPoint"].GetInt());
		}
		if(document.HasMember("resultString"))
		{
			setResultString(document["resultString"].GetString());
		}
		if(document.HasMember("bonusMoney"))
		{
			setBonusMoney(document["bonusMoney"].GetInt64());
		}
		if(document.HasMember("bonusType"))
		{
			setBonusType(document["bonusType"].GetInt());
		}
		if(document.HasMember("winsDown"))
		{
			setWinsDown(document["winsDown"].GetInt64());
		}
		if(document.HasMember("ticketsDown"))
		{
			setTicketsDown(document["ticketsDown"].GetInt());
		}
		if(document.HasMember("list") && !document["list"].IsNull() && document["list"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["list"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
							const rapidjson::Value& c = arrlist[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			LuckyCircleResultListItem d;
			d.toData(json);
			list.push_back(d);

			}
		}


	}
}
