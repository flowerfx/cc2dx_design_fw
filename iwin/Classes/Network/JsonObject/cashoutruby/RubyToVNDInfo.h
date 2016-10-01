#ifndef _RubyToVNDInfo_H_
#define _RubyToVNDInfo_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "ChargeRate.h"

namespace iwinmesage
{
	class RubyToVNDInfo
	{
		
CC_SYNTHESIZE(int, minRubyLeft, MinRubyLeft);
CC_SYNTHESIZE(std::vector<ChargeRate>, rates, Rates);
CC_SYNTHESIZE(int, maxCardChange, MaxCardChange);

	public:
		
		RubyToVNDInfo();
		virtual ~RubyToVNDInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
