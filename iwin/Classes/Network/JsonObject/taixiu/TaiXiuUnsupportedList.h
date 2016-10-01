#ifndef __TaiXiuUnsupportedList_H__
#define __TaiXiuUnsupportedList_H__
#include <string>
#include <vector>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuUnsupportedList
	{
	public:
		//Game không cho mở Tài xỉu
		CC_SYNTHESIZE(std::vector<int>, gameList, GameList);
		//Screen không cho mở Tài xỉu
		CC_SYNTHESIZE(std::vector<int>, screenList, ScreenList);

		/**
		 * trang thai mo tai xiu win
		 * */
		CC_SYNTHESIZE(bool, isWinEnabled, IsWinEnabled);
		/**
		 * trang thai mo tai xiu ruby
		 * */
		CC_SYNTHESIZE(bool, isRubyEnabled, IsRubyEnabled);
	public:
		TaiXiuUnsupportedList();
		virtual ~TaiXiuUnsupportedList();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TaiXiuUnsupportedList_H__
