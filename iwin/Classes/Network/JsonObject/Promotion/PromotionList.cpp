#include "PromotionList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	PromotionList:: PromotionList ()
	{

	}

	PromotionList::~PromotionList()
	{

	}

	rapidjson::Document PromotionList::toJson()
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
		document.AddMember("indexPage", indexPage,locator);
		document.AddMember("totalPages", totalPages,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void PromotionList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void PromotionList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("indexPage"))
		{
			setIndexPage(document["indexPage"].GetInt());
		}
		if(document.HasMember("totalPages"))
		{
			setTotalPages(document["totalPages"].GetInt());
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
			PromotionItem d;
			d.toData(json);
			list.push_back(d);

			}
		}


	}
}
