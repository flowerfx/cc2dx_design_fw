#include "GameState.h"
#include "json/writer.h"
namespace iwinmesage
{
	int GameState::STATE_PAUSE = 0;
int GameState::STATE_RESUME = 1;

	GameState:: GameState ()
	{

	}

	GameState::~GameState()
	{

	}

	rapidjson::Document GameState::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("state", state,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GameState::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GameState::toData(rapidjson::Document & document)
	{
				if(document.HasMember("state"))
		{
			setState(document["state"].GetInt());
		}


	}
}
