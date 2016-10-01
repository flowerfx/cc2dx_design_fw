

#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"

#include "Screens/Object/PlayerGroup.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/SliderBet.h"

#include "Screens/GamePlayMgr.h"

#include "Network/Game/GameService.h"
#include "Network/IwinProtocol.h"
#include "Network/Global/Globalservice.h"

#include "BaiCaoScreen.h"
#include "Utils.h"
#include "AnimUtils.h"
#include "BaiCaoPlayer.h"
#include "FlopCardUI.h"


void BaiCaoScreen::DealCardSeat(int s)
{
	BaiCaoPlayer* p = static_cast<BaiCaoPlayer*>(GetPlayerAtIdx(s));
	if (!p)
		return;
	p->CreateCard(ls_players_card[s]->GetWidgetChildByName(".contain_card") , Vec2(GetGameSize().width / 2, GetGameSize().height / 2));
	p->DealCard();

}

void BaiCaoScreen::ShowButtonSetBet(bool param1)
{
	
}

void BaiCaoScreen::flipCard(int seat, vector<char> ids)
{

}

BaiCaoScreen* BaiCaoScreen::getInstance()
{
	return static_cast<BaiCaoScreen*>(GamePlay->GetCurrentGameScreen());
}

BaiCaoScreen::BaiCaoScreen()
{
	p_panel_slider = nullptr;
	p_panel_custom = nullptr;
	SetMaxPlayer(6);
	ls_cards.resize(16);
	_list_pos_layer_card.clear();
	_list_card_flip.clear();
}

BaiCaoScreen::~BaiCaoScreen()
{
	//p_panel_custom->ClearChild();
	p_panel_custom = nullptr;
	//p_panel_slider->ClearChild();
	p_panel_slider = nullptr;
	ls_players_card.clear();

	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer())
		{
			auto bc_player = static_cast<BaiCaoPlayer*>(p->getPlayer());
			bc_player->ClearCard();
		}
	}

	_list_pos_layer_card.clear();

	if (p_slider_bet)
	{
		delete p_slider_bet;
		p_slider_bet = nullptr;
	}
	_list_card_flip.clear();
}


