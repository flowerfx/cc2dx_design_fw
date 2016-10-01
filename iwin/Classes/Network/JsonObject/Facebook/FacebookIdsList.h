#ifndef _FacebookIdsList_H_
#define _FacebookIdsList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookIdsList
	{
		
CC_SYNTHESIZE(std::vector<std::string>, ids, Ids);
CC_SYNTHESIZE(std::string, myId, MyId);
CC_SYNTHESIZE(std::string, ac, Ac);

	public:
		
		FacebookIdsList();
		virtual ~FacebookIdsList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
