#include "GetGui.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GetGui:: GetGui ()
	{

	}

	GetGui::~GetGui()
	{

	}

	rapidjson::Document GetGui::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vguiVersion(guiVersion.c_str(), guiVersion.size());
		document.AddMember("guiVersion", vguiVersion,locator);
rapidjson::Value vguiLink(guiLink.c_str(), guiLink.size());
		document.AddMember("guiLink", vguiLink,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GetGui::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GetGui::toData(rapidjson::Document & document)
	{
				if(document.HasMember("guiVersion"))
		{
			setGuiVersion(document["guiVersion"].GetString());
		}
		if(document.HasMember("guiLink"))
		{
			setGuiLink(document["guiLink"].GetString());
		}


	}
}
