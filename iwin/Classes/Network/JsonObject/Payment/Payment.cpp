#include "Payment.h"
#include "json/writer.h"
namespace iwinmesage
{
	int Payment::CHARGE_BY_ID_SMS = 0;
int Payment::CHARGE_BY_ID_1_PIN = 1;
int Payment::CHARGE_BY_ID_2_PIN = 2;
int Payment::CHARGE_BY_ID_BANKING = 4;
int Payment::CHARGE_BY_ID_IAP = 5;
int Payment::CHARGE_BY_ID_GOOGLE = 9;
int Payment::CHARGE_BY_SUBID_MOBIFONE = 3;
int Payment::CHARGE_BY_SUBID_VINAPHONE = 8;
int Payment::CHARGE_BY_SUBID_VIETTEL = 5;
int Payment::CHARGE_BY_SUBID_GATE = 4;
int Payment::SHOP_ID_DANGTRONG = 1;

	Payment:: Payment ()
	{
		data = nullptr;
	}

	Payment::~Payment()
	{

	}

	rapidjson::Document Payment::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < list.size(); i++)
		{
			arrlist.PushBack(list.at(i)->toJson(), locator);
		}
		document.AddMember("list",arrlist, locator);
		rapidjson::Value arritems(rapidjson::kArrayType);
		for (size_t i = 0; i < items.size(); i++)
		{
			arritems.PushBack(items.at(i)->toJson(), locator);
		}
		document.AddMember("items",arritems, locator);
		rapidjson::Value arreventItems(rapidjson::kArrayType);
		for (size_t i = 0; i < eventItems.size(); i++)
		{
			arreventItems.PushBack(eventItems.at(i).toJson(), locator);
		}
		document.AddMember("eventItems",arreventItems, locator);
rapidjson::Value vtitle(title.c_str(), (unsigned int)title.size());
		document.AddMember("title", vtitle,locator);
rapidjson::Value vdesc(desc.c_str(), (unsigned int)desc.size());
		document.AddMember("desc", vdesc,locator);
rapidjson::Value vtype(type.c_str(), (unsigned int)type.size());
		document.AddMember("type", vtype,locator);
rapidjson::Value vcharge_type(charge_type.c_str(), (unsigned int)charge_type.size());
		document.AddMember("charge_type", vcharge_type,locator);
rapidjson::Value vicon(icon.c_str(), (unsigned int)icon.size());
		document.AddMember("icon", vicon,locator);
		if(data)
			document.AddMember("data", data->toJson(),locator);
		else
		{
			document.AddMember("data", rapidjson::Document(),locator);
		}
rapidjson::Value vmessage(message.c_str(), message.size());
		document.AddMember("message", vmessage,locator);
rapidjson::Value vdescription(description.c_str(), description.size());
		document.AddMember("description", vdescription,locator);
		document.AddMember("money", money,locator);
		document.AddMember("win", win,locator);
		document.AddMember("win_promotion", win_promotion,locator);
		document.AddMember("charge_by_id", charge_by_id,locator);
		document.AddMember("charge_by_subid", charge_by_subid,locator);
rapidjson::Value vcharge_id(charge_id.c_str(), charge_id.size());
		document.AddMember("charge_id", vcharge_id,locator);
rapidjson::Value vfield_1_name(field_1_name.c_str(), field_1_name.size());
		document.AddMember("field_1_name", vfield_1_name,locator);
rapidjson::Value vfield_2_name(field_2_name.c_str(), field_2_name.size());
		document.AddMember("field_2_name", vfield_2_name,locator);
rapidjson::Value vfield_1_desc(field_1_desc.c_str(), field_1_desc.size());
		document.AddMember("field_1_desc", vfield_1_desc,locator);
rapidjson::Value vfield_2_desc(field_2_desc.c_str(), field_2_desc.size());
		document.AddMember("field_2_desc", vfield_2_desc,locator);
rapidjson::Value vcontent(content.c_str(), content.size());
		document.AddMember("content", vcontent,locator);
rapidjson::Value vphone(phone.c_str(), phone.size());
		document.AddMember("phone", vphone,locator);
rapidjson::Value vconfirmSMS(confirmSMS.c_str(), confirmSMS.size());
		document.AddMember("confirmSMS", vconfirmSMS,locator);
rapidjson::Value vcode(code.c_str(), code.size());
		document.AddMember("code", vcode,locator);
