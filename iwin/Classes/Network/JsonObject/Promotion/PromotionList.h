#ifndef _PromotionList_H_
#define _PromotionList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "PromotionItem.h"

namespace iwinmesage
{
	class PromotionList
	{
		
CC_SYNTHESIZE(std::vector<PromotionItem>, list, List);
CC_SYNTHESIZE(int, indexPage, IndexPage);
CC_SYNTHESIZE(int, totalPages, TotalPages);

	public:
		
		PromotionList();
		virtual ~PromotionList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
