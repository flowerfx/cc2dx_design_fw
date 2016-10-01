#ifndef _OptionItem_H_
#define _OptionItem_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class OptionItem
	{
		
CC_SYNTHESIZE(int, id, Id);
CC_SYNTHESIZE(std::string, text, Text);
CC_SYNTHESIZE(bool, isSelected, IsSelected);

	public:
		
		OptionItem();
		virtual ~OptionItem();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
