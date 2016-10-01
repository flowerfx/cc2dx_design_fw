#ifndef _FacebookRequest_H_
#define _FacebookRequest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class FacebookRequest
	{
		
CC_SYNTHESIZE(int64_t, id, Id);
CC_SYNTHESIZE(std::string, requestId, RequestId);
CC_SYNTHESIZE(std::string, fromFbId, FromFbId);
CC_SYNTHESIZE(int, type, Type);
CC_SYNTHESIZE(time_t, date, Date);
CC_SYNTHESIZE(int, value, Value);

	public:
		static  int TYPE_REQUESTED_GIFT_FROM_FRIEND;
static  int TYPE_RECEIVED_GIFT_FROM_FRIEND;
static  int TYPE_REQUESTED_GIFT_FROM_FRIEND_APPROVED;
static  int TYPE_RECEIVED_GIFT_FROM_FRIEND_APPROVED;
static  int TYPE_OPEN_SUCCESS_GIFT;

		FacebookRequest();
		virtual ~FacebookRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
