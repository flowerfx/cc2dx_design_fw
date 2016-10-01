

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

#include "XiZachScreen.h"
#include "Utils.h"
#include "AnimUtils.h"
#include "XiZachPlayer.h"
#include "../BaiCao/FlopCardUI.h"


void XiZachScreen::DealCardSeat(int s,int num)
{
	XiZachPlayer* p = static_cast<XiZachPlayer*>(GetPlayerAtIdx(s));
	if (!p)
		return;
	p->CreateCard(ls_players_card[s]->GetWidgetChildByName(".contain_card"), Vec2(GetGameSize().width / 2, GetGameSize().height / 2),2);
	p->DealCard(s, num);

}

void XiZachScreen::ShowButtonSetBet(bool param1)
{

}

void XiZachScreen::flipCard(int seat, vector<char> ids)
{

}

XiZachScreen* XiZachScreen::getInstance()
{
	return static_cast<XiZachScreen*>(GamePlay->GetCurrentGameScreen());
}

XiZachScreen::XiZachScreen()
{
	p_panel_slider = nullptr;
	p_panel_custom = nullptr;
	SetMaxPlayer(4);
	ls_cards.resize(2);
	_list_pos_layer_card.clear();
	_scores = { "","","","" };
	whoMove = -1;
}

XiZachScreen::~XiZachScreen()
{
	p_panel_custom = nullptr;
	p_panel_slider = nullptr;
	ls_players_card.clear();

	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer())
		{
			auto bc_player = static_cast<XiZachPlayer*>(p->getPlayer());
			bc_player->ClearCard();
		}
	}

	_list_pos_layer_card.clear();

	if (p_slider_bet)
	{
		delete p_slider_bet;
		p_slider_bet = nullptr;
	}
	_scores.clear();
}


bool XiZachScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);

	XMLMgr->OnLoadXMLData<XiZachScreen>("XiZach_Screen", std::mem_fn(&XiZachScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("XiZach_Screen");

	p_panel_bottom = GetWidgetChildByName("HUD_screen.panel_custom_menu.back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");

	flopCard = new FlopCardUI();
	flopCard->init();
	flopCard->setVisible(false);
	addChild(flopCard);


	btn_quick_play = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_quick_play"));
	btn_bet_money = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_bet_money"));
	btn_flop_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_rub_card"));
	btn_done = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_done"));
	

	return true;
}

void XiZachScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_bet_money")
	{
		if (!p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
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
		else // dat cuoc
		{
			if (!_isStartGame)
			{
				GameService::setCommandinGametoServer(IwinProtocol::WANT_DRAW, _roomID, _boardID, p_slider_bet->getValue());
			}

		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_quick_play")
	{
		GameService::quickPlay(_roomID, _boardID);
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_done")
	{
		GlobalService::Fire(IwinProtocol::MOVE, _roomID, _boardID, 1);
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
			vector<Card*> lsCardShow = ((XiZachPlayer*)p)->getCardShow();
			for (Card* c : lsCardShow)
			{
				if ( c->getID() == -1)
				{
					ids.push_back(((XiZachPlayer*)p)->cardValue[i]);
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
						vector<Card*> lsCardShow = ((XiZachPlayer*)p)->getCardShow();
						for (Card* c : lsCardShow)
						{
							if ( c->getID() == -1 && !havecard)
							{
								havecard = true;
								ids.push_back(((XiZachPlayer*)p)->cardValue[i]);
							}
							else
							{
								ids.push_back(c->getID());
							}
							i++;
						}
						OnFlipCard(GameController::myInfo->IDDB, ids);
					}
					else
					{
						p_enable_touch = true;
						flopCard->setVisible(false);
						btn_flop_card->Visible(false);
					}
				}));
			}
		}

	}
	else if (type_widget == UI_TYPE::UI_SLIDER && name.Contains("slider_bet_money"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->onChangeValue();
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON &&  btn_retrieve->IsVisible() && name.Contains("btn_retrieve"))
	{
		if (_isAllowRetrieveCard)
		{
			GlobalService::Fire(IwinProtocol::MOVE, _roomID, _boardID, 0);
			_isAllowRetrieveCard = false;
		}
	}
	else
	{		
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

int XiZachScreen::Update(float dt)
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
			auto bc_player = static_cast<XiZachPlayer*>(p->getPlayer());
			for (auto c : bc_player->cards)
			{
				if (c && c ->IsVisible())
				{
					c->Update(dt);
				}
				
			}
		}
	}

	if (flopCard && flopCard->isVisible())
	{
		flopCard->update(dt);
	}

	return 1;
}

void XiZachScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
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


	createDynamicCardLayout(widget_layout_card, 3, [&](Utility::UI_Widget::WidgetEntity * _widget)
	{
	});
	ParseIntoCustom(p_list_sample_dec.at("sp_slider_bet"), p_panel_slider, "panel_custom_slider_bet");


	ls_players_bet_info.clear();
	for (int i = 0; i <= 3; i++)
	{
		ParseIntoCustom(p_list_sample_dec.at("lb_bet_win_" + std::to_string(i)), p_panel_custom, "panel_custom_menu");
		ls_players_bet_info.push_back(static_cast<TextWidget*>(p_panel_custom->GetWidgetChildByName(".lb_bet_win_" + std::to_string(i))));
	}
	/*ParseIntoCustom(p_list_sample_dec.at("btn_retrieve"), p_panel_custom, "panel_custom_menu");
	btn_retrieve = static_cast<ButtonWidget*>(p_panel_custom->GetWidgetChildByName(".btn_retrieve"));*/
	
	ParseIntoCustom(p_list_sample_dec.at("panel_retrieve"), p_panel_custom, "panel_custom_menu");
	btn_retrieve = static_cast<LayoutWidget*>(p_panel_custom->GetWidgetChildByName(".panel_retrieve"));

	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();

	p_slider_bet = new SliderBet(p_panel_slider->GetWidgetChildByName(".sp_slider_bet"));

	
}

void XiZachScreen::createDynamicCardLayout(WidgetEntity* layerCard, int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func)
{
	if (!layerCard)
	{
		PASSERT2(layerCard != nullptr, "layout card not set!");
		return;
	}
	ls_players_card.push_back(layerCard);
	layerCard->SetZOrder(1);
	for (int idx = 1; idx <= numClone; idx++)
	{
		auto _widget = layerCard->Clone();
		_widget->SetName("layout_card_" + std::to_string(idx));
		_widget->SetParentWidget(p_panel_custom);
		_widget->SetParentLayer(p_panel_custom->GetParentLayer());
		_widget->SetMenuParent(p_panel_custom->GetMenuParent());
		auto btn = _widget->GetWidgetChildByName(".contain_card.btn_judge_card");
		if (idx == 1)
		{
			btn->SetPosition(Vec2(0,_widget->getHieght()/2));
			btn->GetResource()->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		}else if (idx == 2)
		{
			btn->SetPosition(Vec2( _widget->getWidth()/2, _widget->getHieght() / 2));
			btn->GetResource()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		}
		else
		{
			btn->SetPosition(Vec2(_widget->getWidth(), _widget->getHieght() / 2));
			btn->GetResource()->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
		}
		btn->GetResource()->setZOrder(50);
		static_cast<ButtonWidget*>(btn)->SetCallBack([this,idx](Ref* ref , Widget::TouchEventType event) {
			if (event == Widget::TouchEventType::ENDED)
			{
				if (_isStartGame &&GameController::myInfo->IDDB == _ownerID && whoMove == _ownerID)
				{

					if (p_list_player_group[idx]->getPlayer())
					{
						GlobalService::sendCommandinGametoServer(IwinProtocol::ADD_CARD, _roomID, _boardID, p_list_player_group[idx]->getPlayer()->IDDB);
					}
				}
			}
		});
		//
		p_panel_custom->InsertChildWidget(_widget);
		ls_players_card.push_back(_widget);
	}

}

void XiZachScreen::layoutGame()
{
	//set pos layercard
	for (size_t i = 0; i < 4; i++)
	{
		auto _w = _WIDGET(p_list_player_group[i]);
		auto pos = NODE(_w)->convertToWorldSpace(_list_pos_layer_card[i]);
		ls_players_card[i]->SetPosition(pos);
	}
}

