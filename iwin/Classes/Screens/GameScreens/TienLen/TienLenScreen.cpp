#include <algorithm>

#include "Screens/GamePlayMgr.h"
#include "AnimUtils.h"
#include "TienLenService.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/RectProcessTime.h"
#include "Screens/Object/PlayerGroup.h"
#include "UI/AdvanceUI/wAnimationWidget.h"

#include "TienLenScreen.h"
#include "TienLenPlayer.h"

#define CARD_SELECT_OP  100
#define TIME_SKIP  5
TienLenScreen* TienLenScreen::getInstance()
{
	if (!GamePlay)
		return nullptr;

	return static_cast<TienLenScreen*>(GamePlay->GetCurrentGameScreen());
}

TienLenScreen::TienLenScreen()
{
	p_current_game_type = GameType::GAMEID_TIENLEN;
	p_rect_process = nullptr;

	SetMaxPlayer(4);
	_interval = 0;
	
	IsAutoSelect = false;
	isClearCurrentCard = false;

	p_btn_skip = nullptr;
	p_btn_fire = nullptr;
	p_btn_sort = nullptr;
	p_text_wait = nullptr;
	_list_fire_card.clear();
	_list_last_card.clear();
	_list_end_card.clear();

	_currentPlayer = -1;
	_currentCardsType = -1;
	_autoSkip = false;
	_selectedCardIDs.clear();

	_forceMove3Bich = false;
	_forceMoveSmallest = false;
	_isFirstMatch = true;
	p_layout_fire_card = nullptr;
	_delta_aligne = Vec2(0, 0);
	_load_card_finish = false;
	_number_card_remain_after_game = 0;
}

TienLenScreen::~TienLenScreen()
{
	CleanLastCard();
	CleanFireCard();
	CleanEndCard();
	if (p_rect_process)
		delete p_rect_process;
	p_rect_process = nullptr;

	CleanMyCard(true);

	//removeChildObject(nullptr, true);
	_list_fire_card.clear();
	_list_last_card.clear();
	_list_end_card.clear();
	_list_pos_swap_group.clear();
}

void TienLenScreen::createUI()
{
	p_rect_process = new RectProcessTime(p_btn_skip);
	
}

void TienLenScreen::CleanMyCard(bool cleanup)
{
	if (getMe() == nullptr || getMe()->cards.size() <= 0)
	{
		return;
	}

	//do-not clear my card
	if (cleanup)
	{
		for (Card *c : getMe()->cards)
		{
			NODE(c)->stopAllActions();
			removeChildObject(NODE(c));
			delete c;
		}
		getMe()->cards.clear();
	}
	else
	{
		for (Card *c : getMe()->cards)
		{
			c->SetVisible(false);
		}
		getMe()->remain_cards.clear();
	}
	
}

