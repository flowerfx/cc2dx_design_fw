#include "AppleVerify.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	AppleVerify:: AppleVerify ()
	{

	}

	AppleVerify::~AppleVerify()
	{

	}

	rapidjson::Document AppleVerify::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vplayerID(playerID.c_str(), (unsigned int)playerID.size());
		document.AddMember("playerID", vplayerID,locator);
rapidjson::Value vbundleID(bundleID.c_str(), (unsigned int)bundleID.size());
		document.AddMember("bundleID", vbundleID,locator);
rapidjson::Value vpublicKeyURL(publicKeyURL.c_str(), (unsigned int)publicKeyURL.size());
		document.AddMember("publicKeyURL", vpublicKeyURL,locator);
rapidjson::Value vsignature(signature.c_str(), (unsigned int)signature.size());
		document.AddMember("signature", vsignature,locator);
rapidjson::Value vsalt(salt.c_str(), (unsigned int)salt.size());
		document.AddMember("salt", vsalt,locator);
		document.AddMember("timestamp", timestamp,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void AppleVerify::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void AppleVerify::toData(rapidjson::Document & document)
	{
				if(document.HasMember("playerID"))
		{
			setPlayerID(document["playerID"].GetString());
		}
		if(document.HasMember("bundleID"))
		{
			setBundleID(document["bundleID"].GetString());
		}
		if(document.HasMember("publicKeyURL"))
		{
			setPublicKeyURL(document["publicKeyURL"].GetString());
		}
		if(document.HasMember("signature"))
		{
			setSignature(document["signature"].GetString());
		}
		if(document.HasMember("salt"))
		{
			setSalt(document["salt"].GetString());
		}
		if(document.HasMember("timestamp"))
		{
			setTimestamp(document["timestamp"].GetInt64());
		}


	}
}
