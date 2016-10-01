#ifndef _EventItemId_H_
#define _EventItemId_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class EventItemId
	{
		

	public:
		static  int PHUC;
static  int LOC;
static  int THO;
static  int TRONG_SAT;
static  int TRONG_DONG;
static  int TRONG_BAC;
static  int TRONG_VANG;
static  int BUA;

		EventItemId();
		virtual ~EventItemId();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
