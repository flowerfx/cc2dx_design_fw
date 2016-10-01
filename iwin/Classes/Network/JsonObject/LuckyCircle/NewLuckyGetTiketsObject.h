#ifndef _NewLuckyGetTiketsObject_H_
#define _NewLuckyGetTiketsObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NewLuckyGetTiketsObject
	{
		
CC_SYNTHESIZE(int, screenId, ScreenId);
CC_SYNTHESIZE(std::string, message, Message);

	public:
		
		NewLuckyGetTiketsObject();
		virtual ~NewLuckyGetTiketsObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
