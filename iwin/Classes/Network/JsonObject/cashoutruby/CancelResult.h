#ifndef _CancelResult_H_
#define _CancelResult_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CancelResult
	{
		
CC_SYNTHESIZE(int, code, Code);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		CancelResult();
		virtual ~CancelResult();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
