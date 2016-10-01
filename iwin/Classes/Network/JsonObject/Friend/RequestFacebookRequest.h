#ifndef _RequestFacebookRequest_H_
#define _RequestFacebookRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RequestFacebookRequest
	{
		
CC_SYNTHESIZE(std::string, requestId, RequestId);
CC_SYNTHESIZE(std::string, accessToken, AccessToken);
CC_SYNTHESIZE(std::vector<std::string>, listFbIdRequest, ListFbIdRequest);
CC_SYNTHESIZE(int, type, Type);

	public:
		
		RequestFacebookRequest();
		virtual ~RequestFacebookRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
