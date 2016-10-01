#include "FacebookPicture.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookPicture:: FacebookPicture ()
	{

	}

	FacebookPicture::~FacebookPicture()
	{

	}

	rapidjson::Document FacebookPicture::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("data", data.toJson(),locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookPicture::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookPicture::toData(rapidjson::Document & document)
	{
					if(document.HasMember("data"))
			{
				const rapidjson::Value& cdata = document["data"];
				rapidjson::StringBuffer bufferdata;
				bufferdata.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerdata(bufferdata);
				cdata.Accept(writerdata);
				const char* jsondata = bufferdata.GetString();
				FacebookPictureData ddata;
				ddata.toData(jsondata);
				setData(ddata);
			}


	}
}
