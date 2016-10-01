#ifndef __TaiXiuUserOrderList_H__
#define __TaiXiuUserOrderList_H__

#include <vector>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuUserOrder;
	class TaiXiuUserOrderList
	{
	public:
		/**
		 * list danh sách leaderboard Tài xỉu
		 */
		CC_SYNTHESIZE(std::vector<TaiXiuUserOrder*>, userOrderList, UserOrderList);

		TaiXiuUserOrderList();
		virtual ~TaiXiuUserOrderList();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif // __TaiXiuUserOrderList_H__