bool BaiCaoScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);
	
	XMLMgr->OnLoadXMLData<BaiCaoScreen>("BaiCao_Screen", std::mem_fn(&BaiCaoScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("BaiCao_Screen");

	p_panel_bottom = GetWidgetChildByName("HUD_screen.panel_custom_menu.back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");

	flopCard = new FlopCardUI();
	flopCard->init();
	flopCard->setVisible(false);
	addChild(flopCard);
	

	btn_quick_play =static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_quick_play"));
	btn_bet_money = static_cast<ButtonWidget*>( p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_bet_money"));
	btn_flop_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_rub_card"));
	
	livePlayer();

	return true;
}

void BaiCaoScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_bet_money")
	{
		if (!p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
		{
			OnShowBetSlider(true);
		}
		else if (!_isStartGame)// dat cuoc
		{
			OnShowBetSlider(false);
			doSetMoney(p_slider_bet->getValue());
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_quick_play")
	{
		GameService::quickPlay(_roomID,_boardID);
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_rub_card")
	{
	
		Player *p = getPlayer(GameController::myInfo->IDDB);
		if (p)
		{
			p_enable_touch = false;
			flopCard->setVisible(true);
			std::vector<unsigned char> ids;
			int i = 0;
			for (Card* c :((BaiCaoPlayer*)p)->cards)
			{
				if (c->getID() == -1)
				{
					ids.push_back(((BaiCaoPlayer*)p)->cardValue[i]);
				}
				i++;
			}
			if (ids.size() > 0)
			{
				flopCard->setValueCards(ids);
				flopCard->SetAction(
					([this, p](bool value)
				{
					if (value)
					{
						std::vector<char> ids;
						int i = 0;
						bool havecard = false;
						for (Card* c : ((BaiCaoPlayer*)p)->cards)
						{
							if (c->getID() == -1 && !havecard)
							{
								havecard = true;
								ids.push_back(((BaiCaoPlayer*)p)->cardValue[i]);
							}
							else
							{
								ids.push_back(c->getID());
							}
							i++;
						}
						GlobalService::latBaiBaiCao(_roomID, _boardID, ids);
					}
					else
					{
						this->p_enable_touch = true;
						this->flopCard->setVisible(false);
						this->btn_flop_card->Visible(false);
						this->_list_card_flip.clear();
					}
				}));
			}
		}
		
	}
	else if (type_widget == UI_TYPE::UI_SLIDER && name.Contains("slider_bet_money") )
	{
		if (p_slider_bet)
		{
			p_slider_bet->onChangeValue();
		}

		//string text = "Cược (" +Utils::formatNumber(p_slider_bet->getValue())+")";
		//static_cast<ButtonWidget*>(btn_bet_money)->SetTitle(text);
		
	}
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

int BaiCaoScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	//update action all cards;
	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer())
		{
			auto bc_player = static_cast<BaiCaoPlayer*>(p->getPlayer());
			for (auto c : bc_player->cards)
			{
				c->Update(dt);
			}
		}
	}
	
	if (flopCard)
	{
		flopCard->update(dt);
	}

	return 1;
}

void BaiCaoScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//insert into custom menu
	p_panel_custom = GetWidgetChildByName("HUD_screen.panel_custom_menu");
	p_panel_slider = GetWidgetChildByName("HUD_screen.panel_custom_slider_bet");

	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("layout_card"), p_panel_custom, "panel_custom_menu");
	
	//ParseIntoCustom(p_list_sample_dec.at("progress_bar_panel"), p_panel_custom, "panel_custom_menu");

	if (!p_panel_custom)
	{
		PASSERT2(p_panel_custom != nullptr, "panel custom not set!");
		return;
	}
	auto widget_layout_card = p_panel_custom->GetWidgetChildByName(".layout_card");
	

	createDynamicCardLayout(widget_layout_card, 5 , [&](Utility::UI_Widget::WidgetEntity * _widget)
	{
	});
	ParseIntoCustom(p_list_sample_dec.at("sp_slider_bet"), p_panel_slider, "panel_custom_slider_bet");


	ls_players_bet_info.clear();
	for (int i = 0; i <= 3; i++)
	{
		ParseIntoCustom(p_list_sample_dec.at("lb_bet_win_"+std::to_string(i)), p_panel_custom, "panel_custom_menu");
		ls_players_bet_info.push_back(static_cast<TextWidget*>(p_panel_custom->GetWidgetChildByName(".lb_bet_win_"+ std::to_string(i))));
	}

	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();

	p_slider_bet = new SliderBet(p_panel_slider->GetWidgetChildByName(".sp_slider_bet"));
	
}

void BaiCaoScreen::createDynamicCardLayout(WidgetEntity* layerCard,int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func)
{
	if (!layerCard)
	{
		PASSERT2(layerCard != nullptr, "layout card not set!");
		return;
	}
	ls_players_card.push_back(layerCard);
	layerCard->GetWidgetChildByName(".contain_card")->SetActionCommandWithName("SCALE_NORMAL");
	layerCard->SetZOrder(1);
	for (int idx = 1; idx <= numClone; idx++)
	{
		auto _widget = layerCard->Clone();
		_widget->SetName("layout_card_" + std::to_string(idx));
		_widget->SetParentWidget(p_panel_custom);
		_widget->SetParentLayer(p_panel_custom->GetParentLayer());
		_widget->SetMenuParent(p_panel_custom->GetMenuParent());	
		//
		p_panel_custom->InsertChildWidget(_widget);
		ls_players_card.push_back(_widget);
		_widget->GetWidgetChildByName(".contain_card")->SetActionCommandWithName("SCALE_SMALL");
	}

}

void BaiCaoScreen::layoutGame()
{
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		ls_players_card[4]->Visible(false);
		ls_players_card[5]->Visible(false);

		p_list_player_group[4]->setVisible(false);
		p_list_player_group[5]->setVisible(false);

		//set pos layercard
		for (size_t i = 0; i < 4; i++)
		{
			auto _w = _WIDGET(p_list_player_group[i]);
			auto pos = NODE(_w)->convertToWorldSpace(_list_pos_layer_card[i]);
			ls_players_card[i]->SetPosition(pos);
		} 
		//set pos label money 
	}
	else
	{

	}
	
}

