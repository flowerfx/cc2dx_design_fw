
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Game/GameLogicHandler.h"
#include "Network/Game/GameMessageHandler.h"
#include "Network/Game/GameService.h"
#include "Network/Global/GlobalService.h"
#include "Common/FlyMoney.h"
#include "Constants.h"

#include "Screens/Object/ObjectEntity.h"
#include "Screens/Object/TimeBarProcess.h"
#include "Screens/Object/PlayerGroup.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "GameScreens.h"

GameScreen::GameScreen()
{
	p_current_game_type = GameType::NULL_TYPE;
	p_max_player = GameController::getMaxPlayerOf(GameController::gameId);

	vector<int> ls1;
	ls1.resize(5);
	ls1.clear();
	vector<int> ls2(4,2);
	btn_bet_iwin = nullptr;
	p_text_money = nullptr;
	_myInfo = nullptr;

	_isStartGame = false;
	_isGameEnd = false;

	_roomID = -1;
	_boardID = -1;
	_isViewer = true;

	p_enable_touch = true;

	p_thread_load_xml_player_dec = nullptr;
	p_load_xml_state = 0;
	p_is_finish_load_player = false;
	p_list_msg_return.clear();
}

GameScreen::~GameScreen()
{
	seats.clear();
	//weak pointer cannot delete
	p_panel_game_screen_resource = nullptr;
	_myInfo = nullptr;
	if (p_timebar_process)
	{
		delete p_timebar_process;
		p_timebar_process = nullptr;
	}

	if (p_thread_load_xml_player_dec)
	{
		delete p_thread_load_xml_player_dec;
		p_thread_load_xml_player_dec = nullptr;
	}

	//weak point not delete
	p_thread_player_infos.clear();
	//free msg temp
	for (auto msg : p_list_msg_return)
	{
		delete msg;
	}
	p_list_msg_return.clear();
	p_is_finish_load_player = false;
}


bool GameScreen::InitGame(int game_id)
{
	HUDScreen::Init();

	CallInitComponent(false);
	return true;
}

int	GameScreen::InitComponent()
{
	HUDScreen::InitComponent();
	p_panel_game_screen_resource = GetWidgetChildByName("HUD_screen.panel_custom_menu")->GetResource();

	auto panel_timebar = GetWidgetChildByName("HUD_screen.progress_bar_panel");
	p_timebar_process = new TimeBarProcess(panel_timebar);
	p_timebar_process->setVisible(false);

	btn_bet_iwin = GetWidgetChildByName("HUD_screen.top_right_layout.btn_bet_iwin");
	p_panel_game_screen = GetWidgetChildByName("HUD_screen.panel_game_screen");

	p_text_min_bet = p_panel_game_screen->GetWidgetChildByName(".min_bet");
	p_text_gamename = p_panel_game_screen->GetWidgetChildByName(".game_name");
	p_text_other1 = p_panel_game_screen->GetWidgetChildByName(".other_text");
	p_text_name = GetWidgetChildByName("HUD_screen.user_name");

	p_bonus_effect = GetWidgetChildByName("HUD_screen.animate_effect_bonus");

	return 1;
}

int GameScreen::Update(float dt)
{
	if (HUDScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_timebar_process && p_timebar_process->getVisible()) 
	{
		p_timebar_process->update(dt);
	}

	OnHandleTouchUpdate(dt);

	if (p_load_xml_state > 0 && p_load_xml_state <= p_max_player)
	{
		parse_xml_player(p_load_xml_state - 1);
		p_load_xml_state++;
		if (p_load_xml_state > p_max_player) {
			HandleAfterInitPlayer();
		}
	}

	return 1;
}

GameType GameScreen::CurrentGameType()
{
	return p_current_game_type;
}

void GameScreen::addChildObject(Node * child, int z_order)
{
	p_panel_game_screen_resource->addChild(child, z_order);
}

void GameScreen::removeChildObject(Node * child, bool isAll)
{
	if (!isAll)
	{
		p_panel_game_screen_resource->removeChild(child, true);
	}
	else
	{
		p_panel_game_screen_resource->removeAllChildrenWithCleanup(true);
	}
}

void GameScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	//abstract class not implement this function
}

bool GameScreen::SetMyInfoToPanel(RKString name, u64 number_money, int type_money)
{
	static_cast<TextWidget*>(GetWidgetChildByName("HUD_screen.name_title"))->SetText(name, true);

	auto btn_iwin = GetWidgetChildByName("HUD_screen.btn_purchase_win");
	auto btn_ruby = GetWidgetChildByName("HUD_screen.btn_purchase_ruby");
	if (type_money == 0) // iwin
	{
		btn_iwin->Visible(true);
		btn_ruby->Visible(false);
		p_text_money = btn_iwin->GetWidgetChildByName(".title_");
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNUmber_dot(number_money), true);
	}
	else //ruby
	{
		btn_iwin->Visible(false);
		btn_ruby->Visible(true);
		p_text_money = btn_ruby->GetWidgetChildByName(".title_");
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNUmber_dot(number_money), true);
	}
	CCLOG("call GameScreen::SetMyInfoToPanel and set p_text_money");
	return true;
}

std::map<RKString, xml::UILayerWidgetDec*> GameScreen::LoadWidgetCustom(TiXmlDocument * p_objectXML)
{
	std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec;
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return p_list_sample_dec;
	}

	auto sample_ui_dec = Root->FirstChildElement("MenuWidget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML fail to load!");
		return p_list_sample_dec;
	}
	Vec2 design_size = XMLMgr->ConvertToVector2(sample_ui_dec->Attribute("designSize"));

	//read xml and parse into data
	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget, _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

	return p_list_sample_dec;
}