void XiZachScreen::OnTouchMenuBegin(cocos2d::Point p)
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



void XiZachScreen::ShowButtonAction(bool btnQuickPlay, bool btnDonde, bool btnFlop, bool btnBet)
{
	int maginL = 10;
	int xMagin = btn_quick_play->GetParentWidget()->getWidth() - maginL;
	btn_bet_money->Visible(btnBet);
	if (btnBet)
	{
		xMagin -= btn_bet_money->getWidth() + maginL;
		btn_bet_money->SetPosition(Vec2(xMagin, btn_bet_money->GetPosition().y));
	}
	btn_flop_card->Visible(btnFlop);
	if (btnFlop)
	{
		xMagin -= btn_flop_card->getWidth() + maginL;
		btn_flop_card->SetPosition(Vec2(xMagin, btn_flop_card->GetPosition().y));
	}
	btn_done->Visible(btnDonde);
	if (btnDonde)
	{
		xMagin -= btn_done->getWidth() + maginL;
		btn_done->SetPosition(Vec2(xMagin, btn_done->GetPosition().y));
	}
	btn_quick_play->Visible(btnQuickPlay);
	if (btnQuickPlay)
	{
		xMagin -= btn_quick_play->getWidth() + maginL;
		btn_quick_play->SetPosition(Vec2(xMagin, btn_quick_play->GetPosition().y));
	}
	
	
}


void XiZachScreen::OnFadeInComplete()
{
	GameScreen::OnFadeInComplete();
}

void XiZachScreen::resetGame()
{
	if (_isStartGame)
		return;
	GameScreen::resetGame();
	p_enable_touch = true;
	flopCard->setVisible(false);
	showLbInfoBet();
	hideAllCard();
	hideAllFlare();
	ShowButtonRetrieveCard(false);
	ShowButtonAction(true,false,false,_ownerID != GameController::myInfo->IDDB);
	HideAllFudge();
	_isAllowRetrieveCard = true;


	//demo
	/*for (int i=1;i<4;i++)
	{
		auto p = new XiZachPlayer();
		p_list_player_group[i]->setPlayer(p);
		DealCardSeat(i, 5);
	}
	DealCardSeat(0, 5);*/

}

void XiZachScreen::hideAllCard()
{
	for (auto p : p_list_player_group)
	{
		if (p
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB != -1)
		{
			int seat = GetSeatFromPlayerID(p->getPlayer()->IDDB);
			auto w_text = ls_players_card[seat]->GetWidgetChildByName(".panel_info.text_info");
			w_text->GetParentWidget()->Visible(false);
			static_cast<TextWidget*>(w_text)->SetText("");
			static_cast<XiZachPlayer*>(_PLAYER(p))->HideAllCard();
		}
	}
}

void XiZachScreen::showLbInfoBet()
{
	size_t i = 0;
	for (auto lb : ls_players_bet_info)
	{
		if (!lb)
			continue;
		if (i < p_list_player_group.size() &&
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

void XiZachScreen::onSomeOneJoinBoard(int seat, Player * player)
{
	GameScreen::onSomeOneJoinBoard(seat, player);
}

void XiZachScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
	int s = GetSeatFromPlayerID(leave);
	if (!_isStartGame && s >= 0)
	{
		ls_players_bet_info[s]->Visible(false);
	}
}


void XiZachScreen::OnCardTouchEnd()
{
	bool isFlop = false;
	vector<char> lsCardSelects;
	int idx = 0;
	for (Card *c : GetMe()->cards)
	{
		if (c->IsVisible())
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
		}
		idx++;
	}

	if (lsCardSelects.size() > 0 && isFlop)
	{
		OnFlipCard(GameController::myInfo->IDDB, lsCardSelects);
	}
}

void XiZachScreen::OnCardTouchDown()
{
	for (Card *c : GetMe()->cards) {
		if(!c->IsVisible())
			continue;
		cocos2d::Vec2 wPos = NODE(c)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
		auto rect = c->GetBound();
		auto id = c->getID();
		if (rect.containsPoint(wPos) && id == -1) {
			NODE(c)->setTag(1);
			return;
		}
	}
}

void XiZachScreen::OnCardTouchMove()
{
	for (Card* c : GetMe()->cards) {
		if (!c->IsVisible())
			continue;
		cocos2d::Vec2 wPos = NODE(c)->getParent()->convertTouchToNodeSpace(InputMgr->GetTouch());
		if (c->GetBound().containsPoint(wPos) && NODE(c)->getTag() != 1 && c->getID() == -1) {
			NODE(c)->setTag(1);
		}
	}
}

XiZachPlayer* XiZachScreen::GetMe()
{
	return static_cast<XiZachPlayer*>(getMyPlayer());
}



void XiZachScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	Vec2 pos = dec->GetDataVector2("pos_contain_card");
	pos.x = pos.x * GetGameSize().width / this->GetDesignSize().x;
	pos.y = pos.y * GetGameSize().height / this->GetDesignSize().y;
	_list_pos_layer_card.push_back(pos);
}

void XiZachScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney)
{
	if (value > 0)
	{
		player->showFlare(true);
	}
}

void XiZachScreen::hideAllFlare()
{
	for (auto p : p_list_player_group)
	{
		p->showFlare(false);
	}
}

void XiZachScreen::onBetRange(int min, s64 & max, s64 maxMoneyRoom)
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
#pragma region Handeler mesage server

void XiZachScreen::ShowQuickPlay(int playerID, bool isQuickPlay)
{
	GameScreen::onSomeOneQuickPlay(playerID, isQuickPlay);
	if (playerID == GameController::myInfo->IDDB && isQuickPlay)
	{
		//canClearCard = false;
		CCLOG("BaiCaoScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay) : btn_quick_play visible : FALSE");
		btn_quick_play->Visible(false);
		//OnShowBetSlider(false);
		if (isQuickPlay
			&& _myInfo
			&& _PLAYER(_myInfo)
			&& _PLAYER(_myInfo)->IDDB != _ownerID)
		{
			btn_bet_money->Visible(false);
		}
	}
}

void XiZachScreen::OnBetMoney(int money, int user)
{
	int seat = GetSeatFromPlayerID(user);
	if (seat == -1 || ls_players_bet_info.size() <= 0)
		return;
	if (user == _ownerID)
	{
		static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->Visible(false);
		return;
	}
	static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->SetText(Utils::formatNUmber_dot(money));
	static_cast<Utility::UI_Widget::TextWidget*>(ls_players_bet_info[seat])->Visible(true);
}

void XiZachScreen::OnStopGame(ubyte numPlayer, vector<vector<int>> cards)
{
	//GameController.Vibrate(1000);
	_isStartGame = false;
	_isGameEnd = true;
	needReset = true;
	ResetReady();
	int seat;
	for (int i = 0; i < numPlayer; i++)
	{
		seat = GetSeatFromPlayerID(cards[i][ 0]);
		vector<char> cardsID(cards[i][ 1]);
		for (int k = 0; k < cardsID.size(); k++)
		{
			cardsID[k] = cards[i] [(k + 2)];
		}
	/*	if (seat == 0)
		{
			
			DoFlipCard(seat, cardsID);
			AddCardToMySeat(cardsID);
			isXiDach = false;
		}
		else
		{
			DoFlipCard(seat, cardsID);
			AddCardToSeat(seat);
		}*/
		OnFlipCard(cards[i][0], cardsID);
	}

	ShowButtonRetrieveCard(false);
	ShowButtonAction(false, false, false, false);
	/*ShowButtonRubCard(false);
	ShowButtonDone(false);*/
	for (int i = 0; i < seats.size(); i++)
	{
		auto pg = GetSeatGroup(i);
		if (pg)
		{
			pg->stopTime();
			if (pg->getPlayer())
			{
				SetScores(pg->getPlayer()->IDDB, _scores[i]);
			}
		}
		/*if (i < _BtnXetBai.Length)
		{
			ShowButtonXetBai(i, false);
		}*/
	}
	/*
	if (actionResetGame != null)
	{
		actionResetGame();
	}
	SetReadyButton(false);
	ShowStartButton();*/
}

