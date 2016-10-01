#include "StockCommand.h"
#include "json/writer.h"
namespace iwinmesage
{
	std::string StockCommand::GET_STOCK_LIST = "GET_STOCK_LIST";
std::string StockCommand::BUY_STOCK = "BUY_STOCK";
std::string StockCommand::GET_MY_STOCK_LIST = "GET_MY_STOCK_LIST";
std::string StockCommand::GET_DIVIDEND = "GET_DIVIDEND";

	StockCommand:: StockCommand ()
	{

	}

	StockCommand::~StockCommand()
	{

	}

	rapidjson::Document StockCommand::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void StockCommand::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void StockCommand::toData(rapidjson::Document & document)
	{
		

	}
}
