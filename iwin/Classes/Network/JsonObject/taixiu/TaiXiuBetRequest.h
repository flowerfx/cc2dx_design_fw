#ifndef __TaiXiu_Bet_Request_H__
#define __TaiXiu_Bet_Request_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TaiXiuBetRequest
	{
		/**
		 * userName
		 * */
		CC_SYNTHESIZE(std::string, username, Username);
		/**
		 * Số tiền đặt cược
		 * */
		CC_SYNTHESIZE(s64, betWin, BetWin);
		/**
		 * Lựa chọn đặt cược : tài/xỉu (0,1)
		 * */
		CC_SYNTHESIZE(unsigned int, betChoice, BetChoice);
	public:
		TaiXiuBetRequest();
		TaiXiuBetRequest(std::string name, s64 bet, unsigned int choice);
		virtual ~TaiXiuBetRequest();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif // __TaiXiu_Bet_Request_H__