void BaiCaoScreen::OnTouchMenuBegin(cocos2d::Point p)
{
	HUDScreen::OnTouchMenuBegin(p);
	if (p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_slider->GetWidgetChildByName(".sp_slider_bet"),
			[&](void)
		{
			p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->ForceFinishAction();
				this->p_panel_slider->GetMenuParent()->SetLayerInteractive(true);
			}));
		});
	}
}

void BaiCaoScreen::OnBetMoney(int money, int id)
{
	int seat = GetSeatFromPlayerID(id);
	if (seat == -1 || ls_players_bet_info.size() <= 0)
		return;
	if (id == _ownerID)
	{
		static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->Visible(false);
		return;
	}
	static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->SetText(Utils::formatNUmber_dot(money));
	static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->Visible(true);
}

void BaiCaoScreen::ChangeBetMoney(int money, ubyte roomID, ubyte boardID)
{
	
}

void BaiCaoScreen::OnStopGame(ubyte numPlayer,vector<vector<int>> cards)
{
	_list_card_flip.clear();
	resetQuickPlay();
	_isStartGame = false;
	_isGameEnd = true;
	_isViewer = false;
	/*if (dealCards == null)
	{
		CreateListCardUp();
	}*/
	stopTimeBar();
	for (int i = 0; i < numPlayer; i++)
	{
		int seat = GetSeatFromPlayerID(cards[i][0]);
		if (seat == -1) continue;
		vector<char> ids(3);

		for (int j = 1; j < 4; j++)
		{
			ids[j - 1] = cards[i][j];
		}
//		flipCard(seat, ids);
		BaiCaoPlayer* p =(BaiCaoPlayer*) getPlayer(cards[i][0]);
		p->FlipCard(ids);
	}

	needReset = true;
	/*StopAllTime();
	ShowStartButton();*/
	_isGameEnd = true;
	//ShowButtonRubCard(false);
	//HideRubCard();

	//Game bài cào thì hiển thị nút sẵng sàng luôn
	//SetReadyButton(false);
	//ShowBetButton();
	ShowButtonAction();

	p_enable_touch = true;
	flopCard->setVisible(false);
	GlobalService::getBetRange();
	ResetReady();
}

void BaiCaoScreen::ShowButtonAction()
{
	if (_isStartGame)
	{
		CCLOG("BaiCaoScreen::ShowButtonAction() : btn_quick_play visible : FALSE");
		btn_flop_card->Visible(true);
		btn_quick_play->Visible(false);
		btn_bet_money->Visible(false);
	}
	else
	{
		CCLOG("BaiCaoScreen::ShowButtonAction() : btn_quick_play visible : TRUE");
		btn_flop_card->Visible(false);
		btn_quick_play->Visible(true);
		if (_ownerID != GameController::myInfo->IDDB) 
		{
			btn_bet_money->Visible(true);
		}
		else
		{
			btn_bet_money->Visible(false);
		}
	}


}

void BaiCaoScreen::OnScores(int who1, string des1)
{
	int s = GetSeatFromPlayerID(who1);
	if (s < 0)
		return;

	auto w_text = ls_players_card[s]->GetWidgetChildByName(".panel_info.text_info");
	w_text->GetParentWidget()->Visible(true);
	static_cast<TextWidget*>(w_text)->SetText(des1);
}

void BaiCaoScreen::OnPlaying(ubyte interval, ubyte numPlayer, int cards[6][4])
{
	_isViewer = true;
	_isStartGame = true;
	_isGameEnd = false;
	for (int i = 0; i < numPlayer; i++)
	{
		int s = GetSeatFromPlayerID(cards[i][0]);
		if (s<=-1 || p_list_player_group[s]->getPlayer() == nullptr || p_list_player_group[s]->getPlayer()->IDDB == -1) continue;
		if (_isViewer)
		{
			continue;
		}
		//seatsDeal[s] = 1;

		DealCardSeat(s);
	}
	_isStartGame = true;
	ShowButtonSetBet(false);
	livePlayer();
	//startTimeBar(interval, 0);
}

