#include "GameList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GameList:: GameList ()
	{

	}

	GameList::~GameList()
	{

	}

	rapidjson::Document GameList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrgamePopular(rapidjson::kArrayType);
		for (size_t i = 0; i < gamePopular.size(); i++)
		{
			arrgamePopular.PushBack(gamePopular.at(i), locator);
		}
		document.AddMember("gamePopular",arrgamePopular, locator);
		rapidjson::Value arrgameCard(rapidjson::kArrayType);
		for (size_t i = 0; i < gameCard.size(); i++)
		{
			arrgameCard.PushBack(gameCard.at(i), locator);
		}
		document.AddMember("gameCard",arrgameCard, locator);
		rapidjson::Value arrgameChess(rapidjson::kArrayType);
		for (size_t i = 0; i < gameChess.size(); i++)
		{
			arrgameChess.PushBack(gameChess.at(i), locator);
		}
		document.AddMember("gameChess",arrgameChess, locator);
		rapidjson::Value arrgameOther(rapidjson::kArrayType);
		for (size_t i = 0; i < gameOther.size(); i++)
		{
			arrgameOther.PushBack(gameOther.at(i), locator);
		}
		document.AddMember("gameOther",arrgameOther, locator);
		rapidjson::Value arrgameHot(rapidjson::kArrayType);
		for (size_t i = 0; i < gameHot.size(); i++)
		{
			arrgameHot.PushBack(gameHot.at(i), locator);
		}
		document.AddMember("gameHot",arrgameHot, locator);
		rapidjson::Value arrgameNew(rapidjson::kArrayType);
		for (size_t i = 0; i < gameNew.size(); i++)
		{
			arrgameNew.PushBack(gameNew.at(i), locator);
		}
		document.AddMember("gameNew",arrgameNew, locator);
		rapidjson::Value arrgameUpdate(rapidjson::kArrayType);
		for (size_t i = 0; i < gameUpdate.size(); i++)
		{
			arrgameUpdate.PushBack(gameUpdate.at(i), locator);
		}
		document.AddMember("gameUpdate",arrgameUpdate, locator);
		rapidjson::Value arrgameTournaments(rapidjson::kArrayType);
		for (size_t i = 0; i < gameTournaments.size(); i++)
		{
			arrgameTournaments.PushBack(gameTournaments.at(i), locator);
		}
		document.AddMember("gameTournaments",arrgameTournaments, locator);
		rapidjson::Value arrgameHasNewLobby(rapidjson::kArrayType);
		for (size_t i = 0; i < gameHasNewLobby.size(); i++)
		{
			arrgameHasNewLobby.PushBack(gameHasNewLobby.at(i), locator);
		}
		document.AddMember("gameHasNewLobby", arrgameHasNewLobby, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GameList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void GameList::toData(rapidjson::Document & document)
	{
				if(document.HasMember("gamePopular") && !document["gamePopular"].IsNull() && document["gamePopular"].IsArray() )
		{
			const rapidjson::Value& arrgamePopular = document["gamePopular"];
			for (rapidjson::SizeType i = 0; i < arrgamePopular.Size(); i++)
			{
				gamePopular.push_back(arrgamePopular[i].GetInt());
			}
		}
		if(document.HasMember("gameCard") && !document["gameCard"].IsNull() && document["gameCard"].IsArray() )
		{
			const rapidjson::Value& arrgameCard = document["gameCard"];
			for (rapidjson::SizeType i = 0; i < arrgameCard.Size(); i++)
			{
				gameCard.push_back(arrgameCard[i].GetInt());
			}
		}
		if(document.HasMember("gameChess") && !document["gameChess"].IsNull() && document["gameChess"].IsArray() )
		{
			const rapidjson::Value& arrgameChess = document["gameChess"];
			for (rapidjson::SizeType i = 0; i < arrgameChess.Size(); i++)
			{
				gameChess.push_back(arrgameChess[i].GetInt());
			}
		}
		if(document.HasMember("gameOther") && !document["gameOther"].IsNull() && document["gameOther"].IsArray() )
		{
			const rapidjson::Value& arrgameOther = document["gameOther"];
			for (rapidjson::SizeType i = 0; i < arrgameOther.Size(); i++)
			{
				gameOther.push_back(arrgameOther[i].GetInt());
			}
		}
		if(document.HasMember("gameHot") && !document["gameHot"].IsNull() && document["gameHot"].IsArray() )
		{
			const rapidjson::Value& arrgameHot = document["gameHot"];
			for (rapidjson::SizeType i = 0; i < arrgameHot.Size(); i++)
			{
				gameHot.push_back(arrgameHot[i].GetInt());
			}
		}
		if(document.HasMember("gameNew") && !document["gameNew"].IsNull() && document["gameNew"].IsArray() )
		{
			const rapidjson::Value& arrgameNew = document["gameNew"];
			for (rapidjson::SizeType i = 0; i < arrgameNew.Size(); i++)
			{
				gameNew.push_back(arrgameNew[i].GetInt());
			}
		}
		if(document.HasMember("gameUpdate") && !document["gameUpdate"].IsNull() && document["gameUpdate"].IsArray() )
		{
			const rapidjson::Value& arrgameUpdate = document["gameUpdate"];
			for (rapidjson::SizeType i = 0; i < arrgameUpdate.Size(); i++)
			{
				gameUpdate.push_back(arrgameUpdate[i].GetInt());
			}
		}
		if(document.HasMember("gameTournaments") && !document["gameTournaments"].IsNull() && document["gameTournaments"].IsArray() )
		{
			const rapidjson::Value& arrgameTournaments = document["gameTournaments"];
			for (rapidjson::SizeType i = 0; i < arrgameTournaments.Size(); i++)
			{
				gameTournaments.push_back(arrgameTournaments[i].GetInt());
			}
		}
		if (document.HasMember("gameHasNewLobby") && !document["gameHasNewLobby"].IsNull() && document["gameHasNewLobby"].IsArray())
		{
			const rapidjson::Value& arrgameHasNewLobby = document["gameHasNewLobby"];
			for (rapidjson::SizeType i = 0; i < arrgameHasNewLobby.Size(); i++)
			{
				gameHasNewLobby.push_back(arrgameHasNewLobby[i].GetInt());
			}
		}


	}
	
	bool GameList::hasNewLobby(int gameId)
	{
		for (auto g : gameHasNewLobby)
		{
			if (g == gameId)
			{
				return true;
			}
		}
		return false;
	}
}