void XiZachScreen::OnStartGame(vector<char> myCards, ubyte interval, int user)
{
	stopTimeBar();
	stopCountDownStart();
	resetGame();
	_isStartGame = true;
	_isGameEnd = false;
	ShowButtonSetBet(false);
	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer() && GetSeatFromPlayerID(p->getPlayer()->IDDB) > -1)
		{
			auto _player = p->getPlayer();
			auto current_seat = GetSeatFromPlayerID(_player->IDDB);
			auto bc_player = static_cast<XiZachPlayer*>(_player);
			bc_player->ClearCard();
			if (_player->IDDB == GameController::myInfo->IDDB)
			{
				bc_player->cardValue = myCards;
			}
			DealCardSeat(current_seat);
		}
	}

	ShowButtonAction(false, true, true, false);
	
	OnSetMove(user, interval);
}

void XiZachScreen::SetScores(int user, string des1)
{
	int s = GetSeatFromPlayerID(user);
	if (s < 0)
		return;
	_scores[s] = des1;
	if (_isGameEnd || (whoMove == _ownerID && user != _ownerID))
	{
		if (des1 !="")
		{
			auto w_text = ls_players_card[s]->GetWidgetChildByName(".panel_info.text_info");
			w_text->GetParentWidget()->Visible(true);
			static_cast<TextWidget*>(w_text)->SetText(des1);
		}
	
	}

}

void XiZachScreen::OnSetMove(int playerID, ubyte time)
{
	_isAllowRetrieveCard = true;
	int seatLastMove = GetSeatFromPlayerID(whoMove);
	XiZachPlayer* player = (XiZachPlayer*)getPlayer(playerID);
	if (seatLastMove > -1)
	{
		PlayerGroup* pg = GetSeatGroup(seatLastMove);
		pg->stopTime();

		if (whoMove == GameController::myInfo->IDDB)
		{
			XiZachPlayer* p = (XiZachPlayer*)(pg->getPlayer());
			vector<char> ids;
			for (ubyte c :p->cardValue)
			{
				ids.push_back(c);
			}
			//p->FlipCard(ids);
			ShowScores(0);
		}
	}
	if (playerID != GameController::myInfo->IDDB)
	{
	/*	ShowButtonDone(false);
		ShowButtonRetrieveCard(false);
		ShowButtonRubCard(false);*/
		
		ShowButtonAction(false, false, !GetMe()->IsShowFull(), false);
		ShowButtonRetrieveCard(false);
	}
	int seat = GetSeatFromPlayerID(playerID);
	if (seat > -1)
	{
		auto pg = GetSeatGroup(seat);
		pg->startTime(time, 0);
	}
	if (playerID == GameController::myInfo->IDDB)
	{
		//GameController.Vibrate(1000);
		ShowButtonRetrieveCard(true);
		//isAllowRetrieveCard = true;
		//var listMyCard = GetCardSeat(0);
		//if (listMyCard != null && listMyCard.Length > 0)
		//{
		//	//Nếu còn 1 lá bài úp thì hiển thị nút nặn bài
		//	var isShowButtonRub = true;
		//	for(var c in listMyCard)
		//	{
		//		if (c.id == -1)
		//		{
		//			isShowButtonRub = true;
		//			break;
		//		}
		//		else
		//		{
		//			isShowButtonRub = false;
		//		}
		//	}
		//	ShowButtonRubCard(isShowButtonRub);
		//}
	
		ShowButtonAction(false, true, !player->IsShowFull(), false);
	}
	whoMove = playerID;
	if (whoMove == _ownerID)
	{
		//isOwnerStartMove = true;
		if (GameController::myInfo->IDDB == _ownerID)
		{
			for (auto gr : p_list_player_group)
			{
				XiZachPlayer* p = (XiZachPlayer*)_PLAYER(gr);
				if (p && p->IDDB > 0 && GameController::myInfo->IDDB != p->IDDB)
				{
					int s = GetSeatFromPlayerID(p->IDDB);
					if (s > 0)
					{
						if (p->cards.size() > 0 && p->cards[0] && p->cards[0]->getID() == -1)
						{
							ShowFubgeCard(s, true);
						}
						else
						{
							ShowFubgeCard(s, false);
						}

						
					}
				}
			}
			
		}
	}
}

