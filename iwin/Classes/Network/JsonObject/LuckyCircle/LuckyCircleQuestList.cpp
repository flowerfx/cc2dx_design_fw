#include "LuckyCircleQuestList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LuckyCircleQuestList:: LuckyCircleQuestList ()
	{

	}

	LuckyCircleQuestList::~LuckyCircleQuestList()
	{

	}

	rapidjson::Document LuckyCircleQuestList::toJson()
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
		document.AddMember("ticketRequire", ticketRequire,locator);
rapidjson::Value veventDescription(eventDescription.c_str(), eventDescription.size());
		document.AddMember("eventDescription", veventDescription,locator);
rapidjson::Value vurlImg(urlImg.c_str(), urlImg.size());
		document.AddMember("urlImg", vurlImg,locator);
rapidjson::Value vurlRules(urlRules.c_str(), urlRules.size());
		document.AddMember("urlRules", vurlRules,locator);
		document.AddMember("screenIdToGetTickets", screenIdToGetTickets,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LuckyCircleQuestList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LuckyCircleQuestList::toData(rapidjson::Document & document)
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
		if(document.HasMember("ticketRequire"))
		{
			setTicketRequire(document["ticketRequire"].GetInt());
		}
		if(document.HasMember("eventDescription"))
		{
			setEventDescription(document["eventDescription"].GetString());
		}
		if(document.HasMember("urlImg"))
		{
			setUrlImg(document["urlImg"].GetString());
		}
		if(document.HasMember("urlRules"))
		{
			setUrlRules(document["urlRules"].GetString());
		}
		if(document.HasMember("screenIdToGetTickets"))
		{
			setScreenIdToGetTickets(document["screenIdToGetTickets"].GetInt());
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
			LuckyCircleQuestListItem d;
			d.toData(json);
			list.push_back(d);

			}
		}


	}
}
