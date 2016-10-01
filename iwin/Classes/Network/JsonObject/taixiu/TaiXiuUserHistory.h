#ifndef __TaiXiuUserHistory_H__
#define __TaiXiuUserHistory_H__

#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TaiXiuUserHistory
	{
	public:

		/**
		 * id của ván chơi
		 */
		CC_SYNTHESIZE(s64, matchId, MatchId);
		/**
		 * thời gian đặt cược
		 */
		CC_SYNTHESIZE(s64, time, Time);
		/**
		 * cửa đặt
		 */
		CC_SYNTHESIZE(unsigned int, betChoice, BetChoice);
		/**
		 * kết quả
		 */
		CC_SYNTHESIZE(unsigned int, resultType, ResultType);
		/**
		 * số nút xúc xắc + tài/xỉu (vd: 1-3-1:Xỉu)
		 */
		CC_SYNTHESIZE(std::string, result, Result);
		/**
		 * số win đặt cược
		 */
		CC_SYNTHESIZE(s64, betWin, BetWin);
		/**
		 * số win trả ngược lại khi có vấn đề
		 */
		CC_SYNTHESIZE(s64, returnWin, ReturnWin);
		/**
		 * số win nhận được
		 */
		CC_SYNTHESIZE(s64, receivedWin, ReceivedWin);
	public:
		TaiXiuUserHistory();
		virtual ~TaiXiuUserHistory();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TaiXiuUserHistory_H__
