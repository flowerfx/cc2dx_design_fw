#ifndef _RegisterInfor_H_
#define _RegisterInfor_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class RegisterInfor
	{
		
CC_SYNTHESIZE(std::string, infor, Infor);
CC_SYNTHESIZE(bool, registerStatus, RegisterStatus);

	public:
		static  int USER_REG_SUCCESS;
static  int USER_REG_FAIL;
static  int USER_REG_EXIST;
static  int USER_REG_SMS_SUCCESS;
static  int USER_REG_SMS_FAIL;
static  int USER_REG_SMS_EXIST;

		RegisterInfor();
		virtual ~RegisterInfor();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