void XiZachScreen::OnSetMoveOwner(int playerID, vector<char> cardsID)
{
	int seat = GetSeatFromPlayerID(playerID);
	XiZachPlayer* p = (XiZachPlayer*)getPlayer(playerID);
	if (!p)
	{
		
		return;
	}
	p->MoveOwner(cardsID);
	p->cardValue = cardsID;
	if (playerID == _ownerID)
	{
		//lat bai thang cai
		ShowButtonAction(false, true, false, false);
		ShowScores(seat);
		
		
		/*if (!isOwnerStartMove)
		{
			if (playerID != myInfo.IDDB)
			{
				seat = GetSeatFromPlayerID(playerID);
				if (seat > -1)
				{
					AddCardToSeat(seat);
				}
			}
			else
			{
				AddCardToMySeat(cardsID);
			}
		}
		else
		{
			isOwnerStartMove = false;
		}*/
	}
	else
	{
		seat = GetSeatFromPlayerID(playerID);
		if (seat > -1)
		{
			//OnFlipCard(playerID, cardsID);
			
			//SoundManager.Instance.PlayEffect(Sounds.LATBAI);
			ShowScores(seat);
			/*if (ownerID == myInfo.IDDB)
			{
				ShowButtonXetBai(seat, false);
				ShowButtonRubCard(false);
			}*/
			ShowFubgeCard(seat, false);
		}
	}
	if(_ownerID == GameController::myInfo->IDDB)
	{
		_isAllowRetrieveCard = true;
	}
}

void XiZachScreen::HandleAfterInitPlayer()
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

void XiZachScreen::OnReconnect(
	int limit_time, 
	int remain_time, 
	vector<char> myCards, 
	int crMoveID, 
	vector<vector<int>> 
	playerCards, 
	vector<int> 
	playerBetMoneys, 
	int numPlayer,
	bool isPlaying, 
	bool isOwnerTurn)
{

	//GameController.vibrateMedium();
	//GameController.playSound(iWinSound.BUZZ);
	onReconnect();
	int seat = -1;
	if (checkNullCard(myCards)) {
		_isViewer = true;
	}
	if (isPlaying || _isViewer) {

		resetReady();
		hideReadyButton();
		hideStartButton();
		int seatsDeal[] = { 0, 0, 0, 0 };
		for (int i = 0; i < numPlayer; i++) {
			if (playerCards[i][0] > -1) {
				seat = GetSeatFromPlayerID(playerCards[i][0]);
				XiZachPlayer* player = (XiZachPlayer*)getPlayer(playerCards[i][0]);
				if (seat > -1) {
					//player->DealCard(seat, playerCards[i].size());
					//player->cardValue = playerCards[i][2]
					//numPlayerCards[seat] = playerCards[i][2];
					DealCardSeat(seat, playerCards[i][2]);
					seatsDeal[seat] = 1;
				}
				if (playerCards[i][0] == GameController::myInfo->IDDB) {
					XiZachPlayer* player = (XiZachPlayer*)getPlayer(playerCards[i][0]);
					player->cardValue.clear();
					for (int j = 0; j < playerCards[i][2]; j++) {
						player->cardValue.push_back(playerCards[i][3 + j]);
					}
				}
			}
		}
		if (checkNullCard(myCards)) {
			seat = GetSeatFromPlayerID(GameController::myInfo->IDDB);
			seatsDeal[seat] = 0;
		}

		//doDealCards(seatsDeal);

		_isStartGame = true;

		//this.totalTimes = limit_time;
		OnSetMove(crMoveID, (unsigned char)remain_time);
		
		btn_bet_money->SetVisible(false);
		// btnBetAllMoney.setVisible(false);
		//setBottomButtons(true);
		for (int i = 0; i < numPlayer; i++) {
			if (playerCards[i][0] != -1) {
				seat = GetSeatFromPlayerID(playerCards[i][0]);
				if ((isOwnerTurn && playerCards[i][0] == _ownerID) || playerCards[i][1] != -2) {// bai
																							   // chu
																							   // ban
																							   // ,
																							   // va
																							   // den
																							   // luot
																							   // chu
																							   // ban
																							   // hoac
																							   // bai
																							   // da
																							   // bi
																							   // lat
					for (int j = 0; j < playerCards[i][2]; j++) {
						/*dealCards[seat + j * NUM_SEATS].setCardID((byte)playerCards[i][3 + j],
							Card.TYPE_BAICAO_XIZACH_PHOM);*/
						
					}
					if (playerCards[i][1] != -2 && playerCards[i][0] !=GameController::myInfo->IDDB) {
						btn_flop_card->Visible(false);
						//btnRubCard.setVisible(false);
					}
				}
			}
		}
	}
	for (int i = 0; i < numPlayer; i++) {
		seat = GetSeatFromPlayerID(playerCards[i][0]);
		if (seat >= 0) {
			OnBetMoney(playerBetMoneys[i], playerCards[i][0]);
			
			//lblPlayerBetMoney[seat].runTo(playerBetMoneys[i]);
			//this.playerBetMoney[seat] = playerBetMoneys[i];
		}
	}
	if (checkNullCard(myCards)) {
		_isViewer = true;
		btn_quick_play->Visible(false);
		btn_bet_money->Visible(false);
		showViewerLabel();
		
		OnBetMoney(0, GameController::myInfo->IDDB);
		
	}
	
}

