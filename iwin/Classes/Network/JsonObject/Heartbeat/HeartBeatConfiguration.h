#ifndef _HeartBeatConfiguration_H_
#define _HeartBeatConfiguration_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class HeartBeatConfiguration
	{
		
CC_SYNTHESIZE(bool, on, On);
CC_SYNTHESIZE(int64_t, interval, Interval);
CC_SYNTHESIZE(int64_t, timeout, Timeout);

	public:
		static  int WEAK_TIME_IN_SECOND;
static  int SLOW_TIME_IN_SECOND;
static  int NORMAL_TIME_IN_SECOND;

		HeartBeatConfiguration();
		virtual ~HeartBeatConfiguration();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