void BaiCaoScreen::OnStartGame(vector<ubyte> myCards, ubyte interval1)
{
	_list_card_flip.clear();
	resetQuickPlay();
	stopTimeBar();
	stopCountDownStart();
	resetGame();
	_isStartGame = true;
	_isGameEnd = false;
	//CreateListCardUp();

	ShowButtonSetBet(false);
	/*GameController.Vibrate(VIBRATE_STARTGAME_TIME);
	SoundManager.Instance.PlayEffect(Sounds.STARTGAME);*/

	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer() && GetSeatFromPlayerID(p->getPlayer()->IDDB) > -1)
		{
			auto _player = p->getPlayer();
			auto current_seat = GetSeatFromPlayerID(_player->IDDB);
			auto bc_player = static_cast<BaiCaoPlayer*>(_player);
			bc_player->ClearCard();
			if (_player->IDDB == GameController::myInfo->IDDB)
			{
				bc_player->cardValue = myCards;
			}
			DealCardSeat(current_seat);
		}
	}

	_isStartGame = true;
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		ShowButtonSetBet(false);
	}
	ShowButtonAction();
	startTimeBar(interval1, 0);
	//hide all text status
	for (auto t : ls_players_card)
	{
		t->GetWidgetChildByName(".panel_info")->Visible(false);
	}
	btn_flop_card->Visible(true);
}

void BaiCaoScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	layoutGame();
	resetGame();
	//check if reconnect
	if (p_list_msg_return.size() > 0)
	{
		for (auto msg : p_list_msg_return)
		{
			OnHandleMsg(msg);
			delete msg;
		}
	}
	p_list_msg_return.clear();
}

void BaiCaoScreen::OnReconnect(
	int limitTime, 
	int remainTime, 
	std::vector< std::vector<int>> 
	playerCards, std::vector<int> playerBetMoneys, 
	int nPlayer, ubyte myCards[], 
	int totalMoney, 
	bool isPlaying)
{
	//GameController.Vibrate(1);
	//SoundManager.Instance.PlayEffect(Sounds.BUZZ);
	onReconnect();
	resetGame();
	int seat = -1;
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		for (int i = 0; i < nPlayer; i++)
		{
			seat = GetSeatFromPlayerID(playerCards[i][0]);
			if (seat <= 0) continue;
			static_cast<TextWidget*>(ls_players_bet_info[seat])->SetText(Utils::formatNUmber_dot(playerBetMoneys[i]));
		/*	_listLayerBetMoney[seat].Money = playerBetMoneys[i];
			_playerBetMoney[seat] = playerBetMoneys[i];*/
		}
	}

	if (isPlaying)
	{
		// set screen start game
		//CreateListCardUp();
		/*this.myCards = myCards;
		HideStartButton();
		var seatsDeal = new int[NUM_SEATS];*/

		_isStartGame = true;
		ResetReady();
		/*
		ShowButtonRubCard(true);
		if (GameController.gameId == GameController.GAMEID_BAICAO)
		{
			ShowButtonSetBet(false);
		}*/
		ShowButtonAction();
		startTimeBar(limitTime, remainTime);

		seat = -1;
		bool isShowFlopCard= false;
		for (int i = 0; i < nPlayer; i++)
		{
			seat = GetSeatFromPlayerID(playerCards[i][0]);
			if (seat <= -1) continue;
			
			std::vector<char> ids;

			for (int j = 1; j < 4; j++)
			{
				ids.push_back( (char)playerCards[i][j]);
				if (!isShowFlopCard &&playerCards[i][0] == GameController::myInfo->IDDB && playerCards[i][j] <=-1)
				{
					isShowFlopCard = true;
				}
			}
			DealCardSeat(seat);
			OnFlipCard(playerCards[i][0], ids);
			btn_flop_card->Visible(isShowFlopCard);
			
		}
	}
}

