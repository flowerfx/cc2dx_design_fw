#ifndef _AppleVerify_H_
#define _AppleVerify_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class AppleVerify
	{
		
CC_SYNTHESIZE(std::string, playerID, PlayerID);
CC_SYNTHESIZE(std::string, bundleID, BundleID);
CC_SYNTHESIZE(std::string, publicKeyURL, PublicKeyURL);
CC_SYNTHESIZE(std::string, signature, Signature);
CC_SYNTHESIZE(std::string, salt, Salt);
CC_SYNTHESIZE(int64_t, timestamp, Timestamp);

	public:
		
		AppleVerify();
		virtual ~AppleVerify();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