void TienLenScreen::CleanFireCard()
{
	for (auto c : _list_fire_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_fire_card.clear();
}

void TienLenScreen::CleanLastCard()
{
	for (auto c : _list_last_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}
	_list_last_card.clear();
}

void TienLenScreen::CleanEndCard()
{
	for (auto c : _list_end_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_end_card.clear();
}

void TienLenScreen::addLastCard(std::vector<Card*> lsNode)
{
	int idx = 0;
	for (Card * c : lsNode)
	{
		NODE(c)->setZOrder(175 + idx);
		c->SetActionCommandWithName("MOVE_CENTER_BACK",nullptr,true);
		_list_last_card.push_back(c);
		idx++;
	}
}

void TienLenScreen::runCardFire(std::vector<unsigned char> cards, int whoMove)
{
	if (whoMove == GameController::myInfo->IDDB)//my self move
	{
		if (getMe() != nullptr &&  getMe()->cards.size() > 0)
		{
			float y =		p_layout_fire_card->GetPosition_BottomLeft().y  + _delta_aligne.y;
			float firstX =	p_layout_fire_card->GetPosition_BottomLeft().x  + _delta_aligne.x;
			float endX =	p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;
			

			std::vector<Card*> list_card_move = CardUtils::getCardId(getMe()->cards, cards);		
			std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, list_card_move.size(), list_card_move[0]->GetSize());

			if (list_pos_move_to.size() != list_card_move.size())
			{
				PASSERT2(false, "wrong here!");
			}	
			int idx = 0;
			int size_list_card_move = list_card_move.size();
			for (Card* c : list_card_move)
			{
				getMe()->cards.erase(std::remove(
					getMe()->cards.begin(),
					getMe()->cards.end(), c), 
					getMe()->cards.end());

				NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
				NODE(c)->setZOrder(250 + idx);

				AnimUtils::RunEffectMoveToFire(c, c->GetPosition(), list_pos_move_to[idx], CallFunc::create([c , idx , size_list_card_move, this]() {
					c->EventActionFinish();
					if (idx == 0)
					{
						if (size_list_card_move < 2)
						{
							auto anim = this->p_animate_1;
							this->p_animate_2->Visible(false);
							anim->Visible(true);
							anim->SetPosition(c->GetPosition());
							static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
						}
						else
						{
							auto anim = this->p_animate_2;
							this->p_animate_1->Visible(false);
							anim->Visible(true);
							anim->SetPosition(c->GetPosition());
							static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
						}
					}
				}));
				_list_fire_card.push_back(c);		
				idx++;
			}
			layoutMyCard();
		}
	}
	else
	{
		float y = p_layout_fire_card->GetPosition_BottomLeft().y      + _delta_aligne.y;
		float firstX = p_layout_fire_card->GetPosition_BottomLeft().x + _delta_aligne.x;
		float endX = p_layout_fire_card->GetPosition_BottomRight().x  + _delta_aligne.x;
		int idx = 0;
		for (auto id : cards)
		{
			Card* c = Card::NewInstance(id, DECK_TYPE::DECK_TIENLEN);
			addChildObject(NODE(c), 250 + idx);
			NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
			_list_fire_card.push_back(c);
			idx++;
		}

		auto seat_id = GetSeatFromPlayerID(whoMove);
		if (seat_id > -1)
		{
			auto pl = GetPlayerGroupAtIdx(seat_id);
			if (pl)
			{
				WidgetEntity* userG = _WIDGET(pl);
				PASSERT2(userG != nullptr, "userG is nullptr");
				if (userG && _list_fire_card.size() > 0)
				{
					std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, _list_fire_card.size(), _list_fire_card[0]->GetSize());
					idx = 0;

					int size_list_card_move = _list_fire_card.size();
					for (Card* c : _list_fire_card)
					{
						AnimUtils::RunEffectMoveToFire(c, userG->GetPosition_Middle(), list_pos_move_to[idx], CallFunc::create([c, idx, size_list_card_move, this]() {
							c->EventActionFinish();
							if (idx == 0)
							{
								if (size_list_card_move < 2)
								{
									auto anim = this->p_animate_1;
									this->p_animate_2->Visible(false);
									anim->Visible(true);
									anim->SetPosition(c->GetPosition());
									static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
								}
								else
								{
									auto anim = this->p_animate_2;
									this->p_animate_1->Visible(false);
									anim->Visible(true);
									anim->SetPosition(c->GetPosition());
									static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
								}
							}
						}));
						idx++;
					}
				}
			}
		}
	}
}

void TienLenScreen::OnCardTouchEnd()
{
	if (!getMe())
		return;

	int numAciton = 0;
	vector<unsigned char> lsCardSelects;
	for (Card *c : getMe()->cards){
		if (NODE(c)->getTag() == 1){
			c->SelectCardAction(!c->isSelected());
		}
		if (c->isSelected()){
			lsCardSelects.push_back(c->getID());
			numAciton++;
		}
		c->showHighlight(false);
		NODE(c)->setTag(0);
	}

	if (!getMe() || _currentPlayer != GameController::myInfo->IDDB || !IsAutoSelect){
		return;
	}

	if (GameController::gameId != GameType::GAMEID_TIENLEN_MB)
	{
		vector<unsigned char> selectAuto;
		if (numAciton == 1){
			selectAuto = CardUtils::autoSelecWhenTouchLTMN(getMe(), lsCardSelects, getcardFire());
		}
		else if (numAciton > 1){
			selectAuto = CardUtils::autoSelecWhenTouchLTMN(getMe(), lsCardSelects, getcardFire());
		}

		if (selectAuto.size() >0){
			for(Card* c :getMe()->cards){
				if (NODE(c)->getTag() == 1){
					c->SelectCardAction(false);
					NODE(c)->setTag(0);
				}
			}
			for (auto s :  selectAuto){
				for (Card* c : getMe()->cards){
					if (!c->isSelected()){
						if (s == c->getID()){
							c->SelectCardAction(true);
							break;
						}
					}
				}

			}
			IsAutoSelect = false;
		}

	}

}