void BaiCaoScreen::OnFlipCard(int id, std::vector<char> cardID)
{
	BaiCaoPlayer* p = (BaiCaoPlayer*)getPlayer(id);
	if (!p)
		return;
	CCLOG("ACTUAL FLIP BAI CAO id: %d , %d , %d", cardID[0], cardID[1], cardID[2]);
	p->FlipCard(cardID);
	if (id == GameController::myInfo->IDDB)
	{
		bool isFullFlop = true;
		for (auto c : _list_card_flip)
		{
			if (c < 0)
			{
				isFullFlop = false;
				break;
			}
		}
		if (_list_card_flip.size() <= 0)
		{
			isFullFlop = false;
		}

		if (isFullFlop)
		{
			_list_card_flip.clear();
			p_enable_touch = true;
			flopCard->setVisible(false);
			btn_flop_card->Visible(false);
		}
	}

}

void BaiCaoScreen::doFlopCard(std::vector<char> ids)
{
	//SoundManager.Instance.PlayEffect(Sounds.LATBAI);
	CCLOG("PRESS BAI CAO id :  %d , %d , %d", ids[0], ids[1], ids[2]);
	if (_list_card_flip.size() == 0)
	{
		_list_card_flip = ids;
	}
	else
	{
		int idx = 0;
		for (auto c : _list_card_flip)
		{
			if (c >= 0)
			{
				ids[idx] = c;
			}
			else if (c < 0 && ids[idx] >= 0)
			{
				_list_card_flip[idx] = ids[idx];
			}
			idx++;
		}
	}

	GameService::latBaiBaiCao(_roomID, _boardID, ids);
}

void BaiCaoScreen::resetGame()
{
	if (_isStartGame)
		return;
	GameScreen::resetGame();
	ShowButtonAction();
	p_enable_touch = true;
	flopCard->setVisible(false);
	showLbInfoBet();
	hideAllCard();
	hideAllFlare();

	livePlayer();
	_list_card_flip.clear();
	_isViewer = false;
}

void BaiCaoScreen::hideAllCard()
{
	for (auto p : p_list_player_group)
	{
		if (p 
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB != -1)
		{
			static_cast<BaiCaoPlayer*>(_PLAYER(p))->HideAllCard();
		}
	}
}

void BaiCaoScreen::showLbInfoBet()
{
	size_t i = 0;
	for (auto lb :ls_players_bet_info)
	{
		if(!lb)
			continue;
		if (i < p_list_player_group.size()&& 
			p_list_player_group[i] && 
			p_list_player_group[i]->getPlayer() && 
			p_list_player_group[i]->getPlayer()->IDDB != _ownerID)
		{
			lb->Visible(true);
		}
		else
		{
			lb->Visible(false);
		}
		i++;
	}
}

void BaiCaoScreen::onSomeOneJoinBoard(int seat, Player * player)
{
	GameScreen::onSomeOneJoinBoard(seat, player);
	livePlayer();
}

void BaiCaoScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	int s = GetSeatFromPlayerID(leave);
	if (!_isStartGame && s >= 0)
	{
		ls_players_bet_info[s]->Visible(false);
	}

	GameScreen::onSomeOneLeaveBoard(leave, newOwner);

	livePlayer();
}


void BaiCaoScreen::OnCardTouchEnd()
{
	bool isFlop = false;
	vector<char> lsCardSelects;
	int idx = 0;
	for (Card *c : GetMe()->cards) 
	{
		if (NODE(c)->getTag() == 1) 
		{
			lsCardSelects.push_back(GetMe()->GetCardValueIdx(idx));
			isFlop = true;
		}
		else
		{
			lsCardSelects.push_back(c->getID());
		}
		NODE(c)->setTag(0);
		idx++;
	}

	if (lsCardSelects.size() > 0 && isFlop)
	{
		this->doFlopCard(lsCardSelects);
	}
}

void BaiCaoScreen::OnCardTouchDown()
{
	for (Card *c : GetMe()->cards) {
		cocos2d::Vec2 wPos = NODE(c)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
		auto rect = c->GetBound();
		auto id = c->getID();
		if (rect.containsPoint(wPos) && id == -1) 
		{
			NODE(c)->setTag(1);
			return;
		}
	}
}

void BaiCaoScreen::OnCardTouchMove()
{
	for (Card* c : GetMe()->cards) 
	{
		cocos2d::Vec2 wPos = NODE(c)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
		if (c->GetBound().containsPoint(wPos) && NODE(c)->getTag() != 1 && c->getID() == -1) 
		{
			NODE(c)->setTag(1);
		}
	}
}

