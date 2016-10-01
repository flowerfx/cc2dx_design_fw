#ifndef _RequestFeature_H_
#define _RequestFeature_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "ButtonFeature.h"

namespace iwinmesage
{
	class RequestFeature
	{
		
CC_SYNTHESIZE(std::string, message, Message);
CC_SYNTHESIZE(std::string, buttonOk, ButtonOk);
CC_SYNTHESIZE(std::string, buttonCancel, ButtonCancel);
CC_SYNTHESIZE(int, featureId, FeatureId);
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(std::vector<ButtonFeature>, listButton, ListButton);
CC_SYNTHESIZE(int, type, Type);

	public:
		
		RequestFeature();
		virtual ~RequestFeature();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
