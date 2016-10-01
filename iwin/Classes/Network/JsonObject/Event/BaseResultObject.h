#ifndef _BaseResultObject_H_
#define _BaseResultObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class BaseResultObject
	{
		
CC_SYNTHESIZE(std::string, message, Message);
CC_SYNTHESIZE(bool, isSuccess, IsSuccess);

	public:
		
		BaseResultObject();
		virtual ~BaseResultObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