void TienLenScreen::OnCardTouchDown()
{
	if (!getMe())
		return;
	for (Card *c : getMe()->cards){
		if (InputMgr->IsPressOnBound(c->GetBound()) && c->IsVisible()){
			c->showHighlight(true);
			NODE(c)->setTag(1);
			return;
		}
	}
}

void TienLenScreen::OnCardTouchMove()
{
	if (!getMe())
		return;
	for (Card* c : getMe()->cards){
		if (InputMgr->IsPressOnBound(c->GetBound()) && NODE(c)->getTag() != 1 && c->IsVisible()) {
			c->showHighlight(true);
			NODE(c)->setTag(1);
		}	
	}
}


std::vector<unsigned char> TienLenScreen::doAutoSelect(bool auto_skip)
{
	vector<unsigned char> curentFire = getcardFire();
	std::vector<unsigned char> foundAt = CardUtils::autoSelect(curentFire, getMe());

	if (foundAt.size() <= 0 && GameController::myInfo->IDDB == _currentPlayer)
	{
		// ko chon duoc la nao thi se hien bo luot
		_currentPlayer = -1;
		_forceMoveSmallest = false;
		auto w_panel = p_btn_skip->GetWidgetChildByName(".eat_card_panel");
		if (!p_rect_process->isShowTimeAction()) {
			p_rect_process->startTime(TIME_SKIP, ([this , w_panel]() {
				w_panel->Visible(false);
				this->p_rect_process->stop();
				TienLenService::skip(_roomID, _boardID);
			}));
		}

		if (auto_skip)
		{		
			w_panel->Visible(true);
			w_panel->SetActionCommandWithName("FOCUS_ACTION");
		}
		/*else
		{
			ActionSkip(L.Get(1055) + ". " + L.Get(90));
		}*/
	}

	/*if (!notify)
	{
		for (var i = 0; i < GetMe().cards.Count; i++)
		{
			CardUtil.SetSelect(GetMe().cards[i], false);
		}
	}*/
	return foundAt;
}

std::vector<unsigned char> TienLenScreen::getcardFire()
{
	vector<unsigned char> curentFire;

	for (auto c : _list_fire_card)
	{
		curentFire.push_back(c->getID());
	}
	return curentFire;
}

