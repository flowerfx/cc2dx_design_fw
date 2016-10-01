#ifndef _QuestCategoryList_H_
#define _QuestCategoryList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "QuestCategory.h"

namespace iwinmesage
{
	class QuestCategoryList
	{
		
CC_SYNTHESIZE(std::vector<QuestCategory>, categoryList, CategoryList);
CC_SYNTHESIZE(int, currentCategory, CurrentCategory);

	public:
		
		QuestCategoryList();
		virtual ~QuestCategoryList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
