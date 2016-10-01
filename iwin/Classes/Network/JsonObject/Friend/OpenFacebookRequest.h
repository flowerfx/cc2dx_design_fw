#ifndef _OpenFacebookRequest_H_
#define _OpenFacebookRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class OpenFacebookRequest
	{
		
CC_SYNTHESIZE(std::vector<int64_t>, ids, Ids);
CC_SYNTHESIZE(std::string, fbId, FbId);

	public:
		
		OpenFacebookRequest();
		virtual ~OpenFacebookRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
