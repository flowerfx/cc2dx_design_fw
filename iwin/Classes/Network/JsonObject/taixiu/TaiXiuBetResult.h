#ifndef __TaiXiu_Bet_Result_H_
#define __TaiXiu_Bet_Result_H_
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TaiXiuBetResult
	{
		/**
		 * thành công: 1, 0: thất bại
		 * */
		CC_SYNTHESIZE(unsigned int, code, Code);
		/**
		 * mô tả kết quả (thành công, không đủ tiền, …)
		 * */
		CC_SYNTHESIZE(std::string, message, Message);
		/**
		 * cửa đặt (Tài 0, Xỉu 1)
		 * */
		CC_SYNTHESIZE(unsigned int, betChoice, BetChoice);
		/**
		 * số win user đã đặt thêm vào cửa đã chọn
		 * */
		CC_SYNTHESIZE(s64, betWin, BetWin);
		/**
		 * tổng số win user đã đặt vào cửa đã chọn
		 * */
		CC_SYNTHESIZE(s64, userBetWin, UserBetWin);
		/**
		 * tổng số win đã đặt vào cửa đã chọn (trên toàn hệ thống)
		 * */
		CC_SYNTHESIZE(s64, totalBetWin, TotalBetWin);
		/**
		 * tổng số user đã đặt vào cửa đã chọn (trên toàn hệ thống)
		 * */
		CC_SYNTHESIZE(s64, totalUserBet, TotalUserBet);
		/**
		 * số win đặt cược tối thiểu
		 * */
		CC_SYNTHESIZE(s64, minBet, MinBet);
		/**
		 * số win đặt cược tối đa
		 * */
		CC_SYNTHESIZE(s64, maxBet, MaxBet);
	public:
		TaiXiuBetResult();
		virtual ~TaiXiuBetResult();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);

	};
}
#endif //__TaiXiu_Bet_Result_H_
