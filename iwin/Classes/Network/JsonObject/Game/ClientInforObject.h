#ifndef _ClientInforObject_H_
#define _ClientInforObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ClientInforObject
	{
		
CC_SYNTHESIZE(std::string, version, Version);
CC_SYNTHESIZE(std::string, phone, Phone);
CC_SYNTHESIZE(std::string, lang, Lang);
CC_SYNTHESIZE(std::string, os, Os);
CC_SYNTHESIZE(std::string, device, Device);
CC_SYNTHESIZE(std::string, osVersion, OsVersion);
CC_SYNTHESIZE(std::string, carrier, Carrier);
CC_SYNTHESIZE(std::string, net, Net);
CC_SYNTHESIZE(int, isJaibreak, IsJaibreak);
CC_SYNTHESIZE(std::string, notifyKey, NotifyKey);
CC_SYNTHESIZE(std::string, deviceUDID, DeviceUDID);
CC_SYNTHESIZE(std::string, bundleId, BundleId);
CC_SYNTHESIZE(int, width, Width);
CC_SYNTHESIZE(int, height, Height);
CC_SYNTHESIZE(bool, isExistHackInApp, IsExistHackInApp);
CC_SYNTHESIZE(bool, isFirstLoginOk, IsFirstLoginOk);
CC_SYNTHESIZE(std::string, channel, Channel);

	public:
		static  std::string GOWIN_BUNDLE_ID;
static  std::string IWIN_BUNDLE_ID;

		ClientInforObject();
		virtual ~ClientInforObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
