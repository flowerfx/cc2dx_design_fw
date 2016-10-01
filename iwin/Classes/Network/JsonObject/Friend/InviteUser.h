#ifndef _InviteUser_H_
#define _InviteUser_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class InviteUser
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(int, avatar, Avatar);
CC_SYNTHESIZE(int64_t, money, Money);
CC_SYNTHESIZE(bool, selected, Selected);

	public:
		
		InviteUser();
		virtual ~InviteUser();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
