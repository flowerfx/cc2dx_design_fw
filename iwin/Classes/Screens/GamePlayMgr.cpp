#include "GamePlayMgr.h"
#include "GameScreens/TienLen/TienLenScreen.h"
#include "GameScreens/MauBinh/MauBinhScreen.h"
#include "RKString_Code/RKString.h"
#include "GameScreens/BaiCao/BaiCaoScreen.h"
#include "GameScreens/Phom/PhomScreen.h"
#include "GameScreens/XiZach/XiZachScreen.h"
#include "GameScreens/CoTuong/CoTuongScreen.h"
using namespace RKUtils;
GamePlayMgr::GamePlayMgr()
{
	p_current_game_screen = nullptr;
	RegisterGameScreen();
}

GamePlayMgr::~GamePlayMgr()
{
	if (p_current_game_screen)
	{
		delete p_current_game_screen;
		p_current_game_screen = nullptr;
	}
	p_factory_game_screen_entity.Clear();
}

bool GamePlayMgr::Init(int game_id)
{
	auto id = game_id;// GetGameTypeID(game_id);

	auto val = p_factory_game_screen_entity.CreateNewProduct(id);
	if (val)
	{
		val->InitGame(game_id);
		p_current_game_screen = val;
	}
	return true;
}

void GamePlayMgr::RegisterGameScreen()
{
	p_factory_game_screen_entity.RegisterProduct<TienLenScreen>(GameType::GAMEID_TIENLEN);
	p_factory_game_screen_entity.RegisterProduct<TienLenScreen>(GameType::GAMEID_TIENLEN_MB);
	p_factory_game_screen_entity.RegisterProduct<TienLenScreen>(GameType::GAMEID_TIENLEN_SOLO);
	p_factory_game_screen_entity.RegisterProduct<BaiCaoScreen>(GameType::GAMEID_BAICAO);
	p_factory_game_screen_entity.RegisterProduct<BaiCaoScreen>(GameType::GAMEID_BAICAORUA);
	p_factory_game_screen_entity.RegisterProduct<MauBinhScreen>(GameType::GAMEID_BINH);
	p_factory_game_screen_entity.RegisterProduct<PhomScreen>(GameType::GAMEID_PHOM_MB);
	p_factory_game_screen_entity.RegisterProduct<XiZachScreen>(GameType::GAMEID_BLACKJACK);
	p_factory_game_screen_entity.RegisterProduct<CoTuongScreen>(GameType::GAMEID_CHESS);
}

GameType GamePlayMgr::GetGameTypeID(int game_id)
{
	return GameType::GAMEID_TIENLEN;
}