#include "MoboData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MoboData:: MoboData ()
	{

	}

	MoboData::~MoboData()
	{

	}

	rapidjson::Document MoboData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vcode(code.c_str(), code.size());
		document.AddMember("code", vcode,locator);
rapidjson::Value vaccessToken(accessToken.c_str(), accessToken.size());
		document.AddMember("accessToken", vaccessToken,locator);
rapidjson::Value vusernameIwin(usernameIwin.c_str(), usernameIwin.size());
		document.AddMember("usernameIwin", vusernameIwin,locator);
		document.AddMember("moboMap", moboMap.toJson(),locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MoboData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MoboData::toData(rapidjson::Document & document)
	{
				if(document.HasMember("code"))
		{
			setCode(document["code"].GetString());
		}
		if(document.HasMember("accessToken"))
		{
			setAccessToken(document["accessToken"].GetString());
		}
		if(document.HasMember("usernameIwin"))
		{
			setUsernameIwin(document["usernameIwin"].GetString());
		}
			if(document.HasMember("moboMap"))
			{
				const rapidjson::Value& cmoboMap = document["moboMap"];
				rapidjson::StringBuffer buffermoboMap;
				buffermoboMap.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writermoboMap(buffermoboMap);
				cmoboMap.Accept(writermoboMap);
				const char* jsonmoboMap = buffermoboMap.GetString();
				MoboMap dmoboMap;
				dmoboMap.toData(jsonmoboMap);
				setMoboMap(dmoboMap);
			}


	}
}
