#include "ShopId.h"
#include "json/writer.h"
namespace iwinmesage
{
	int ShopId::TRONG_SAT = 1;
int ShopId::BUA = 2;
int ShopId::COMBO_TRONG_BUA = 3;
int ShopId::NAP_WIN_TANG_TRONG_SAT = 4;

	ShopId:: ShopId ()
	{

	}

	ShopId::~ShopId()
	{

	}

	rapidjson::Document ShopId::toJson()
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
	void ShopId::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ShopId::toData(rapidjson::Document & document)
	{
		

	}
}
