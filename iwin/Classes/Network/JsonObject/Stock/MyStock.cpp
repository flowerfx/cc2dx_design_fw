#include "MyStock.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MyStock:: MyStock ()
	{

	}

	MyStock::~MyStock()
	{

	}

	rapidjson::Document MyStock::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("id", id,locator);
		document.AddMember("stock", stock.toJson(),locator);
		document.AddMember("numberOfStock", numberOfStock,locator);
		document.AddMember("stockIndex", stockIndex,locator);
		document.AddMember("time", time,locator);
		document.AddMember("price", price,locator);
		document.AddMember("dividend", dividend,locator);
		document.AddMember("canGetDividend", canGetDividend,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MyStock::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MyStock::toData(rapidjson::Document & document)
	{
				if(document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
			if(document.HasMember("stock"))
			{
				const rapidjson::Value& cstock = document["stock"];
				rapidjson::StringBuffer bufferstock;
				bufferstock.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerstock(bufferstock);
				cstock.Accept(writerstock);
				const char* jsonstock = bufferstock.GetString();
				Stock dstock;
				dstock.toData(jsonstock);
				setStock(dstock);
			}
		if(document.HasMember("numberOfStock"))
		{
			setNumberOfStock(document["numberOfStock"].GetInt());
		}
		if(document.HasMember("stockIndex"))
		{
			setStockIndex(document["stockIndex"].GetDouble());
		}
		if(document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if(document.HasMember("price"))
		{
			setPrice(document["price"].GetInt64());
		}
		if(document.HasMember("dividend"))
		{
			setDividend(document["dividend"].GetInt64());
		}
		if(document.HasMember("canGetDividend"))
		{
			setCanGetDividend(document["canGetDividend"].GetBool());
		}


	}
}
