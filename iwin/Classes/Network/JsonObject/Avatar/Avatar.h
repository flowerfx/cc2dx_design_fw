#ifndef _Avatar_H_
#define _Avatar_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Avatar
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(int, cateId, CateId);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(int, expireDay, ExpireDay);
CC_SYNTHESIZE(int, bought, Bought);
CC_SYNTHESIZE(bool, bestSeller, BestSeller);
CC_SYNTHESIZE(std::string, img, Img);
CC_SYNTHESIZE(int64_t, money, Money);

	public:
		
		Avatar();
		Avatar(Avatar * ava);
		virtual ~Avatar();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
