#ifndef _ResponseFacebookRequest_H_
#define _ResponseFacebookRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "FacebookRequest.h"

namespace iwinmesage
{
	class ResponseFacebookRequest
	{
		
CC_SYNTHESIZE(std::vector<FacebookRequest>, facebookRequests, FacebookRequests);
CC_SYNTHESIZE(int, maxGiftOpen, MaxGiftOpen);
CC_SYNTHESIZE(std::string, description, Description);

	public:
		
		ResponseFacebookRequest();
		virtual ~ResponseFacebookRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