bool TienLenScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);

	XMLMgr->OnLoadXMLData<TienLenScreen>("TLMN_Screen", std::mem_fn(&TienLenScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("TLMN_Screen");

	auto panel_custom_menu = GetWidgetChildByName("HUD_screen.panel_custom_menu");
	p_panel_bottom = panel_custom_menu->GetWidgetChildByName(".back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");
	
	auto lay_btn_tl = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_tlmn");
	p_btn_fire = lay_btn_tl->GetWidgetChildByName(".btn_deal_turn");
	p_btn_skip = lay_btn_tl->GetWidgetChildByName(".btn_leave_turn");
	p_btn_sort = lay_btn_tl->GetWidgetChildByName(".btn_quick_choose_turn");
	p_text_wait = lay_btn_tl->GetWidgetChildByName(".txt_wait_player");
	p_text_wait->Visible(true);
	p_text_wait->SetActionCommandWithName("ACTION");

	p_layout_fire_card = panel_custom_menu->GetWidgetChildByName(".layout_fire_card");
	p_animate_1 = panel_custom_menu->GetWidgetChildByName(".animate_effect_1");
	p_animate_2 = panel_custom_menu->GetWidgetChildByName(".animate_effect_2");

	createUI();
	return true;
}

void TienLenScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_leave_turn")
	{
		if (p_rect_process&& p_rect_process->isShowTimeAction())
		{
			doSkip();
			p_btn_skip->GetWidgetChildByName(".eat_card_panel")->Visible(false);
			p_rect_process->stop();
		}
		else
		{
			p_rect_process->startTime(TIME_SKIP);
		}
		
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_deal_turn")
	{
		doFireWork();
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_quick_choose_turn")
	{
		auto list_card = doAutoSelect();

		int idx = 0;
		for (auto c : getMe()->cards)
		{
			for (auto l : list_card)
			{
				if (idx == l)
				{
					c->SelectCardAction(true);
					break;
				}
				else
				{
					c->SelectCardAction(false);
				}
			}
			idx++;
		}
	}
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

int TienLenScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (!_load_card_finish)
	{
		if(_list_card_create.size() > 0)
		{
			Card * c = Card::NewInstance(_list_card_create[0], DECK_TYPE::DECK_TIENLEN);
			c->SetVisible(false);
			getMe()->cards.push_back(c);
			_list_card_create.erase(_list_card_create.begin());
			if (_list_card_create.size() == 0 && _number_card_remain_after_game <= 0)
			{
				_load_card_finish = true;
				p_is_finish_load_player = true;
			}
		}
		else if (_list_card_create.size() <= 0)
		{
			if (_number_card_remain_after_game > 0)
			{
				int idx = _number_card_remain_after_game - 1;
				(getMe()->cards[idx])->ResetID(-1, DECK_TYPE::DECK_TIENLEN);
				getMe()->cards[idx]->SetVisible(false);
				_number_card_remain_after_game--;
			}
			else
			{
				_load_card_finish = true;
			}
		}
		return 0;
	}

	if (p_is_finish_load_player)
	{
		//handle msg return after init player group
		if (p_list_msg_return.size() > 0)
		{
			OnHandleMsg(p_list_msg_return[0]);
			delete p_list_msg_return[0];
			p_list_msg_return.erase(p_list_msg_return.begin());
		}
	}
	else
	{
		return 0;
	}

	if (getMe())
	{
		for (auto c : getMe()->cards) {
			c->Update(dt);
		}
	}

	for (auto c : _list_fire_card) {
		c->Update(dt);
	}

	for (auto c : _list_last_card) {
		c->Update(dt);
	}

	for (auto c : _list_end_card) {
		c->Update(dt);
	}

	return 1;
}

void TienLenScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//insert into custom menu
	auto p_panel_custom = GetWidgetChildByName("HUD_screen.panel_custom_menu");
	//
	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("layout_fire_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_1"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_2"), p_panel_custom, "panel_custom_menu");

	for (auto it : p_list_sample_dec)
	{
		delete it.second;
	}
	p_list_sample_dec.clear();
}

/// để tạo cho thằng choi nó ngồi ấy mà
void TienLenScreen::resetSeat()
{

}

void TienLenScreen::start(int whoMoveFirst, char interval, std::vector<unsigned char> myCards)
{
	OnHideCountDown();
	resetGame();
	//GameController.Vibrate(VIBRATE_STARTGAME_TIME);
	//RemoveAllImageFinish();
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();
	ResetReady();
	ResetAllSkip();
	//ShowAllUserGroupOpacity();

	//TienLenModel.Instance.ShowHeighBetMoney(false);

	_isStartGame = true;
	_forceMove3Bich = false;

	int idx = 0;
	PASSERT2(getMe()->cards.size() > 0, "my list card must be init first");
	getMe()->remain_cards = myCards;
	for (auto mc : myCards){
		Card * c = getMe()->cards[idx];
		c->ResetID(mc, DECK_TYPE::DECK_TIENLEN);
		c->SetVisible(true);
		addChildObject(NODE(c), 50 + idx);
		NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
		c->SetStatus(false);
		idx++;
	}
	_delta_aligne = getMe()->cards[0]->GetSize() / 2.f;
	CardUtils::sortByOrder(getMe()->cards);

	dealCards();

	if (_isFirstMatch 
		&& whoMoveFirst == GameController::myInfo->IDDB 
		&& getMe()->cards[0]->getID() == 0)
	{
		_forceMove3Bich = true;
	}
	this->_interval = interval;
	_currentPlayer = whoMoveFirst;

	//layoutMyCard();

	stopTimeBar();
	PlayerGroup* pg = getUserGroup(whoMoveFirst);
	if (pg != nullptr)
	{
		pg->startTime(interval, 0);
	}
	if (myCards.size() > 0)
	{
		if (_currentPlayer == GameController::myInfo->IDDB)
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, true);
			EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, false);
			IsAutoSelect = true;
		}
		else
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		}
	}

	pg->RunTurnEffect();

}

TienLenPlayer* TienLenScreen::getMe()
{
	return static_cast<TienLenPlayer*>(getMyPlayer());
}

void TienLenScreen::resetGame()
{
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();

	EnableButton(BTN_TIENLEN::BTN_COUNT, false);

	stopTimeBar();
	IsAutoSelect = false;
	stopAllTimePlayer();
	hideAllRank();
}

