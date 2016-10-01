#include "LobbyList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	LobbyList:: LobbyList ()
	{

	}

	LobbyList::~LobbyList()
	{
		for (auto l : list)
		{
			delete l;
		}
		list.clear();
	}

	rapidjson::Document LobbyList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		document.AddMember("moneyType", moneyType, locator);
		rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < list.size(); i++)
		{
			arrlist.PushBack(list.at(i)->toJson(), locator);
		}
		document.AddMember("list", arrlist, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void LobbyList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void LobbyList::toData(rapidjson::Document & document)
	{
		if(document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("list") && !document["list"].IsNull() && document["list"].IsArray())
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
				LobbyItem* l = new LobbyItem();
				l->toData(json);
				list.push_back(l);

			}
		}

	}

	LobbyItem* LobbyList::getLobbyItem(int idx)
	{
		return list[idx];
	}
}
