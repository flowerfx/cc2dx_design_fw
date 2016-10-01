#ifndef _ButtonFeature_H_
#define _ButtonFeature_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ButtonFeature
	{
		
CC_SYNTHESIZE(std::string, text, Text);
CC_SYNTHESIZE(int, featureId, FeatureId);
CC_SYNTHESIZE(int, gameId, GameId);

	public:
		
		ButtonFeature();
		virtual ~ButtonFeature();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
