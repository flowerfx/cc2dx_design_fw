#include "MoboMap.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	MoboMap:: MoboMap ()
	{

	}

	MoboMap::~MoboMap()
	{

	}

	rapidjson::Document MoboMap::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arruserList(rapidjson::kArrayType);
		for (size_t i = 0; i < userList.size(); i++)
		{
			arruserList.PushBack(userList.at(i).toJson(), locator);
		}
		document.AddMember("userList",arruserList, locator);
rapidjson::Value vphone(phone.c_str(), phone.size());
		document.AddMember("phone", vphone,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void MoboMap::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void MoboMap::toData(rapidjson::Document & document)
	{
				if(document.HasMember("phone"))
		{
			setPhone(document["phone"].GetString());
		}
		if(document.HasMember("userList") && !document["userList"].IsNull() && document["userList"].IsArray() )
		{
			const rapidjson::Value& arruserList = document["userList"];
			for (rapidjson::SizeType i = 0; i < arruserList.Size(); i++)
			{
							const rapidjson::Value& c = arruserList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			IwinUser d;
			d.toData(json);
			userList.push_back(d);

			}
		}


	}
}