rapidjson::Value vpayload(payload.c_str(), payload.size());
		document.AddMember("payload", vpayload,locator);
rapidjson::Value vlink(link.c_str(), link.size());
		document.AddMember("link", vlink,locator);
rapidjson::Value vunit(unit.c_str(), unit.size());
		document.AddMember("unit", vunit,locator);
		document.AddMember("shopId", shopId,locator);
		document.AddMember("isChooseItems", isChooseItems,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Payment::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Payment::toData(rapidjson::Document & document)
	{
				if(document.HasMember("title"))
		{
			setTitle(document["title"].GetString());
		}
		if(document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetString());
		}
		if(document.HasMember("charge_type"))
		{
			setCharge_type(document["charge_type"].GetString());
		}
		if(document.HasMember("icon"))
		{
			setIcon(document["icon"].GetString());
		}
			if(document.HasMember("data"))
			{
				const rapidjson::Value& cdata = document["data"];
				rapidjson::StringBuffer bufferdata;
				bufferdata.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writerdata(bufferdata);
				cdata.Accept(writerdata);
				const char* jsondata = bufferdata.GetString();
				Payment* ddata = new Payment();
				ddata->toData(jsondata);
				data = ddata;
				
			}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if(document.HasMember("description"))
		{
			setDescription(document["description"].GetString());
		}
		if(document.HasMember("money"))
		{
			setMoney(document["money"].GetDouble());
		}
		if(document.HasMember("win"))
		{
			setWin(document["win"].GetInt());
		}
		if(document.HasMember("win_promotion"))
		{
			setWin_promotion(document["win_promotion"].GetInt());
		}
		if(document.HasMember("charge_by_id"))
		{
			setCharge_by_id(document["charge_by_id"].GetInt());
		}
		if(document.HasMember("charge_by_subid"))
		{
			setCharge_by_subid(document["charge_by_subid"].GetInt());
		}
		if(document.HasMember("charge_id"))
		{
			setCharge_id(document["charge_id"].GetString());
		}
		if(document.HasMember("field_1_name"))
		{
			setField_1_name(document["field_1_name"].GetString());
		}
		if(document.HasMember("field_2_name"))
		{
			setField_2_name(document["field_2_name"].GetString());
		}
		if(document.HasMember("field_1_desc"))
		{
			setField_1_desc(document["field_1_desc"].GetString());
		}
		if(document.HasMember("field_2_desc"))
		{
			setField_2_desc(document["field_2_desc"].GetString());
		}
		if(document.HasMember("content"))
		{
			setContent(document["content"].GetString());
		}
		if(document.HasMember("phone"))
		{
			setPhone(document["phone"].GetString());
		}
		if(document.HasMember("confirmSMS"))
		{
			setConfirmSMS(document["confirmSMS"].GetString());
		}
		if(document.HasMember("code"))
		{
			setCode(document["code"].GetString());
		}
		if(document.HasMember("payload"))
		{
			setPayload(document["payload"].GetString());
		}
		if(document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}
		if (document.HasMember("unit"))
		{
			setUnit(document["unit"].GetString());
		}
		if (document.HasMember("helpLink"))
		{
			setHelpLink(document["helpLink"].GetString());
		}
		if(document.HasMember("shopId"))
		{
			setShopId(document["shopId"].GetInt());
		}
		if(document.HasMember("isChooseItems"))
		{
			setIsChooseItems(document["isChooseItems"].GetBool());
		}
		if(document.HasMember("list") && !document["list"].IsNull() && document["list"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["list"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
							const rapidjson::Value& c = arrlist[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Payment* d = new Payment();
			d->toData(json);
			list.push_back(d);

			}
		}
		if(document.HasMember("items") && !document["items"].IsNull() && document["items"].IsArray() )
		{
			const rapidjson::Value& arritems = document["items"];
			for (rapidjson::SizeType i = 0; i < arritems.Size(); i++)
			{
							const rapidjson::Value& c = arritems[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Payment* d = new Payment();
			d->toData(json);
			items.push_back(d);

			}
		}
		if(document.HasMember("eventItems") && !document["eventItems"].IsNull() && document["eventItems"].IsArray() )
		{
			const rapidjson::Value& arreventItems = document["eventItems"];
			for (rapidjson::SizeType i = 0; i < arreventItems.Size(); i++)
			{
							const rapidjson::Value& c = arreventItems[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			EventItem d;
			d.toData(json);
			eventItems.push_back(d);

			}
		}


	}
}