void TienLenScreen::layoutMyCard()
{
	if (!getMe() || getMe()->cards.size() <= 0)
		return;

	auto w_avatar = _WIDGET(_myInfo);
	auto size_avatar = w_avatar->GetWidgetChildByName(".layout_player.bg_avatar")->GetSize();
	int idx = 0;
	auto list_pos = CardUtils::AligneCardPosition(
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2)+ 5 + _delta_aligne.x,
		p_panel_bottom->GetSize().y + 5 + _delta_aligne.y,
		GetGameSize().width - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	for (Card* c : getMe()->cards)
	{
		NODE(c)->setAnchorPoint(Vec2(0.5,0.5));
		AnimUtils::RunEffectLayout(c, list_pos[idx] , nullptr);
		idx++;
	}


}

void TienLenScreen::Move(int whoMove, std::vector< unsigned char> cards, int nextMove)
{
	/*
	if (_list_last_card.size() > 0){
		CleanLastCard();
	}*/
	int seat = GetSeatFromPlayerID(whoMove);
	CCLOG("MOVE: who move skip id : %d", whoMove);
	seat = GetSeatFromPlayerID(nextMove);
	CCLOG("MOVE: nextMove skip id : %d", nextMove);

	if (_interval <= 0){
		_interval = 20;
	}

	if (_list_fire_card.size() > 0){
		CleanLastCard();
		addLastCard(_list_fire_card);
		_list_fire_card.clear();
	}

	if (_myInfo && whoMove == _myInfo->getPlayer()->IDDB){
	//	layoutMyCard();
		EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		//StopTimeBar();
		OnHideCountDown();
	}
	else
	{
		int p = GetSeatFromPlayerID(whoMove);
	}

	if (whoMove == nextMove)
	{
		ResetAllSkip();
	}
	
	if (_myInfo)
	{
		runCardFire(cards, whoMove);
	}
	//ShowLastCard();
	//ShowFireCard(false);
	_forceMove3Bich = false;
	_currentPlayer = nextMove;
	//SetCurrentCards(fireCards);

	PlayerGroup* group = getUserGroup(whoMove);
	if (group){
		group->stopTime();
	}

	auto groupNext = getUserGroup(nextMove);
	if (groupNext){
		groupNext->startTime((_interval == 0 ? 30 : _interval), 0);
		groupNext->RunTurnEffect();
	}

	if (_myInfo && nextMove == _myInfo->getPlayer()->IDDB)
	{
		EnableButton(BTN_TIENLEN::BTN_COUNT, true);
		//GameController.Vibrate(1000);
		 IsAutoSelect = true;

		 auto have_card_available = doAutoSelect(true);
		if (have_card_available.size() <= 0)
		{
			_autoSkip = true;
			EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, true);
			EnableButton(BTN_TIENLEN::BTN_SORT_CARD, false);
			EnableButton(BTN_TIENLEN::BTN_FIRE_CARD, false);
		}
	}
	else
	{
		if (_myInfo && whoMove == _myInfo->getPlayer()->IDDB)
		{
			stopTimeBar();
			if (_isStartGame)
			{
				EnableButton(BTN_TIENLEN::BTN_COUNT, false);
			}
		}
	}

	//HideStartButton();
	//ShowButtonSoftCard(cards != null && cards.Count > 2);
	IsAutoSelect = true;
}

void TienLenScreen::doFireWork()
{
	_selectedCardIDs.clear();
	for (Card* c : getMe()->cards){
		if (c->isSelected()){
			_selectedCardIDs.push_back((char)c->getID());
		}
	}
	TienLenService::move(_roomID, _boardID, _selectedCardIDs);


}

void TienLenScreen::doSkip(){
	TienLenService::skip(_roomID, _boardID);
}

void TienLenScreen::skip(int whoSkip, int nextMove, bool clearCard)
{
	_currentPlayer = nextMove;
	if (clearCard)
	{
		if (whoSkip == GameController::myInfo->IDDB )
		{
			this->isClearCurrentCard = clearCard;

		}
		else
		{
			stopTimeBar();
		}
		CleanLastCard();
		CleanFireCard();
		ResetReady();
		ResetAllSkip();
	}
	
	int seat = GetSeatFromPlayerID(whoSkip);
	CCLOG("SKIP: skip seat id : %d", seat);
	PlayerGroup* pg = GetSeatGroup(seat);
	if (pg && !clearCard)
	{
		pg->stopTime();
		pg->setIsDenyTurn(true);
	}
	seat = GetSeatFromPlayerID(nextMove);

	CCLOG("SKIP: next move seat id : %d", seat);

	PlayerGroup* pgnext = GetSeatGroup(seat);
	if (pgnext)
	{
		pgnext->startTime(_interval, 0);
	}
	if (seat == 0)
	{
		stopTimeBar();
	}
	if (_isStartGame && getMe() && getMe()->cards.size() > 0)
	{
		if (_currentPlayer == GameController::myInfo->IDDB)
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, true);
			
			if(clearCard && nextMove == GameController::myInfo->IDDB)
			{
				EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, false);
			}
		}
		else
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		}
	}
	if (whoSkip == GameController::myInfo->IDDB)
	{
		if (getMe())
		{
			getMe()->unSelectAllCards();
		}
		//Nếu đang đếm ngược bỏ lượt thì stop
		//if (isCountingSkip)
		//{
		//	OnHideCountDown();
		//}
	}
}