void GameScreen::ParseIntoCustom(xml::UILayerWidgetDec* xml_dec, WidgetEntity * p_panel_custom, RKString name_panel_xml)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	auto layer_root = GetLayerChildByName("HUD_screen");

	xml_dec->p_menu_widget = menu_widget_xml;
	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen")->GetChildWidgetByName(name_panel_xml.GetString());

	auto widget = layer_root->InitWidgetEntity(xml_dec, layer_root, p_panel_custom);
	p_panel_custom->ResizeAndPosChildWidget(widget);

	widget->SetMenuParent(p_panel_custom->GetMenuParent());
	widget->SetParentWidget(p_panel_custom);
	widget->SetParentLayer(p_panel_custom->GetParentLayer());
	p_panel_custom->InsertChildWidget(widget);

}

void GameScreen::kicked(int roomId, int boardId, int playerID, std::string reason)
{
	if (roomId != GetRoomID() || boardId != GetBoardID()) 
		return;

	if (playerID == GameController::myInfo->IDDB)
	{
		ScrMgr->OnShowDialog("notice", reason, TYPE_DIALOG::DIALOG_ONE_BUTTON, "",
			[this](const char * str, const char* btn_name)
		{
			this->doQuit();
		});

	}
}

bool GameScreen::onKeyBack()
{
	return false;
}

void GameScreen::doBack()
{
	GameService::leaveBoard(_roomID, _boardID);
	clientQuit();
}

void GameScreen::clientQuit()
{
}

void GameScreen::doQuit()
{
	if (GameController::isNewLobby(GameController::getCurrentGameType())) {
		ScrMgr->SwitchToMenu(NEW_LOBBY_SCREEN, HUD_SCREEN);
	}
	else {
		ScrMgr->SwitchToMenu(LOBBY_SCREEN, HUD_SCREEN);
	}
}

void GameScreen::doConfirmQuit(string msg)
{
	ScrMgr->OnShowDialog("notice", msg,
		TYPE_DIALOG::DIALOG_TWO_BUTTON,
		"",
		[& , this](const char * call_back, const char* btn_name)
	{
		//agree leave board
		GlobalService::confirmLeaveGame(this->_roomID,this->_boardID, (unsigned char)0);
		ScrMgr->OnShowDialog("notice", "pending", TYPE_DIALOG::DIALOG_WAITING_BUTTON);
	},
		[&, this](const char * call_back, const char* btn_name)
	{
		//not agree leave board
		GlobalService::confirmLeaveGame(this->_roomID, this->_boardID, (unsigned char)1);
	});
}

/**
* Ham nay dung de kick 1 user ra khoi game.
*/
void GameScreen::doKick(int playerID)
{}
void GameScreen::getFriendListToInvite()
{}

void GameScreen::showProfileOf(int userID)
{}

/**
* Set cho user vao trang thai ready hay k?
*
* @param isReady
*/
void GameScreen::doReady()
{}

void GameScreen::doReadyForReady()
{}

void GameScreen::doQuickPlay()
{}

void GameScreen::resetReady()
{}


/**
* Ham nay dung de set lai tien dat cuoc khi server tra ve.
*
* @param money
*/
void GameScreen::setBetMoney(s64 money, s64 max_money)
{
	GetSound->PlaySoundEffect("SFX_BETMONEY");
	this->money = money;
	
	if (btn_bet_iwin)
	{
		static_cast<ButtonWidget*>(btn_bet_iwin)->SetTitle(Utils::formatNUmber_dot(money));
	}

	if (max_money == 0)
	{
		static_cast<TextWidget*>(p_text_min_bet)->SetText(StringUtils::format("%s %s", LangMgr->GetString("minium_be").GetString(), Utils::formatMoney_dot_win(money).c_str()));
	}
	else
	{
		static_cast<TextWidget*>(p_text_min_bet)->SetText(LangMgr->GetString("minium_be") + Utils::formatNumber(money) + "/" + Utils::formatNumber(max_money));
	}
}
/**
* Remove the player when the player leaved the board. Khong nen override
* ham nay.
*
* @param leaveID
*/
void GameScreen::removePlayer(int leaveID)
{
	int idx_player = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB == leaveID)
		{
			if (p != nullptr)
			{
				p->setPlayer(nullptr , leaveID);
				p->setReady(false);
				p->stopTime();
			}
			seats[idx_player] = -1;
			//_PLAYER(p)->IDDB = -1;
			//_PLAYER(p)->setName("");
			//_PLAYER(p)->setExp(0);
			//_PLAYER(p)->isReady = false;
			//_PLAYER(p)->reset();
			break;
		}
		idx_player++;
	}

}
/**
* Thêm player vào danh sách
*/
void GameScreen::addPlayer(int seat, Player * p)
{
	PASSERT2((size_t)seat < p_list_player_group.size(), "wrong seat number !");
	p_list_player_group[seat]->setPlayer(p , seat);
}

/**
* Khoi tao thong tin ban khi minh moi vao game. Khi overide ham nay phai
* goi super truoc.
*
* @param roomID
* @param boardID
* @param ownerID
* @param money
* @param playerInfos
*/


/**
* cập nhật thông tin bàn
*/
void GameScreen::updateBoardInfo()
{}


void GameScreen::resetSeat() {

}

/**
* reset seat phuc vu cho viec join board khong nhay tien cua tat ca cac
* thang
*
*/
void GameScreen::resetSeatNotUserAction()
{}
/**
* Set chu phong cho game.
*
* @param newOwner
*/

