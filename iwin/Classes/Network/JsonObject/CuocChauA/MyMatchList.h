#ifndef _MyMatchList_H_
#define _MyMatchList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "MyMatch.h"

namespace iwinmesage
{
	class MyMatchList
	{
		
CC_SYNTHESIZE(std::vector<MyMatch>, matchList, MatchList);
CC_SYNTHESIZE(int, indexPage, IndexPage);
CC_SYNTHESIZE(int, totalPages, TotalPages);

	public:
		
		MyMatchList();
		virtual ~MyMatchList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
