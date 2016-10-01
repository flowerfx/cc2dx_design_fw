#ifndef __TaiXiu_GameInfo_H__
#define __TaiXiu_GameInfo_H__
#include <string>
#include <vector>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TaiXiuGameInfo {
		/**
		 * thời gian (ms) đặt cược còn lại
		 * */
		CC_SYNTHESIZE(s64, timeBetRemain, TimeBetRemain);
		/**
		 * thời gian (ms) xem kết quả còn lại
		 * */
		CC_SYNTHESIZE(s64, timeShowResultRemain, TimeShowResultRemain);
		/**
		 * tổng số user đặt cược vào cửa Tài
		 * */
		CC_SYNTHESIZE(int, totalUserTai, TotalUserTai);
		/**
		 * tổng số user đặt cược vào cửa Xỉu
		 * */
		CC_SYNTHESIZE(int, totalUserXiu, TotalUserXiu);
		/**
		 * tổng số win đặt cửa Tài
		 * */
		CC_SYNTHESIZE(s64, totalWinTai, TotalWinTai);
		/**
		 * tổng số win đặt cửa Xỉu
		 * */
		CC_SYNTHESIZE(s64, totalWinXiu, TotalWinXiu);
		/**
		 * số win của user gửi request đã đặt cửa Tài
		 * */
		CC_SYNTHESIZE(s64, winTai, WinTai);
		/**
		 * số win của user gửi request đã đặt cửa Xỉu
		 * */
		CC_SYNTHESIZE(s64, winXiu, WinXiu);
		/**
		 * số nút của 3 xúc xắc
		 * */
		CC_SYNTHESIZE(unsigned int, dice1, Dice1);
		CC_SYNTHESIZE(unsigned int, dice2, Dice2);
		CC_SYNTHESIZE(unsigned int, dice3, Dice3);
		/**
		 * kết quả tài/xỉu (0,1)
		 * */
		CC_SYNTHESIZE(unsigned int, result, Result);
		/**
		 * số win user được nhận
		 * */
		CC_SYNTHESIZE(s64, winReceived, WinReceived);
		/**
		 * số win đặt cược tối thiểu
		 * */
		CC_SYNTHESIZE(s64, minBet, MinBet);
		/**
		 * số win đặt cược tối đa
		 * */
		CC_SYNTHESIZE(s64, maxBet, MaxBet);
		/**
		 * Mảng chứa lịch sử kết quả
		 * */
		CC_SYNTHESIZE(std::vector<unsigned int>, historyList, HistoryList);
		/**
		 * tổng thời gian đặt cược
		 * */
		CC_SYNTHESIZE(int, timeBetTotal, TimeBetTotal);
		/**
		 * tổng thời gian show kết quả
		 * */
		CC_SYNTHESIZE(int, timeShowResultTotal, TimeShowResultTotal);
	public:
		TaiXiuGameInfo();
		virtual ~TaiXiuGameInfo();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif // __TaiXiu_GameInfo_H__