void GameScreen::hideStartButton() {}
void GameScreen::hideReadyButton() {}

void GameScreen::showViewerLabel() {}

void GameScreen::hideViewerLabel() {}


bool GameScreen::hasOrtherPlayerInBoard()
{
	return false;
}
/**
* Show button continue game.
*/
void GameScreen::setContinueButton() {}

void GameScreen::hideContinueButton() {}
void GameScreen::setStartButton() {}



/**
* Tra ve player co id cu the
*
* @param userID
* @return null neu k tim thay.
*/

void GameScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney)
{
	//not handle this function
}

void GameScreen::onBonusMoney(int playerID, int value, std::string description, s64 currMoney)
{
	CCLOG("BONUS MONEY : %lld value %d with dec: %s ", currMoney, value, description.c_str());
	PlayerGroup* p = getPlayerGroup(playerID);
	if (p 
		&& p->getPlayer() && p->GetIDDB() >= 0)
	{
		if (p->GetIDDB() == GameController::myInfo->IDDB)
		{
			int pre_money = p->GetCurrentMoney();
			if (p_text_money)
			{
				static_cast<TextWidget*>(p_text_money)->TextRunEffect(
					currMoney, pre_money, 1.f, Utils::formatNUmber_dot, CallFunc::create([p, currMoney, this]()
				{
					p->SetCurrentMoney(currMoney);

					this->setMyMoney(currMoney);
				}));
				GameController::myInfo->setMoney(currMoney);
				GameController::myInfo->userProfile->money = currMoney;
				ScrMgr->MyInfoChanged();
			}
			else
			{
				PASSERT2(false, "some thing wrong here");
			}
			if (value > 0)
			{
				p_bonus_effect->Visible(true);
				static_cast<AnimWidget*>(p_bonus_effect)->RunAnim(CallFunc::create([this]() {
					this->p_bonus_effect->Visible(false);
				}));
				//playsound
				GetSound->PlaySoundEffect("SFX_COIN_DROP");
				//
				if (_isGameEnd)
				{
					GetSound->PlaySoundEffect("SFX_VOTAY");
				}
			}
		}
		else
		{
			p->SetActionMoneyRunTo(currMoney);
		}
		p->SetActionAppearBonusMoney(value);

		onHandleBonusMoney(p, value, currMoney);
	}
}

PlayerGroup*  GameScreen::getSeatGroup(int seat)
{
	return p_list_player_group[seat];
}


void GameScreen::setInviteOrtherPeopleButton()
{

}

void GameScreen::onSomeOneReady(int playerID, bool isReady)
{
	ScrMgr->OnHideDialog();
	if (p_list_player_group.size() <= 0)
	{
		PASSERT2(false, "wrong !");
		return;
	}

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB == playerID)
		{
			_PLAYER(p)->isReady = isReady;

			if (playerID == GameController::myInfo->IDDB) 
			{
				if (!_isViewer && !_isStartGame && !_isGameEnd)
				{
					//setReadyButton(isReady);
				}

				if (_myInfo && _PLAYER(_myInfo))
				{
					_PLAYER(_myInfo)->isReady = isReady;
				}
			}
			if (isReady)
			{
				GetSound->PlaySoundEffect("SFX_READY");
			}
			else
			{
				GetSound->PlaySoundEffect("SFX_UNREADY");
			}
			p->setReady(isReady);
			break;
		}
	}
	if (_isViewer && (!_myInfo || !_PLAYER(_myInfo))) {
		hideReadyButton();
		hideStartButton();
		showViewerLabel();
	}
}

void GameScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay)
{
	ScrMgr->OnHideDialog();
	if (p_list_player_group.size() <= 0) 
	{
		PASSERT2(false, "wrong !");
		return;
	}

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB == playerID)
		{

			_PLAYER(p)->isQuickPlay = isQuickPlay;

			if (playerID == GameController::myInfo->IDDB) 
			{
				// if (!isViewer && !isStartGame && !isGameEnd)
				// setReadyButton(isReady);
				if (_myInfo && _PLAYER(_myInfo))
				{
					_PLAYER(_myInfo)->isQuickPlay = isQuickPlay;
				}
			}
			if (isQuickPlay)
			{
				GetSound->PlaySoundEffect("SFX_READY");
			}
			else
			{
				GetSound->PlaySoundEffect("SFX_UNREADY");
			}
			p->setQuickPlay(isQuickPlay);
			break;
		}
	}

	if (_isViewer && _myInfo == nullptr)
	{
		hideReadyButton();
		hideStartButton();
		showViewerLabel();
	}
}


/**
* Implement ham nay de goi khi reset toan bo game cho van moi.
*/
void GameScreen::resetGame()
{}

/**
* thuc hien ban phao bong
*
* @param userID
*/


/**
* Goi ham nay de start game.
*/
bool GameScreen::doStartGame()
{
	return false;
}

void GameScreen::showDialogSendGiftTo(int userID)
{}



///thanh count down dem khi moi vao ban
void GameScreen::stopCountDownStart()
{
	OnHideCountDown();
}

// stop time bar phia duoi
void GameScreen::stopTimeBar()
{
	if (!p_timebar_process)
		return;

	p_timebar_process->onStop();
}
void GameScreen::startTimeBar(int interval, int remain)
{
	if (!p_timebar_process)
		return;

	p_timebar_process->setVisible(true);
	p_timebar_process->onStartime(interval,remain);

}
// nhìn chữ visibale thì biết dùng để ẩn count downtime rồi
void GameScreen::setVisibleCountDownTime(bool param1)
{

}

