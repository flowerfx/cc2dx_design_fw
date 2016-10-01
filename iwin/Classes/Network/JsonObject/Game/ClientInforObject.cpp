#include "ClientInforObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	std::string ClientInforObject::GOWIN_BUNDLE_ID = "vn.me.gowin";
std::string ClientInforObject::IWIN_BUNDLE_ID = "vn.mecorp.iwin";

	ClientInforObject:: ClientInforObject ()
	{

	}

	ClientInforObject::~ClientInforObject()
	{

	}

	rapidjson::Document ClientInforObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vversion(version.c_str(), (unsigned int)version.size());
		document.AddMember("version", vversion,locator);
rapidjson::Value vphone(phone.c_str(), (unsigned int)phone.size());
		document.AddMember("phone", vphone,locator);
rapidjson::Value vlang(lang.c_str(), (unsigned int)lang.size());
		document.AddMember("lang", vlang,locator);
rapidjson::Value vos(os.c_str(), (unsigned int)os.size());
		document.AddMember("os", vos,locator);
rapidjson::Value vdevice(device.c_str(), (unsigned int)device.size());
		document.AddMember("device", vdevice,locator);
rapidjson::Value vosVersion(osVersion.c_str(), (unsigned int)osVersion.size());
		document.AddMember("osVersion", vosVersion,locator);
rapidjson::Value vcarrier(carrier.c_str(), (unsigned int)carrier.size());
		document.AddMember("carrier", vcarrier,locator);
rapidjson::Value vnet(net.c_str(), (unsigned int)net.size());
		document.AddMember("net", vnet,locator);
		document.AddMember("isJaibreak", isJaibreak,locator);
rapidjson::Value vnotifyKey(notifyKey.c_str(), (unsigned int)notifyKey.size());
		document.AddMember("notifyKey", vnotifyKey,locator);
rapidjson::Value vdeviceUDID(deviceUDID.c_str(), (unsigned int)deviceUDID.size());
		document.AddMember("deviceUDID", vdeviceUDID,locator);
rapidjson::Value vbundleId(bundleId.c_str(), (unsigned int)bundleId.size());
		document.AddMember("bundleId", vbundleId,locator);
		document.AddMember("width", width,locator);
		document.AddMember("height", height,locator);
		document.AddMember("isExistHackInApp", isExistHackInApp,locator);
		document.AddMember("isFirstLoginOk", isFirstLoginOk,locator);
rapidjson::Value vchannel(channel.c_str(), (unsigned int)channel.size());
		document.AddMember("channel", vchannel,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ClientInforObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ClientInforObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("version"))
		{
			setVersion(document["version"].GetString());
		}
		if(document.HasMember("phone"))
		{
			setPhone(document["phone"].GetString());
		}
		if(document.HasMember("lang"))
		{
			setLang(document["lang"].GetString());
		}
		if(document.HasMember("os"))
		{
			setOs(document["os"].GetString());
		}
		if(document.HasMember("device"))
		{
			setDevice(document["device"].GetString());
		}
		if(document.HasMember("osVersion"))
		{
			setOsVersion(document["osVersion"].GetString());
		}
		if(document.HasMember("carrier"))
		{
			setCarrier(document["carrier"].GetString());
		}
		if(document.HasMember("net"))
		{
			setNet(document["net"].GetString());
		}
		if(document.HasMember("isJaibreak"))
		{
			setIsJaibreak(document["isJaibreak"].GetInt());
		}
		if(document.HasMember("notifyKey"))
		{
			setNotifyKey(document["notifyKey"].GetString());
		}
		if(document.HasMember("deviceUDID"))
		{
			setDeviceUDID(document["deviceUDID"].GetString());
		}
		if(document.HasMember("bundleId"))
		{
			setBundleId(document["bundleId"].GetString());
		}
		if(document.HasMember("width"))
		{
			setWidth(document["width"].GetInt());
		}
		if(document.HasMember("height"))
		{
			setHeight(document["height"].GetInt());
		}
		if(document.HasMember("isExistHackInApp"))
		{
			setIsExistHackInApp(document["isExistHackInApp"].GetBool());
		}
		if(document.HasMember("isFirstLoginOk"))
		{
			setIsFirstLoginOk(document["isFirstLoginOk"].GetBool());
		}
		if(document.HasMember("channel"))
		{
			setChannel(document["channel"].GetString());
		}


	}
}
