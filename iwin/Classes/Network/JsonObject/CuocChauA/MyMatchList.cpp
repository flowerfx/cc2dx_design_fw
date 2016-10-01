#include "MyMatchList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MyMatchList:: MyMatchList ()
	{

	}

	MyMatchList::~MyMatchList()
	{

	}

	rapidjson::Document MyMatchList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrmatchList(rapidjson::kArrayType);
		for (size_t i = 0; i < matchList.size(); i++)
		{
			arrmatchList.PushBack(matchList.at(i).toJson(), locator);
		}
		document.AddMember("matchList",arrmatchList, locator);
		document.AddMember("indexPage", indexPage,locator);
		document.AddMember("totalPages", totalPages,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MyMatchList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MyMatchList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("indexPage"))
		{
			setIndexPage(document["indexPage"].GetInt());
		}
		if(document.HasMember("totalPages"))
		{
			setTotalPages(document["totalPages"].GetInt());
		}
		if(document.HasMember("matchList") && !document["matchList"].IsNull() && document["matchList"].IsArray() )
		{
			const rapidjson::Value& arrmatchList = document["matchList"];
			for (rapidjson::SizeType i = 0; i < arrmatchList.Size(); i++)
			{
							const rapidjson::Value& c = arrmatchList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			MyMatch d;
			d.toData(json);
			matchList.push_back(d);

			}
		}


	}
}
