#ifndef _FacebookRequestResponse_H_
#define _FacebookRequestResponse_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookRequestResponse
	{
		
CC_SYNTHESIZE(std::string, request, Request);
CC_SYNTHESIZE(std::vector<std::string>, to, To);

	public:
		
		FacebookRequestResponse();
		virtual ~FacebookRequestResponse();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
