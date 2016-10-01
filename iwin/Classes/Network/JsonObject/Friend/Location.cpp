#include "Location.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Location:: Location ()
	{

	}

	Location::~Location()
	{

	}

	rapidjson::Document Location::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("latitude", latitude,locator);
		document.AddMember("longitude", longitude,locator);
rapidjson::Value vnamelocation(namelocation.c_str(), namelocation.size());
		document.AddMember("namelocation", vnamelocation,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Location::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Location::toData(rapidjson::Document & document)
	{
				if(document.HasMember("latitude"))
		{
			setLatitude(document["latitude"].GetDouble());
		}
		if(document.HasMember("longitude"))
		{
			setLongitude(document["longitude"].GetDouble());
		}
		if(document.HasMember("namelocation"))
		{
			setNamelocation(document["namelocation"].GetString());
		}


	}
}