void GameScreen::clearSeat()
{
	for (int i = 0; i < p_max_player; i++)
	{
		seats[i] = -1;
	}
}
void GameScreen::setSeat(std::vector<Player*> l_players)
{
	if (p_list_player_group.size() > 0)
	{
		int myIndex = findMyIndex(l_players);

		size_t count = 0;

		while (count < l_players.size())
		{
			int _myseat = 0;//_myInfo ? _myInfo->GetSeat() : 0;
			int seat =  (_myseat + count) % p_max_player;
			int index = abs(myIndex - (int)count) % l_players.size();

			if (myIndex < (int)count)
				index *= -1;

			if (index < 0)
			{
				index = l_players.size() + index;
			}

			if (index < (int)l_players.size()
				&& l_players[index] != nullptr
				&& l_players[index]->IDDB != -1)
			{
				seats[seat] = index;
			}
			else
			{
				seats[seat] = -1;
			}
			count++;
		}

	}
}

void GameScreen::p_set_player_after_init(std::vector<Player*> playerInfos)
{
	//after create player finish, set the player 
	//alway remove old player
	if (p_list_player_group.size() > 0)
	{
		for (auto p : p_list_player_group)
		{
			p->removePlayer();
		}
	}
	//
	CreatePLayer(playerInfos);
	UpdateInfoPlayer(_roomID, _boardID, _ownerID, money, playerInfos);
}

void GameScreen::setPlayers(ubyte roomID, ubyte boardID, int ownerID, s64 money, std::vector<Player*> playerInfos)
{	
	this->_roomID = roomID;
	this->_boardID = boardID;
	this->_ownerID = ownerID;
	p_thread_player_infos.clear();
	p_thread_player_infos = playerInfos;

	if (p_load_xml_state > p_max_player)
	{
		p_set_player_after_init(p_thread_player_infos);
	}

}

void GameScreen::CreatePLayer(std::vector<Player*> playerInfos)
{
	//do not call to this function
	//PASSERT2(false, "child class dont not call this function, just override its !");
}

void GameScreen::UpdateInfoPlayer(ubyte roomID, ubyte boardID, int ownerID, s64 money ,std::vector<Player*> playerInfos)
{
	if (p_list_player_group.size() <= 0)
	{
		PASSERT2(false, "something wrong !");
		return;
	}

	if (playerInfos.size() > p_list_player_group.size())
	{
		PASSERT2(false, "size of player must be smaller or equal with size of design");
		return;
	}

	for (size_t i = playerInfos.size(); i < p_list_player_group.size(); i++)
	{
		_WIDGET(p_list_player_group[i])->Visible(false);
	}

	//
	this->_roomID = roomID;
	this->_boardID = boardID;
	this->_ownerID = ownerID;
	for (auto p : p_list_player_group)
	{
		if (_PLAYER(p) && GameController::myInfo->IDDB == _PLAYER(p)->IDDB)
		{
			_isViewer = false;
		}
	}
	clearSeat();
	setSeat(playerInfos);
	ResetReady();
	setPlayerSeat(playerInfos);
	//CheckReady();
	setOwner(ownerID);
	setBetMoney(money);
}

void GameScreen::setPlayerSeat(std::vector<Player*> l_players)
{
	if (l_players.size() > 0)
	{
		_myInfo = nullptr;
		for (size_t i = 0; i < l_players.size(); i++)
		{
			if (i >= (size_t)p_max_player)
			{
				return;
			}
			int seat = seats[i];
			PlayerGroup* pg = p_list_player_group[i];
			if (seat != -1)
			{
				if (l_players[seat] && 
					l_players[seat]->IDDB == GameController::myInfo->IDDB) // nếu là mình thì run action tiền ở khung my info
				{
					_myInfo = pg;
				}

				pg->setPlayer(l_players[seat], i);
			}
			else
			{
				pg->setPlayer(nullptr , i);
			}
		}
		_isViewer = (_myInfo == nullptr);
	}
}

int GameScreen::findMyIndex(std::vector<Player*> l_players)
{
	int idx = 0;
	for (auto p : l_players)
	{
		if (p != nullptr && 
			GameController::myInfo->IDDB == p->IDDB)
		{
			break;
		}

		idx++;

	}
	return idx;
}

void GameScreen::onSomeOneJoinBoard(int seat, Player * player)
{
	GetSound->PlayMusicEffect("SFX_JOINBOARD");
	addPlayer(seat, player);
	size_t idx = 0;
	for (; idx < seats.size() ; idx++)
	{
		if (seats[idx] < 0)
		{
			seats[idx] = seat;
			break;
		}
	}
	idx = 0;

	if (_myInfo != nullptr)
	{
		setOwner(_ownerID);
	}
	//run effect money
	auto player_group = getPlayerGroup(player->IDDB);
	if (player_group && player)
	{
		player_group->SetActionMoneyRunTo(player->getMoney());
	}
}

void GameScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GetSound->PlayMusicEffect("SFX_LEAVEBOARD");
	_ownerID = newOwner;
	removePlayer(leave);
	setOwner(newOwner);
	//ResetSeat();

	if (GetAvailableNumberPlayers() <= 1)
	{
		OnHideCountDown();
	}
}

PlayerGroup* GameScreen::getUserGroup(int playerID)
{
	int seat = GetSeatFromPlayerID(playerID);
	return GetSeatGroup(seat);
}

int GameScreen::GetSeatFromPlayerID(int playerID)
{
	int idx = 0;
	for (auto p :  p_list_player_group)
	{
		if (p != nullptr &&
			p->getPlayer() != nullptr &&
			p->getPlayer()->IDDB > 0 &&
			p->getPlayer()->IDDB == playerID)
		{
			return idx;
		}
		idx++;
	}
	return -1;

}

