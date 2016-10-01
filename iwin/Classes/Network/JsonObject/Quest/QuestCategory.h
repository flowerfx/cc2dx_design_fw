#ifndef _QuestCategory_H_
#define _QuestCategory_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
namespace iwinmesage
{
	class QuestCategory
	{
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, imgUrl, ImgUrl);
CC_SYNTHESIZE(int, itemNo, ItemNo);

	public:
		
		QuestCategory();
		virtual ~QuestCategory();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