void XiZachScreen::OnPlaying(ubyte interval, ubyte numPlayers, vector<vector<int>> cards)
{
	_isViewer = true;
	_isStartGame = true;
	_isGameEnd = false;
	for (int i = 0; i < numPlayers; i++)
	{
		int s = GetSeatFromPlayerID(cards[i][0]);
		if (s <= -1 || p_list_player_group[s]->getPlayer() == nullptr || p_list_player_group[s]->getPlayer()->IDDB == -1) continue;
		if (_isViewer)
		{
			continue;
		}
		DealCardSeat(s);
	}
	_isStartGame = true;
	ShowButtonSetBet(false);

}

void XiZachScreen::OnRetrieveCard(int user, ubyte idCard)
{
	_isAllowRetrieveCard = true;
	XiZachPlayer* p =(XiZachPlayer*) getPlayer(user);
	if (p == nullptr)
		return;
	p->cardValue.push_back(idCard);
	p->AddRetrieve(GameController::myInfo->IDDB == p->IDDB ?-1: idCard);
	


}

void XiZachScreen::ShowButtonRetrieveCard(bool show)
{
	btn_retrieve->SetVisible(show);
	if (show)
	{
		btn_retrieve->GetWidgetChildByName(".img_tick")->SetActionCommandWithName("FOCUS_ACTION");
	}
	else
	{
		NODE(btn_retrieve->GetWidgetChildByName(".img_tick"))->stopAllActions();
	}

}

#pragma endregion
void XiZachScreen::OnFlipCard(int id, std::vector<char> cardID)
{
	XiZachPlayer* p = (XiZachPlayer*)getPlayer(id);
	if (!p)
		return;
	//CCLOG("ACTUAL FLIP BAI CAO id: %d , %d , %d", cardID[0], cardID[1], cardID[2]);
	
	p->FlipCard(cardID);
	if (id == GameController::myInfo->IDDB)
	{
		if (p->IsShowFull())
		{
			flopCard->setVisible(false);
			btn_flop_card->Visible(false);
			int s = GetSeatFromPlayerID(id);
			if (s > 0)
			{
				ShowFubgeCard(s, false);
			}
		}
	}

}

void XiZachScreen::ShowScores(int seat)
{
	if (_scores[seat] != "")
	{
		auto w_text = ls_players_card[seat]->GetWidgetChildByName(".panel_info.text_info");
		w_text->GetParentWidget()->Visible(true);
		static_cast<TextWidget*>(w_text)->SetText(_scores[seat]);
	}
}

void XiZachScreen::ShowFubgeCard(int seat,bool isShow)
{
	if (!ls_players_card[seat])
	{
		return;
	}
	ls_players_card[seat]->GetWidgetChildByName(".contain_card.btn_judge_card")->SetVisible(isShow);
}

void XiZachScreen::HideAllFudge()
{
	for (int i=0;i<4;i++)
	{
		ShowFubgeCard(i, false);
	}
}
bool XiZachScreen::checkNullCard(vector<char> cards) {
	for (int i = 0; i < cards.size(); i++) {
		if (cards[i] != -1) {
			return false;
		}
	}
	return true;
}