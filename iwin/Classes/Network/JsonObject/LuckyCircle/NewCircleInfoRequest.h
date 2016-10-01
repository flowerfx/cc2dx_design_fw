#ifndef _NewCircleInfoRequest_H_
#define _NewCircleInfoRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NewCircleInfoRequest
	{
		
CC_SYNTHESIZE(int, circleId, CircleId);
CC_SYNTHESIZE(int, screenType, ScreenType);

	public:
		
		NewCircleInfoRequest();
		virtual ~NewCircleInfoRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
