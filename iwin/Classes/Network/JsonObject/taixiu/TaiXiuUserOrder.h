#ifndef __TaiXiuUserOrder_H__
#define __TaiXiuUserOrder_H__	
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TaiXiuUserOrder
	{
	public:
		/**
		 * user name
		 * */
		CC_SYNTHESIZE(std::string, username, Username);
		/**
		 * thứ hạng
		 * */
		CC_SYNTHESIZE(unsigned int, order, Order);

		/**
		 * tổng số tiền thắng cược
		 * */
		CC_SYNTHESIZE(s64, win, Win);
	public:
		TaiXiuUserOrder();
		TaiXiuUserOrder(std::string name, ubyte ord , s64 w);
		virtual ~TaiXiuUserOrder();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif // __TaiXiuUserOrder_H__
