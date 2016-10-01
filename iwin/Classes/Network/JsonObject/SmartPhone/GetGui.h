#ifndef _GetGui_H_
#define _GetGui_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GetGui
	{
		
CC_SYNTHESIZE(std::string, guiVersion, GuiVersion);
CC_SYNTHESIZE(std::string, guiLink, GuiLink);

	public:
		
		GetGui();
		virtual ~GetGui();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