BaiCaoPlayer* BaiCaoScreen::GetMe()
{
	return static_cast<BaiCaoPlayer*>(getMyPlayer());
}

void BaiCaoScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	Vec2 pos = dec->GetDataVector2("pos_contain_card");
	pos.x = pos.x * GetGameSize().width / this->GetDesignSize().x;
	pos.y = pos.y * GetGameSize().height / this->GetDesignSize().y;
	_list_pos_layer_card.push_back(pos);
}

void BaiCaoScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney)
{
	if (value > 0)
	{
		player->showFlare(true);
	}
}

void BaiCaoScreen::hideAllFlare()
{
	for (auto p : p_list_player_group)
	{
		p->showFlare(false);
	}
}

void BaiCaoScreen::onBetRange(int min, s64 & max, s64 maxMoneyRoom)
{
	GameScreen::onBetRange(min, max, maxMoneyRoom);
	if (p_slider_bet)
	{
		p_slider_bet->setMinMax(min, max);
	}
	else
	{
		PASSERT2(false, "wrong");
	}
}

void BaiCaoScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay)
{
	GameScreen::onSomeOneQuickPlay(playerID, isQuickPlay);
	if (playerID == GameController::myInfo->IDDB && isQuickPlay) 
	{
		//canClearCard = false;
		CCLOG("BaiCaoScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay) : btn_quick_play visible : FALSE");
		btn_quick_play->Visible(false);
		OnShowBetSlider(false);
		if (isQuickPlay 
			&& _myInfo 
			&& _PLAYER(_myInfo)
			&& _PLAYER(_myInfo)->IDDB != _ownerID)
		{
			btn_bet_money->Visible(false);
		}
	}
}

void BaiCaoScreen::OnShowBetSlider(bool show)
{
	if (!show && p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this]()
		{
			this->p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->ForceFinishAction();
			this->p_panel_slider->GetMenuParent()->SetLayerInteractive(true);
		}));
	}
	else if (show && !p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this]()
		{
			this->p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->ForceFinishAction();
			this->p_panel_slider->GetMenuParent()->SetLayerInteractive(false);

			this->p_panel_custom->SetInteractive(true);
			this->p_panel_slider->SetInteractive(true);
			//request bet range from server !!!!
			GlobalService::getBetRange();
		}));
	}

}

void BaiCaoScreen::doSetMoney(int money)
{
	GameService::setCommandinGametoServer(
		(GameController::getCurrentGameType() == GameType::GAMEID_BAICAO) ? IwinProtocol::WANT_DRAW : IwinProtocol::SET_MONEY, 
		_roomID, 
		_boardID, 
		money);
}

void BaiCaoScreen::onGetTimeLeft(float timeLeft)
{
	OnShowCountDown(timeLeft, 0);
	livePlayer();
}

void BaiCaoScreen::livePlayer()
{
	int livePlayer = 0;
	for (auto p : p_list_player_group) 
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0) 
		{
			livePlayer++;
		}
	}

	if (_myInfo && _PLAYER(_myInfo) && !_PLAYER(_myInfo)->isQuickPlay) 
	{
		if (livePlayer < 2) 
		{
			onSomeOneQuickPlay(_PLAYER(_myInfo)->IDDB, false);
			CCLOG("BaiCaoScreen::livePlayer() : btn_quick_play visible : FALSE");
			btn_quick_play->Visible(false);
		}
		else
		{
			CCLOG("BaiCaoScreen::livePlayer() : btn_quick_play visible : TRUE");
			btn_quick_play->Visible(true);
		}
	}
	if (_isViewer)
	{
		CCLOG("BaiCaoScreen::livePlayer() _isViewer: btn_quick_play visible : FALSE");
		btn_quick_play->Visible(false);
	}
	// return livePlayer;
}

void BaiCaoScreen::resetQuickPlay() 
{
	CCLOG("BaiCaoScreen::resetQuickPlay() : btn_quick_play visible : TRUE");
	btn_quick_play->Visible(true);
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0)
		{
			onSomeOneQuickPlay(_PLAYER(p)->IDDB, false);
		}
	}

}