void TienLenScreen::onGetTimeLeftAutoStart(char timeStart)
{
	OnShowCountDown(timeStart, 0);

	if (getNumberPlayer() <= 1)
	{
		p_text_wait->Visible(true);
		p_text_wait->SetActionCommandWithName("ACTION");
	}
	else
	{
		p_text_wait->Visible(false);
		p_text_wait->ForceFinishAction();
	}
}

void TienLenScreen::Finish(int whoFinish, char finishPos, int money, int exp)
{
	//ShowContentChat(whoFinish, L.Get(359) + " " + (finishPosition + 1));
	
	if (whoFinish == GameController::myInfo->IDDB)
	{
		EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		//reset all card , only call once
		_list_card_create.clear();
		_number_card_remain_after_game = getMe()->cards.size();
		for (int i = 0; i < NUMBER_CARD_TL; i++)
		{
			if (i < _number_card_remain_after_game)
			{
				auto c = getMe()->cards[i];
				NODE(c)->stopAllActions();
				removeChildObject(NODE(c));
			}
			else
			{
				_list_card_create.push_back(-1);
			}
		}
		_load_card_finish = false;
		_number_card_remain_after_game = getMe()->cards.size();
	}
	int s = GetSeatFromPlayerID(whoFinish);
	if (s >= 0 && GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
	{
		p_list_player_group[s]->setRankNumber(finishPos);
		p_list_player_group[s]->SetTextChat( "Thắng"  + std::to_string(finishPos + 1));
	}
	//RemoveAllImageFinish();

	if (finishPos == 0)
	{
		GetSound->PlaySoundEffect("SFX_VOTAY");
	}

	EnableButton(BTN_TIENLEN::BTN_SORT_CARD, false);
}

void TienLenScreen::ForceFinish()
{

}

void TienLenScreen::StopGame(int whoShow, std::vector<char> cardsShow)
{
	/*if (GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
	{
		ShowStartButton();
	}*/
	needReset = true;
	if (whoShow != -1)
	{
		CleanLastCard();
		CleanMyCard();

		float y = p_layout_fire_card->GetPosition_BottomLeft().y ;
		float y1 = p_layout_fire_card->GetPosition_TopLeft().y ;
		float firstX = p_layout_fire_card->GetPosition_BottomLeft().x + _delta_aligne.x;
		float endX = p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;

		int idx = 0;
		for (auto s : cardsShow)
		{
			Card* c = Card::NewInstance(s, DECK_TYPE::DECK_TIENLEN);
			_list_end_card.push_back(c);
			idx++;
		}
		idx = 0;
		std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, _list_end_card.size(), _list_end_card[0]->GetSize());
		for (auto c : _list_end_card)
		{
			addChildObject(NODE(c), 250 + idx);
			c->SetPosition(list_pos_move_to[idx]);
			idx++;
		}
		idx = 0;
		auto list_pos_move_to1 = CardUtils::AligneCardPosition(firstX, y1, endX, _list_fire_card.size(), _list_fire_card[0]->GetSize());
		for (auto c : _list_fire_card)
		{
			Card* c1 = Card::NewInstance(c->getID(), DECK_TYPE::DECK_TIENLEN);
			addChildObject(NODE(c1), 150 + idx);
			c1->SetPosition(list_pos_move_to1[idx]);
			_list_end_card.push_back(c1);
			idx++;
		}

		CleanFireCard();
	}

	OnHideCountDown();
	EnableButton(BTN_TIENLEN::BTN_COUNT, false);
	ResetReady();
	_isGameEnd = true;
	_isFirstMatch = false;
	_isStartGame = false;
	//TienLenModel.Instance.ShowHeighBetMoney(true);
	stopAllTimePlayer();

}

void TienLenScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	if (_list_pos_swap_group.size()<=0)
	{
		_list_pos_swap_group.push_back(_WIDGET(p_list_player_group[1])->GetPosition());
		_list_pos_swap_group.push_back(_WIDGET(p_list_player_group[2])->GetPosition());
	}
	if (GameController::gameId == GameType::GAMEID_TIENLEN_SOLO)
	{
		p_list_player_group[2]->setVisible(false);
		p_list_player_group[3]->setVisible(false);
		p_list_player_group[1]->setPosition(_list_pos_swap_group[1]);
	}
	else
	{
		p_list_player_group[2]->setVisible(true);
		p_list_player_group[3]->setVisible(true);
		p_list_player_group[1]->setPosition(_list_pos_swap_group[0]);
	}
	for (int i = 0; i < NUMBER_CARD_TL; i++)
	{
		_list_card_create.push_back(-1);
	}
	_load_card_finish = false;
}

void TienLenScreen::OnReconnect(
	int interval, 
	int playerId, 
	std::vector<unsigned char> movedCards, 
	int next_user_id, 
	std::vector<unsigned char> myCards, 
	int timeLimit, 
	std::vector<int> userRank, 
	std::vector<unsigned char> posRank, 
	std::vector<bool> userSkip)
{

	onReconnect();
	_interval = interval;
	_isStartGame = true;
	_delta_aligne = getMe()->cards[0]->GetSize() / 2.f;
	
	int delta_card = getMe()->cards.size() - myCards.size();
	PASSERT2(delta_card >= 0, "size 2 list error!");
	for (int i = 0; i < (int)myCards.size(); i++)
	{
		auto c = getMe()->cards[i];
		addChildObject(NODE(c));
		c->ResetID(myCards[i], DECK_TYPE::DECK_TIENLEN);
		c->SetVisible(true);
	}
	if (delta_card > 0)
	{
		for (auto i = myCards.size(); i < getMe()->cards.size(); i++)
		{
			auto c = getMe()->cards[i];
			NODE(c)->stopAllActions();
			removeChildObject(NODE(c));
			delete c;
		}
		getMe()->cards.erase(getMe()->cards.begin() + myCards.size(), getMe()->cards.begin() + myCards.size() + delta_card);
	}


	Move(playerId, movedCards, next_user_id);
	layoutMyCard();
	PlayerGroup* pg = getUserGroup(_currentPlayer);
	if (pg)
	{
		pg->startTime(interval, timeLimit);
	}
	
	// 1 ai do ve truoc
	for (auto p : p_list_player_group)
	{
		if (!(p 
			&& _PLAYER(p) 
			&& _PLAYER(p)->IDDB >= 0))
		{
			continue;
		}

		auto tl_p = _PLAYER(p);

		TienLenPlayer* ptl = static_cast<TienLenPlayer*>(tl_p);
		if (ptl->IDDB != -1)
		{
			for (size_t i = 0; i < userRank.size(); i++)
			{
				if (ptl->IDDB == userRank[i] && userRank[i] != -1)
				{
					ptl->finishPosition = posRank[i];
					//SetFinishStatus(GetSeatFromPlayerID(p->IDDB), p->finishPosition);
					ptl->isReady = false;
					//p->bSkip = userSkip[j];
					//finish[i] = userRank[j];
					break;
				}
			}
		}
	}
	if (getMe()->cards.size() > 0)
	{
		EnableButton(BTN_TIENLEN::BTN_COUNT, _currentPlayer == GameController::myInfo->IDDB);
	}
	//ShowButtonSoftCard(true);

	p_text_wait->Visible(false);
	p_text_wait->ForceFinishAction();
}

void TienLenScreen::EnableButton(BTN_TIENLEN btn_type, bool value)
{
	if (btn_type == BTN_TIENLEN::BTN_FIRE_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		p_btn_fire->Visible(value);
	}

	if (btn_type == BTN_TIENLEN::BTN_SORT_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		p_btn_sort->Visible(value);
	}

	if (btn_type == BTN_TIENLEN::BTN_SKIP_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		if (btn_type == BTN_TIENLEN::BTN_SKIP_CARD && value == false)
		{
			CCLOG("btn skip is hide!");
		}
		p_btn_skip->Visible(value);
		if (!value && p_rect_process)
		{
			p_btn_skip->GetWidgetChildByName(".eat_card_panel")->Visible(false);
			p_rect_process->stop();
		}
	}

}

