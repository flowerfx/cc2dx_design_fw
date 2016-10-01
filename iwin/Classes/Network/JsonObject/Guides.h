#ifndef _Guides_H_
#define _Guides_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class Guides
	{
		
CC_SYNTHESIZE(std::string, help, Help);
CC_SYNTHESIZE(std::string, payment, Payment);
CC_SYNTHESIZE(std::string, winfree, Winfree);
CC_SYNTHESIZE(std::string, activity, Activity);
CC_SYNTHESIZE(std::string, betasia, Betasia);
CC_SYNTHESIZE(std::string, stock, Stock);
CC_SYNTHESIZE(std::string, moonfestival, Moonfestival);
CC_SYNTHESIZE(std::string, independence, Independence);
CC_SYNTHESIZE(std::string, halloween, Halloween);
CC_SYNTHESIZE(std::string, phucloctho, Phucloctho);
CC_SYNTHESIZE(std::string, lixi, Lixi);
CC_SYNTHESIZE(std::string, dangtrong, Dangtrong);
CC_SYNTHESIZE(std::string, rankingdomino, Rankingdomino);
CC_SYNTHESIZE(std::string, dice, Dice);
CC_SYNTHESIZE(std::string, giaidau, Giaidau);
CC_SYNTHESIZE(std::string, game, Game);

	public:
		
		Guides();
		virtual ~Guides();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
