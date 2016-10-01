#ifndef _EventAward_H_
#define _EventAward_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class EventAward
	{
		
CC_SYNTHESIZE(int, type, Type);
CC_SYNTHESIZE(std::string, value, Value);
CC_SYNTHESIZE(std::string, des, Des);

	public:
		static  int WIN;
static  int LUCKY_CIRCLE_POINT;
static  int FIRE_CRACKER_POINT;
static  int GREETING;
static  int IPHONE6;
static  int ZENPHONE5;

		EventAward();
		virtual ~EventAward();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