void TienLenScreen::dealCards()
{
	std::vector<cocos2d::Rect> lsPointOther;
	std::vector<Vec2>		list_end_point;
	for (PlayerGroup* pg : p_list_player_group)
	{
		if (pg != nullptr && 
			pg->getPlayer() != nullptr && 
			pg->getPlayer()->IDDB > 0 &&
			pg->getPlayer()->IDDB != GameController::myInfo->IDDB)
		{
			lsPointOther.push_back(pg->GetRectFront());
			list_end_point.push_back(_WIDGET(pg)->GetPosition_Middle());
		}
	}

	int idx = 0;
	float delay_time = 0.1f;
	auto w_avatar = _WIDGET(_myInfo);
	auto size_avatar = w_avatar->GetWidgetChildByName(".layout_player.bg_avatar")->GetSize();
	auto list_pos_move_to = CardUtils::AligneCardPosition(
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2) + 5  + _delta_aligne.x,
		p_panel_bottom->GetSize().y		   + 5  + _delta_aligne.y,
		GetGameSize().width				   - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	PASSERT2(getMe()->cards.size() == list_pos_move_to.size(), "size of 2 list must be equal");


	std::map<int , std::vector<Vec2> > list_move_other;
	for (size_t i = 0 ; i < lsPointOther.size() ; i ++)
	{
		auto recta = lsPointOther[i];
		cocos2d::Size size_rect = recta.size;
		cocos2d::Vec2 origin_rect = recta.origin;
		bool isAligneX = (size_rect.width > size_rect.height);
		if (isAligneX)
		{
			auto list_pos_move_other = CardUtils::AligneCardPosition(
				origin_rect.x + _delta_aligne.x,
				origin_rect.y - (size_rect.height * getMe()->cards[0]->GetSize().y),
				size_rect.width + _delta_aligne.x,
				getMe()->cards.size(), getMe()->cards[0]->GetSize(), true);
			list_move_other.insert({ i ,list_pos_move_other });
		}
		else
		{
			auto list_pos_move_other = CardUtils::AligneCardPosition(
				origin_rect.y,
				origin_rect.x - (size_rect.width * getMe()->cards[0]->GetSize().x),
				size_rect.height,
				getMe()->cards.size(), getMe()->cards[0]->GetSize(), false);
			list_move_other.insert({ i ,list_pos_move_other });
				
		}
	}

	for (Card * c : getMe()->cards)
	{
		c->SetStatus(false);
		AnimUtils::RunEffectChiaBai(c , list_pos_move_to[idx] , delay_time * (idx + 1) , CallFunc::create([this, c]() 
		{
			if (this->getMe()->cards.size() > 0)
			{
				c->EventActionFinish();
				c->SetStatus(true);
			}
		}));

		for (size_t i = 0; i < lsPointOther.size(); i++)
		{
			Card* cOther = Card::NewInstance(-2, DECK_TYPE::DECK_TIENLEN);
			addChildObject(NODE(cOther), 50);

			AnimUtils::RunEffectChiaBai1(
				cOther, 
				list_move_other.at(i)[idx], 
				delay_time * (idx + 1),
				list_end_point[i],
				delay_time * (getMe()->cards.size() - idx), CallFunc::create([cOther, this]()
			{
				cOther->EventActionFinish();
				this->removeChildObject(NODE(cOther));
				cOther->release();
			}));
		}
		idx++;
	}
}

void TienLenScreen::hideAllRank()
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p ) 
		{
			if (p->getPlayer() != nullptr && p->GetIDDB() != -1)
			{
				p->HideRank();
			}
			else
			{
				p->ShowLayerPlayer(false);
			}
		}
	}
}

void TienLenScreen::ResetReady()
{
	GameScreen::ResetReady();
}

void TienLenScreen::ResetAllSkip()
{
	for (auto g : p_list_player_group)
	{
		g->stopTime();
		g->setIsDenyTurn(false);
	}
}
void TienLenScreen::onSomeOneJoinBoard(int seat, Player * p)
{
	GameScreen::onSomeOneJoinBoard(seat, p);
	if (p_text_wait->Visible())
	{
		p_text_wait->Visible(false);
		p_text_wait->ForceFinishAction();
	}
}
void TienLenScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
	if (getNumberPlayer() <= 1)
	{
		p_text_wait->Visible(true);
		p_text_wait->SetActionCommandWithName("ACTION");
	}
}