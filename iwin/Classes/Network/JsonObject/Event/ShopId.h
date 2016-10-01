#ifndef _ShopId_H_
#define _ShopId_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ShopId
	{
		

	public:
		static  int TRONG_SAT;
static  int BUA;
static  int COMBO_TRONG_BUA;
static  int NAP_WIN_TANG_TRONG_SAT;

		ShopId();
		virtual ~ShopId();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