PlayerGroup* GameScreen::GetSeatGroup(int seat)
{
	if (seat > -1 && seat < p_max_player)
	{
		return p_list_player_group.at(seat);
	}
	else
	{
		return nullptr;
	}
}

Player* GameScreen::getMyPlayer()
{
	for (auto p : p_list_player_group)
	{
		if (p != nullptr
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB == GameController::myInfo->IDDB)
		{
			return _PLAYER(p);
		}
	}

	return nullptr;
}


void GameScreen::OnBeginFadeIn()
{
	static_cast<TextWidget*>(p_text_gamename)->SetText(StringUtils::format("%s", GameController::getCurrentGameName().c_str()));
}

void GameScreen::OnFadeInComplete()
{
	HUDScreen::OnFadeInComplete();
	if (!_isStartGame)
	{
		resetGame();
	}
}

void GameScreen::setOwner(int newOwner)
{
	_ownerID = newOwner;
	for( PlayerGroup* g : p_list_player_group)
	{
		if (g != nullptr)
		{
			g->setOwner(false);
		}
	}

	PlayerGroup* pg = getUserGroup(newOwner);
	if (pg != nullptr)
	{
		Player * p = getPlayer(newOwner);
		if (p != nullptr)
		{
			p->isReady = false;
		}
		pg->setOwner(true);

	}
}

Player* GameScreen::GetPlayerAtIdx(int idx)
{
	if (p_list_player_group.size() > 0 && idx < (int)p_list_player_group.size() && p_list_player_group[idx])
	{
		return p_list_player_group[idx]->getPlayer();
	}
	return nullptr;
}

Player* GameScreen::getPlayer(int userID)
{
	for (auto p : p_list_player_group)
	{
		if (p != nullptr
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB == userID)
		{
			return _PLAYER(p);
		}
	}
	return nullptr;
}

PlayerGroup * GameScreen::getPlayerGroup(int playerID)
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p != nullptr && p->getPlayer() != nullptr && p->getPlayer()->IDDB == playerID)
		{
			return p;
		}
	}
	return nullptr;
}

int	GameScreen::getNumberPlayer()
{
	int number = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB > -1)
		{
			number++;
		}
	}
	return number;
}

void GameScreen::stopAllTimePlayer()
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p != nullptr)
		{
			p->stopTime();
		}
	}
}
bool GameScreen::IsShow()
{
	return ScrMgr->GetCurrentMenuUI() == MENU_LAYER::HUD_SCREEN;
}

void GameScreen::flyMoney(s64 money, int userID)
{
	PlayerGroup* group = getPlayerGroup(userID);
	GetFlyMoney()->fly(money, getPlayerPos(group), this);
}

Vec2 GameScreen::getPlayerPos(PlayerGroup* pg)
{
	Vec2 pos;
	Vec2 group_pos = _WIDGET(pg)->GetPosition();
	Vec2 group_size = _WIDGET(pg)->GetSize();
	bool horizontal = false;
	if (pg->GetSeat() == 0) return Vec2(group_pos.x + group_size.x / 2, group_pos.y + group_size.y / 2);
	if (pg->GetSeat() == 1)
	{
		pos = group_pos + Vec2(group_size.x, group_size.y);
	}
	else if (pg->GetSeat() == 2)
	{
		pos = group_pos + Vec2(group_size.x, .0f);
		horizontal = true;
	}
	else if (pg->GetSeat() == 3)
	{
		pos = group_pos - Vec2(group_size.x, -group_size.y / 2);
	}
	return pos;
}


void GameScreen::updateTextWithMoneyType()
{
	GetWidgetChildByName("HUD_screen.btn_purchase_ruby")->SetVisible(false);
	GetWidgetChildByName("HUD_screen.btn_purchase_win")->SetVisible(false);
	if (GameController::currentMoneyType == RUBY_MONEY)
	{
		GetWidgetChildByName("HUD_screen.btn_purchase_ruby")->SetVisible(true);
		p_text_money = static_cast<TextWidget*>(GetWidgetChildByName("HUD_screen.btn_purchase_ruby.title"));
	}
	else
	{
		GetWidgetChildByName("HUD_screen.btn_purchase_win")->SetVisible(true);
		p_text_money = static_cast<TextWidget*>(GetWidgetChildByName("HUD_screen.btn_purchase_win.title"));
	}
	CCLOG("call GameScreen::updateTextWithMoneyType() and set p_text_money");
}

void GameScreen::setMyMoney(s64 money)
{
	if (p_text_money)
	{
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNUmber_dot(money));
	}
}

void GameScreen::setMyUsername(std::string name)
{
	static_cast<TextWidget*>(p_text_name)->SetText(name);
}

void GameScreen::setRubyMoney(int iddb, s64 ruby)
{
	if (iddb == GameController::myInfo->IDDB)
	{
		setMyMoney(ruby);
	}
	else
	{
		PlayerGroup* pg = getPlayerGroup(iddb);
		if (pg)
		{
			pg->setMoney(ruby);
		}
	}
}

void GameScreen::HandleAfterInitPlayer()
{
	//PASSERT2(false, "every class must override this function ???");
	//not handle this
	p_is_finish_load_player = true;
}

