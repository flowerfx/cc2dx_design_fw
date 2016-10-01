#include "RegisterInfor.h"
#include "json/writer.h"
namespace iwinmesage
{
	int RegisterInfor::USER_REG_SUCCESS = 100500;
int RegisterInfor::USER_REG_FAIL = 100501;
int RegisterInfor::USER_REG_EXIST = 100502;
int RegisterInfor::USER_REG_SMS_SUCCESS = 100603;
int RegisterInfor::USER_REG_SMS_FAIL = 100604;
int RegisterInfor::USER_REG_SMS_EXIST = 100605;

	RegisterInfor:: RegisterInfor ()
	{

	}

	RegisterInfor::~RegisterInfor()
	{

	}

	rapidjson::Document RegisterInfor::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vinfor(infor.c_str(), infor.size());
		document.AddMember("infor", vinfor,locator);
		document.AddMember("registerStatus", registerStatus,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RegisterInfor::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RegisterInfor::toData(rapidjson::Document & document)
	{
				if(document.HasMember("infor"))
		{
			setInfor(document["infor"].GetString());
		}
		if(document.HasMember("registerStatus"))
		{
			setRegisterStatus(document["registerStatus"].GetBool());
		}


	}
}
