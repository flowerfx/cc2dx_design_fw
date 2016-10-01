#include "Avatar.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Avatar::Avatar ()
	{

	}

	Avatar::Avatar(Avatar * ava)
	{
		this->id = ava->id;
		this->bestSeller = ava->bestSeller;
		this->bought = ava->bought;
		this->cateId = ava->cateId;
		this->expireDay = ava->expireDay;
		this->img = ava->img;
		this->money = ava->money;
		this->name = ava->name;
	}

	Avatar::~Avatar()
	{
		this->id = 0;
		this->bestSeller = false;
		this->bought = 0;
		this->cateId = 0;
		this->expireDay = 0;
		this->img = "";
		this->money = 0;
		this->name = "";
	}

	rapidjson::Document Avatar::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("cateId", cateId,locator);
rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname,locator);
		document.AddMember("expireDay", expireDay,locator);
		document.AddMember("bought", bought,locator);
		document.AddMember("bestSeller", bestSeller,locator);
rapidjson::Value vimg(img.c_str(), img.size());
		document.AddMember("img", vimg,locator);
		document.AddMember("money", money,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Avatar::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Avatar::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if(document.HasMember("cateId"))
		{
			setCateId(document["cateId"].GetInt());
		}
		if(document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if(document.HasMember("expireDay"))
		{
			setExpireDay(document["expireDay"].GetInt());
		}
		if(document.HasMember("bought"))
		{
			setBought(document["bought"].GetInt());
		}
		if(document.HasMember("bestSeller"))
		{
			setBestSeller(document["bestSeller"].GetBool());
		}
		if(document.HasMember("img"))
		{
			setImg(document["img"].GetString());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetInt64());
		}


	}
}