void GameScreen::start_thread_load_xml_player(RKString name)
{
	if (p_thread_load_xml_player_dec)
	{
		delete p_thread_load_xml_player_dec;
		p_thread_load_xml_player_dec = nullptr;
	}

	p_thread_load_xml_player_dec = new ThreadWrapper();
	p_game_name_xml = name;
	p_thread_load_xml_player_dec->CreateThreadWrapper("thread_parse_ui",
		([](void * data) -> uint32
	{
		GameScreen * scr = (GameScreen*)data;
		scr->p_thread_load_xml_player_dec->OnCheckUpdateThreadWrapper
		([scr]()
		{
			XMLMgr->OnLoadXMLData<GameScreen>(scr->p_game_name_xml, std::mem_fn(&GameScreen::LoadXMLListPosPlayer), scr);
			XMLMgr->OnLoadXMLData<GameScreen>("ListPlayer", std::mem_fn(&GameScreen::LoadXMLListPlayer), scr);
			scr->p_load_xml_state = 1;
		});
		return 1;
	}), (void*)this);
}

void GameScreen::parse_xml_player(int idx)
{
	CCLOG("Create Player Group %d", idx);
	InitPlayerChild(p_list_sample_dec.at("layout_user_"), p_list_pos_dec.at(idx), idx);
	if (idx == p_max_player - 1)
	{

		for (auto it : p_list_sample_dec)
		{
			delete it.second;
		}
		p_list_sample_dec.clear();

		if (p_thread_player_infos.size() > 0)
		{
			p_set_player_after_init(p_thread_player_infos);
		}
	}
}

void GameScreen::onCreateListPlayer(RKString name_game_xml)
{
	if (p_list_player_group.size() > 0)
	{
		p_list_player_group.clear();
		p_list_player_layout->ClearChild();
	}
	//XMLMgr->OnLoadXMLData<GameScreen>(name_game_xml, std::mem_fn(&GameScreen::LoadXMLListPosPlayer), this);
	//XMLMgr->OnLoadXMLData<GameScreen>("ListPlayer", std::mem_fn(&GameScreen::LoadXMLListPlayer), this);

	start_thread_load_xml_player(name_game_xml);
}

void GameScreen::LoadXMLListPosPlayer(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}
	GameType current_gameplay = GameController::getCurrentGameType();

	Vec2 design_size = Vec2(0, 0);

	auto sample_game_type_dec = Root->FirstChildElement("game");

	if (sample_game_type_dec)
	{
		int id = atoi(sample_game_type_dec->Attribute("id"));
		//if (id == current_gameplay)
		{
			design_size = XMLMgr->ConvertToVector2(sample_game_type_dec->Attribute("design"));
			auto _player_xml = sample_game_type_dec->FirstChildElement("player");
			do
			{
				if (_player_xml)
				{
					int id = atoi(_player_xml->Attribute("id"));

					xml::BasicDec * dec = new xml::BasicDec();

					auto common_obj = _player_xml->FirstChildElement("common");
					do
					{
						if (common_obj)
						{
							RKString name_common_value = common_obj->Attribute("name");
							RKString common_value = common_obj->Attribute("value");

							bool res = XMLMgr->OnParseValueIntoXMLDec(name_common_value, common_value, dec, nullptr);
							

							common_obj = common_obj->NextSiblingElement();
						}
					} while (common_obj);
			
					dec->InsertDataVector2("design_size", design_size);
					p_list_pos_dec.insert(std::pair<int, xml::BasicDec* >(id, dec));
					_player_xml = _player_xml->NextSiblingElement();
				}
			} while (_player_xml);
		}
	}
}

void GameScreen::LoadXMLListPlayer(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("list_widget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget, _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

}

void GameScreen::InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec * dec, int idx)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	xml_dec->p_menu_widget = menu_widget_xml;
	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen")->GetChildWidgetByName("layout_list_user_card");
	//auto panel = GetWidgetChildByName("HUD_screen.layout_list_user_card");
	auto layer_root = GetLayerChildByName("HUD_screen");

	Vec2 position = dec->GetDataVector2("value_pos");
	Vec2 pos_user = dec->GetDataVector2("pos_tex1");
	Vec2 pos_iwin = dec->GetDataVector2("pos_tex2");
	Vec2 pos_chat = dec->GetDataVector2("pos_chat");
	Vec2 pos_text_bonus = dec->GetDataVector2("pos_text_bonus");

	Vec2 design_size = dec->GetDataVector2("design_size");
	Vec2 ratio_pos = Vec2(position.x / design_size.x, position.y / design_size.y);
	Vec2 ratio_pos_1 = Vec2(pos_user.x / design_size.x, pos_user.y / design_size.y);
	Vec2 ratio_pos_2 = Vec2(pos_iwin.x / design_size.x, pos_iwin.y / design_size.y);
	Vec2 ratio_pos_chat = Vec2(pos_chat.x / design_size.x, pos_chat.y / design_size.y);
	Vec2 ratio_text_bonus  = Vec2(
		pos_text_bonus.x * GetGameSize().width / design_size.x, 
		pos_text_bonus.y * GetGameSize().height / design_size.y);

	Vec2 anchor_point = dec->GetDataVector2("anchor_point");

	xml_dec->GeneralValue->ReplaceDataVector2("anchor_point", anchor_point);
	xml_dec->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos);

	auto layout_player = xml_dec->GetChildWidgetByName("layout_player");

	auto user_name = layout_player->GetChildWidgetByName("user_name_txt");
	user_name->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 0));
	user_name->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_1);

	auto iwin = layout_player->GetChildWidgetByName("number_iwin_txt");
	iwin->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 1));
	iwin->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_2);

	auto panel_chat = xml_dec->GetChildWidgetByName("panel_display_chat");
	panel_chat->GeneralValue->ReplaceDataVector2("anchor_point", anchor_point);
	panel_chat->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_chat);

	auto widget = layer_root->InitWidgetEntity(xml_dec, layer_root, p_list_player_layout);
	p_list_player_layout->ResizeAndPosChildWidget(widget);

	widget->SetName("layout_user_" + std::to_string(idx));

	widget->GetWidgetChildByName(".layout_player.btn_info_user_")->SetName("btn_info_user_" + std::to_string(idx));
	//widget->GetWidgetChildByName(".layout_player.btn_gift_user_")->SetName("btn_gift_user_" + std::to_string(idx));

	auto layout_control = widget->GetWidgetChildByName(".layout_player.layout_player_controller");
	if (layout_control && dec->HaveDataInt("rotate_control"))
	{
		int rotate = dec->GetDataInt("rotate_control");
		layout_control->SetRotate(rotate);
		for (size_t i = 0; i < layout_control->GetNumberChildWidget(); i++)
		{
			layout_control->GetWidgetChildAtIdx(i)->SetRotate(-rotate);
		}
	}

	widget->SetMenuParent(p_list_player_layout->GetMenuParent());
	widget->SetParentWidget(p_list_player_layout);
	widget->SetParentLayer(p_list_player_layout->GetParentLayer());

	//set for owner
	if (idx == 0)
	{
		widget->GetWidgetChildByName(".btn_add_user_")->Visible(false);
		//widget->GetWidgetChildByName(".layout_player.btn_gift_user_0")->Visible(false);
		widget->GetWidgetChildByName(".layout_player.user_name_txt")->Visible(false);
		widget->GetWidgetChildByName(".layout_player.number_iwin_txt")->Visible(false);
	}

	auto group_player = new PlayerGroup(widget);

	group_player->SetPosBonus(ratio_text_bonus);
	group_player->SetCurrentGameScreen(this);
	group_player->SetXMLData(xml_dec->Clone(), 0);
	OnLoadDecPrivatePlayer(dec, group_player);
	p_list_player_group.push_back(group_player);
	p_list_player_layout->InsertChildWidget(widget);
}

