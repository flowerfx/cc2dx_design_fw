#ifndef __TaiXiuUserHistoryList_H__
#define __TaiXiuUserHistoryList_H__
#include <string>
#include <vector>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuUserHistory;
	class TaiXiuUserHistoryList
	{
	public:
		/**
		 * list danh sách user history Tài xỉu
		 */
		CC_SYNTHESIZE(std::vector<TaiXiuUserHistory*>, userHistoryList, UserHistoryList);

		TaiXiuUserHistoryList();
		virtual ~TaiXiuUserHistoryList();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);

	};
}

#endif // __TaiXiuUserHistoryList_H__
