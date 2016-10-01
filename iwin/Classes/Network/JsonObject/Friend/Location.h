#ifndef _Location_H_
#define _Location_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Location
	{
		
CC_SYNTHESIZE(double, latitude, Latitude);
CC_SYNTHESIZE(double, longitude, Longitude);
CC_SYNTHESIZE(std::string, namelocation, Namelocation);

	public:
		
		Location();
		virtual ~Location();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
