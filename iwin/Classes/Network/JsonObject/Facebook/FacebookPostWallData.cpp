#include "FacebookPostWallData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	FacebookPostWallData:: FacebookPostWallData ()
	{

	}

	FacebookPostWallData::~FacebookPostWallData()
	{

	}

	rapidjson::Document FacebookPostWallData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vdisplay(display.c_str(), display.size());
		document.AddMember("display", vdisplay,locator);
rapidjson::Value vcaption(caption.c_str(), caption.size());
		document.AddMember("caption", vcaption,locator);
rapidjson::Value vlink(link.c_str(), link.size());
		document.AddMember("link", vlink,locator);
rapidjson::Value vredirect_uri(redirect_uri.c_str(), redirect_uri.size());
		document.AddMember("redirect_uri", vredirect_uri,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void FacebookPostWallData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void FacebookPostWallData::toData(rapidjson::Document & document)
	{
				if(document.HasMember("display"))
		{
			setDisplay(document["display"].GetString());
		}
		if(document.HasMember("caption"))
		{
			setCaption(document["caption"].GetString());
		}
		if(document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}
		if(document.HasMember("redirect_uri"))
		{
			setRedirect_uri(document["redirect_uri"].GetString());
		}


	}
}
