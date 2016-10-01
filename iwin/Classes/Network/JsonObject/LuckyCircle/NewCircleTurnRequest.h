#ifndef _NewCircleTurnRequest_H_
#define _NewCircleTurnRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NewCircleTurnRequest
	{
		
CC_SYNTHESIZE(int, circleId, CircleId);

	public:
		
		NewCircleTurnRequest();
		virtual ~NewCircleTurnRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
