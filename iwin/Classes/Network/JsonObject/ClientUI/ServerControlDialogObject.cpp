#include "ServerControlDialogObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ServerControlDialogObject:: ServerControlDialogObject ()
	{

	}

	ServerControlDialogObject::~ServerControlDialogObject()
	{

	}

	rapidjson::Document ServerControlDialogObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrctls(rapidjson::kArrayType);
		for (size_t i = 0; i < ctls.size(); i++)
		{
			arrctls.PushBack(ctls.at(i).toJson(), locator);
		}
		document.AddMember("ctls",arrctls, locator);
rapidjson::Value vti(ti.c_str(), ti.size());
		document.AddMember("ti", vti,locator);
rapidjson::Value vimgDes(imgDes.c_str(), imgDes.size());
		document.AddMember("imgDes", vimgDes,locator);
rapidjson::Value vbgName(bgName.c_str(), bgName.size());
		document.AddMember("bgName", vbgName,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ServerControlDialogObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ServerControlDialogObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("ti"))
		{
			setTi(document["ti"].GetString());
		}
		if(document.HasMember("imgDes"))
		{
			setImgDes(document["imgDes"].GetString());
		}
		if(document.HasMember("bgName"))
		{
			setBgName(document["bgName"].GetString());
		}
		if(document.HasMember("ctls") && !document["ctls"].IsNull() && document["ctls"].IsArray() )
		{
			const rapidjson::Value& arrctls = document["ctls"];
			for (rapidjson::SizeType i = 0; i < arrctls.Size(); i++)
			{
							const rapidjson::Value& c = arrctls[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			ServerControlButtonObject d;
			d.toData(json);
			ctls.push_back(d);

			}
		}


	}
}
