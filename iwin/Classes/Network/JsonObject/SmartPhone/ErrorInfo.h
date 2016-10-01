#ifndef _ErrorInfo_H_
#define _ErrorInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ErrorInfo
	{
		
CC_SYNTHESIZE(std::string, cmd, Cmd);
CC_SYNTHESIZE(std::string, desc, Desc);

	public:
		
		ErrorInfo();
		virtual ~ErrorInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
