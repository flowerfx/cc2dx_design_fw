#ifndef _ServerControlDialogObject_H_
#define _ServerControlDialogObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "ServerControlButtonObject.h"

namespace iwinmesage
{
	class ServerControlDialogObject
	{
		
CC_SYNTHESIZE(std::string, ti, Ti);
CC_SYNTHESIZE(std::vector<ServerControlButtonObject>, ctls, Ctls);
CC_SYNTHESIZE(std::string, imgDes, ImgDes);
CC_SYNTHESIZE(std::string, bgName, BgName);

	public:
		
		ServerControlDialogObject();
		virtual ~ServerControlDialogObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
