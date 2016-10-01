#include "FacebookFriendList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookFriendList:: FacebookFriendList ()
	{

	}

	FacebookFriendList::~FacebookFriendList()
	{

	}

	rapidjson::Document FacebookFriendList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrdata(rapidjson::kArrayType);
		for (size_t i = 0; i < data.size(); i++)
		{
			arrdata.PushBack(data.at(i).toJson(), locator);
		}
		document.AddMember("data",arrdata, locator);
		document.AddMember("paging", paging.toJson(),locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookFriendList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookFriendList::toData(rapidjson::Document & document)
	{
					if(document.HasMember("paging"))
			{
				const rapidjson::Value& cpaging = document["paging"];
				rapidjson::StringBuffer bufferpaging;
				bufferpaging.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerpaging(bufferpaging);
				cpaging.Accept(writerpaging);
				const char* jsonpaging = bufferpaging.GetString();
				FacebookFriendListPage dpaging;
				dpaging.toData(jsonpaging);
				setPaging(dpaging);
			}
		if(document.HasMember("data") && !document["data"].IsNull() && document["data"].IsArray() )
		{
			const rapidjson::Value& arrdata = document["data"];
			for (rapidjson::SizeType i = 0; i < arrdata.Size(); i++)
			{
							const rapidjson::Value& c = arrdata[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			FacebookFriend d;
			d.toData(json);
			data.push_back(d);

			}
		}


	}
}
