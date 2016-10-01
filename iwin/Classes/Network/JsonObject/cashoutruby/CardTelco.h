#ifndef _Card_H_
#define _Card_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Card
	{
		
CC_SYNTHESIZE(int, cardId, CardId);
CC_SYNTHESIZE(int, vnd, Vnd);
CC_SYNTHESIZE(int, ruby, Ruby);

	public:
		
		Card();
		virtual ~Card();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
