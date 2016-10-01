#ifndef _EventType_H_
#define _EventType_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class EventType
	{
		

	public:
		static  int NEWYEAR_ENVELOPE;
static  int DOMINO_RANKING;
static  int DOMINO_RECEIVE_CARD;
static  int DANG_TRONG_DEN_HUNG;

		EventType();
		virtual ~EventType();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