int GameScreen::GetRoomID()
{
	return _roomID;
}

int GameScreen::GetBoardID()
{
	return _boardID;
}

bool GameScreen::IsMyInfoRoomOwner()
{
	if (_myInfo)
	{
		return _myInfo->IsOwner();
	}
	return false;
}

void GameScreen::OnCardTouchEnd()
{
	//not handle
}
void GameScreen::OnCardTouchDown()
{
	//not handle
}
void GameScreen::OnCardTouchMove()
{
	//not handle
}
void GameScreen::OnCardTouchCancel()
{
	//not handle
}

void GameScreen::OnHandleTouchUpdate(float dt)
{
	if (!p_enable_touch)
		return;
	//touch update
	if (InputMgr->GetNumberTouch() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_PRESS)
		{
			OnCardTouchDown();
		}
		else if (current_state_touch == Control::STATE_HOLD)
		{
			OnCardTouchMove();
		}

	}
	else if (InputMgr->GetNumberTouchRelease() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_RELEASE)
		{
			OnCardTouchEnd();
		}
		InputMgr->OnClearTouchReleaseList();
	}

}

void GameScreen::onBuzz(int fromId, std::string message)
{
	getPlayerGroup(fromId)->SetTextChat(message);
}

void GameScreen::onChat(int fromId, std::string message)
{
	getPlayerGroup(fromId)->SetTextChat(message);
}

size_t GameScreen::GetAvailableNumberPlayers()
{
	size_t num = 0;

	for (auto p : p_list_player_group)
	{
		if (p 
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB > -1 
			&& _PLAYER(p)->name != "")
		{
			num++;
		}
	}
	return num;
}

void GameScreen::SetEnableTouch(bool value)
{
	p_enable_touch = value;
}

void GameScreen::SetMaxPlayer(int max_player)
{
	p_max_player = max_player;
	for (int i = 0; i < p_max_player; i++)
	{
		seats.push_back(-1);
	}
}

void GameScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	//not handle this function
}

void GameScreen::OnHandleMsg(iwincore::Message* msg)
{
	//not handle this function
	if (GameLogicHandler::gameHandlers)
	{
		CCLOG("Handle late message");
		//GameLogicHandler::gameHandlers->onMessage(msg);
		GetGameMessageHandler()->onMessage(msg);
	}
	else
	{
		PASSERT2(false, "");
	}
}

int GameScreen::PushReturnMsg(iwincore::Message* msg)
{
	if (!p_is_finish_load_player)
	{
		p_list_msg_return.push_back(new iwincore::Message(msg));
		return 1;
	}

	return 0;
}



void GameScreen::onBetRange(int min, s64 & max, s64 maxMoneyRoom)
{
	s64 min_max_money = max;

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB > -1)
		{
			s64 money = _PLAYER(p)->money;

			if (money < min_max_money)
			{
				min_max_money = money;
			}
		}
	}

	max = min_max_money;

	setBetMoney(min , max);
}

void GameScreen::doSetMoney(int money)
{
	GameService::setMoney(_roomID, _boardID, money);
}

void GameScreen::ResetReady()
{
	for (auto p : p_list_player_group) 
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB != -1 && _PLAYER(p)->IDDB != _ownerID) 
		{
			_PLAYER(p)->isReady = false;
			p->setReady(false);
		}
	}

	if (_myInfo &&  _PLAYER(_myInfo) && _PLAYER(_myInfo)->IDDB != _ownerID)
	{
		_PLAYER(_myInfo)->isReady = false;
		_myInfo->setReady(false);
	}
}

void GameScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_send_message")
	{
		auto text_chat_widget = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input_chat.text_input_chat");
		RKString str_chat = static_cast<TextFieldWidget*>(text_chat_widget)->GetText();

		GetPlayerGroupAtIdx(0)->SetTextChat(str_chat);
		GameService::chatToBoard(_roomID, _boardID, str_chat.GetString());

		p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this]()
		{
			this->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
			this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create([this] {
				this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->ForceFinishAction();
				this->p_panel_chat_root->SetActionCommandWithName("ON_HIDE");
				this->p_panel_chat_root->GetParentLayer()->SetInteractive(true);
			}));
		}
		));

		static_cast<TextFieldWidget*>(p_text_input_chat)->ClearText();
	}
	if (type_widget == UI_TYPE::UI_BUTTON && name == "quit_ui_btn")
	{	
		p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this]()
		{
			p_panel_igm->ForceFinishAction();
			SetInteractiveInsteadOf(true, p_panel_igm);
			this->doBack();
		}));
	}
	else
	{
		HUDScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

void GameScreen::OnChatInBoard(int fromID, RKString message)
{
	auto p = getPlayerGroup(fromID);
	if (p)
	{
		p->SetTextChat(message);
	}
}

void GameScreen::OnSystemChat(RKString message)
{
	if (_myInfo)
	{
		message = "[SYSTEM] : " + message;
		_myInfo->SetTextChat(message);
	}
}

void GameScreen::OnMatchResult(std::vector<GameResult*> result)
{
	PASSERT2(false, "handle action here!!!!");
	for (auto m : result)
	{
		PlayerGroup * g = getPlayerGroup(m->ID);
		if (g && _PLAYER(g))
		{
			// Update info for report
			if (GameController::getCurrentGameType() != GameType::GAMEID_CO_UP
				&& GameController::getCurrentGameType() != GameType::GAMEID_CHESS)
			{
				/*if (m->name == null)
				{
					m->name = Util::computeVisibleTextAndTruncated(GameController::game.commonAsset.defaultFont,
						_PLAYER(g).name, getWidth() / 4);
				}*/
			}

			//m.name = Util.computeVisibleTextAndTruncated(GameController.game.commonAsset.defaultFont, p.name,
			//	getWidth() / 4);
			m->avatar = _PLAYER(g)->avatar;
			// m.d_exp_percent = (m.d_exp * 100) / p.level.expRequire;
			// Update new data for wait screen
			int newExp = _PLAYER(g)->getExp() + m->d_exp;
			if (newExp < 0) 
			{
				newExp = 0;
			}
			_PLAYER(g)->setExp(newExp);
			_PLAYER(g)->addMoney(m->d_money);

			if (m->ID == GameController::myInfo->IDDB) 
			{
				GameController::myInfo->addMoney(m->d_money);
				flyMoney(m->d_money , m->ID);
				GameController::myInfo->setExp(newExp);
				if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS
					|| GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_COVUA) {
					switch (m->result) 
					{
					case -1:
						doSomethingWhenILose();
						GetSound->PlaySoundEffect("SFX_GAMECO_LOSE");
						//shareCaption = L.gL(843);
						break;
					case 0:
						doSomethingWhenIRaw();
						GetSound->PlaySoundEffect("SFX_GAMECO_DRAW");
						//shareCaption = L.gL(844);
						break;
					case 1:
						doSomethingWhenIWin();
						GetSound->PlaySoundEffect("SFX_GAMECO_WIN");
						//shareCaption = L.gL(842);
						break;
					}
				}
				else if (
					GameController::getCurrentGameType() == GameType::GAMEID_CARO
					|| GameController::getCurrentGameType() == GameType::GAMEID_TETRIS) 
				{
					switch (m->result) 
					{
					case -1:
						doSomethingWhenILose();
						GetSound->PlaySoundEffect("SFX_GAMEBAI_THUABAI");
						//shareCaption = L.gL(846);
						break;
					case 0:
						doSomethingWhenIRaw();
						GetSound->PlaySoundEffect("SFX_VOTAY");
						//shareCaption = L.gL(847);
						break;
					case 1:
						doSomethingWhenIWin();
						GetSound->PlaySoundEffect("SFX_GAMEBAI_THANGBAI");
						//shareCaption = L.gL(845);
						break;
					}

				}
			}
			break;
		}
	}

	//delete after use
	SAFE_DELETE_VECTOR(result);
}

void GameScreen::OnReceivedWaitTimeInfo(int sec)
{
	PASSERT2(false, "handle action here!!!!");
}

/**
* thuc hien ban phao bong
*
* @param userID
*/
void GameScreen::doFireWork(int userID) 
{
	PlayerGroup * g = getPlayerGroup(userID);
	if (g != nullptr && g->getPlayer() != nullptr) 
	{
		auto position = _WIDGET(g)->GetPosition_Middle();
		PASSERT2(false, "not handle effect firework");
		//fireWorkGroup.toFront();
		//fireWorkGroup.fire();
	}
}

/**
* lam mot so hieu ung, am thanh gi do khi minh thang , thuc hien cac hieu
* ung chung cho tat ca game
*/
void GameScreen::doSomethingWhenIWin() 
{
}

/**
* lam mot so hieu ung, am thanh gi do khi minh thua, thuc hien cac hieu ung
* chung cho tat ca game
*/
void GameScreen::doSomethingWhenILose() 
{
}

/**
* lam mot so hieu ung, am thanh gi do khi minh hoa, thuc hien cac hieu ung
* chung cho tat ca game
*/
void GameScreen::doSomethingWhenIRaw() 
{
}

void GameScreen::onReconnect()
{
	GetSound->PlaySoundEffect("SFX_BUZZ");
}