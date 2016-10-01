#include "AppleReviewObject.h"
#include "json/writer.h"
namespace iwinmesage
{
	std::string AppleReviewObject::LINK = "http://av.iwin.vn/wf/help/review";

	AppleReviewObject:: AppleReviewObject ()
	{

	}

	AppleReviewObject::~AppleReviewObject()
	{

	}

	rapidjson::Document AppleReviewObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrreviewApps(rapidjson::kArrayType);
		for (size_t i = 0; i < reviewApps.size(); i++)
		{
			arrreviewApps.PushBack(reviewApps.at(i).toJson(), locator);
		}
		document.AddMember("reviewApps",arrreviewApps, locator);
		rapidjson::Value arrdisabledGPlus(rapidjson::kArrayType);
		for (size_t i = 0; i < disabledGPlus.size(); i++)
		{
			arrdisabledGPlus.PushBack(disabledGPlus.at(i).toJson(), locator);
		}
		document.AddMember("disabledGPlus",arrdisabledGPlus, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void AppleReviewObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void AppleReviewObject::toData(rapidjson::Document & document)
	{
				if(document.HasMember("reviewApps") && !document["reviewApps"].IsNull() && document["reviewApps"].IsArray() )
		{
			const rapidjson::Value& arrreviewApps = document["reviewApps"];
			for (rapidjson::SizeType i = 0; i < arrreviewApps.Size(); i++)
			{
							const rapidjson::Value& c = arrreviewApps[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			ReviewApp d;
			d.toData(json);
			reviewApps.push_back(d);

			}
		}
		if(document.HasMember("disabledGPlus") && !document["disabledGPlus"].IsNull() && document["disabledGPlus"].IsArray() )
		{
			const rapidjson::Value& arrdisabledGPlus = document["disabledGPlus"];
			for (rapidjson::SizeType i = 0; i < arrdisabledGPlus.Size(); i++)
			{
							const rapidjson::Value& c = arrdisabledGPlus[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			ReviewApp d;
			d.toData(json);
			disabledGPlus.push_back(d);

			}
		}


	}
}
