#include "GetOnwerBoardListResponse.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetOnwerBoardListResponse:: GetOnwerBoardListResponse ()
	{

	}

	GetOnwerBoardListResponse::~GetOnwerBoardListResponse()
	{

	}

	rapidjson::Document GetOnwerBoardListResponse::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrboardOwners(rapidjson::kArrayType);
		for (size_t i = 0; i < boardOwners.size(); i++)
		{
			arrboardOwners.PushBack(boardOwners.at(i).toJson(), locator);
		}
		document.AddMember("boardOwners",arrboardOwners, locator);
		document.AddMember("win", win,locator);
		document.AddMember("lose", lose,locator);
		document.AddMember("draw", draw,locator);
		document.AddMember("leave", leave,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetOnwerBoardListResponse::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetOnwerBoardListResponse::toData(rapidjson::Document & document)
	{
				if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt());
		}
		if(document.HasMember("lose"))
		{
			setLose(document["lose"].GetInt());
		}
		if(document.HasMember("draw"))
		{
			setDraw(document["draw"].GetInt());
		}
		if(document.HasMember("leave"))
		{
			setLeave(document["leave"].GetInt());
		}
		if(document.HasMember("boardOwners") && !document["boardOwners"].IsNull() && document["boardOwners"].IsArray() )
		{
			const rapidjson::Value& arrboardOwners = document["boardOwners"];
			for (rapidjson::SizeType i = 0; i < arrboardOwners.Size(); i++)
			{
							const rapidjson::Value& c = arrboardOwners[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			BoardOwner d;
			d.toData(json);
			boardOwners.push_back(d);

			}
		}


	}
}
