#ifndef _GameState_H_
#define _GameState_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GameState
	{
		
CC_SYNTHESIZE(int, state, State);

	public:
		static  int STATE_PAUSE;
static  int STATE_RESUME;

		GameState();
		virtual ~GameState();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
