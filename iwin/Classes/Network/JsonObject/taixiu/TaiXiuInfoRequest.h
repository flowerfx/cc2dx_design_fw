#ifndef __TaiXiuInfoRequest_H__
#define __TaiXiuInfoRequest_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuInfoRequest
	{
	public:
		CC_SYNTHESIZE(std::string, userName, UserName);

		TaiXiuInfoRequest();
		TaiXiuInfoRequest(std::string name);
		virtual ~TaiXiuInfoRequest();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}

#endif //__TaiXiuInfoRequest_H__
