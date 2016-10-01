#include "UserRankingList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	UserRankingList:: UserRankingList ()
	{

	}

	UserRankingList::~UserRankingList()
	{

	}

	rapidjson::Document UserRankingList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrrankingList(rapidjson::kArrayType);
		for (size_t i = 0; i < rankingList.size(); i++)
		{
			arrrankingList.PushBack(rankingList.at(i).toJson(), locator);
		}
		document.AddMember("rankingList",arrrankingList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void UserRankingList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void UserRankingList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("rankingList") && !document["rankingList"].IsNull() && document["rankingList"].IsArray() )
		{
			const rapidjson::Value& arrrankingList = document["rankingList"];
			for (rapidjson::SizeType i = 0; i < arrrankingList.Size(); i++)
			{
							const rapidjson::Value& c = arrrankingList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			UserRanking d;
			d.toData(json);
			rankingList.push_back(d);

			}
		}


	}
}
