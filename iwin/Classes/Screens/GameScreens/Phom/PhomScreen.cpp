#include "PhomScreen.h"
#include "UI/AdvanceUI/wButton.h"

#include "CardPhomBox.h"
#include "PhomUtils.h"
#include "PhomService.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/PlayerGroup.h"
#include "Screens/Object/SpecialCardsBound.h"
#include "Screens/GameScreens/TienLen/TienLenPlayer.h"
#include "Screens/GameScreens/DealCardLayer.h"

extern int s_card_w;
extern int s_card_h;

#define PHOM_LAYER_DEAL 53

Phom::Phom(int type, VecByte cards)
{
	_cards = cards;
	_type = type;
}

Phom::Phom()
{

}

bool Phom::containCard(byte card)
{
	return K_VEC_CONTAIN(_cards, card);
}

PhomScreen::PhomScreen()
{
	_card_get_now = -1;
	_cur_fire_cardID = -1;
	SetMaxPlayer(4);
}

PhomScreen::~PhomScreen()
{
	for (int i = 0; i < p_max_player; i++)
	{
		CC_SAFE_DELETE(_cardPhomBox[i]);
	}
}

bool PhomScreen::InitGame(int game_id)
{
	GameScreen::InitGame(game_id);


	XMLMgr->OnLoadXMLData<PhomScreen>("Phom_Screen", std::mem_fn(&PhomScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("Phom_Screen");
	p_panel_bottom = GetWidgetChildByName("HUD_screen.panel_custom_menu.back_panel_card");
	p_panel_game_card = GetWidgetChildByName("HUD_screen.panel_custom_menu.game_card_layer");

	for (int i = 0; i < 53; i++)
	{
		Card* c = Card::NewInstance(i, DECK_TYPE::DECK_BAICAO);
		NODE(c)->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		c->SetVisible(false);
		_card_pools.push_back(c);
		NODE(p_panel_game_card)->addChild(NODE(c));
	}

	s_card_w = _card_pools[0]->GetSize().x;
	s_card_h = _card_pools[0]->GetSize().y;

	for (int i = 0; i < p_max_player; i++)
	{
		_cardPhomBox[i] = new CardPhomBox();
		_cardPhomBox[i]->init(this, i);
	}



	

	p_btn_change_phom = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_change_phom"));
	p_btn_get_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_get_card"));
	p_btn_eat_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_eat_card"));
	p_btn_fire_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_fire_card"));
	p_btn_down_phom = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_down_phom"));
	p_btn_send_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_send_card"));
	p_btn_create_phom = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".btn_create_phom"));
	_list_btns.push_back(p_btn_change_phom);
	_list_btns.push_back(p_btn_get_card);
	_list_btns.push_back(p_btn_eat_card);
	_list_btns.push_back(p_btn_fire_card);
	_list_btns.push_back(p_btn_down_phom);
	_list_btns.push_back(p_btn_send_card);
	_list_btns.push_back(p_btn_create_phom);
	for (auto it : _list_btns)
	{
		it->SetVisible(false);
	}

	_deal_card_layer = new DealCardLayer();
	_deal_card_layer->init();
	_deal_card_layer->autorelease();
	NODE(p_panel_game_card)->addChild(_deal_card_layer, PHOM_LAYER_DEAL);


	return true;
}



void PhomScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//insert into custom menu
	auto p_panel_custom = GetWidgetChildByName("HUD_screen.panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("game_card_layer"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();
}


void PhomScreen::InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec * dec, int idx)
{
	GameScreen::InitPlayerChild(xml_dec, dec, idx);
	Vec2 design_size = dec->GetDataVector2("design_size");
	Vec2 pos_eat = dec->GetDataVector2("pos_eat");
	Vec2 pos_fire = dec->GetDataVector2("pos_fire");
	Vec2 pos_free = dec->GetDataVector2("pos_free");
	cocos2d::Size game_size = GetGameSize();

	//Vec2 pos_eat_ratio = Vec2(pos_eat.x / design_size.x)
	pos_eat = Vec2(pos_eat.x / design_size.x, pos_eat.y / design_size.y);
	pos_fire = Vec2(pos_fire.x / design_size.x, pos_fire.y / design_size.y);
	pos_free = Vec2(pos_free.x / design_size.x, pos_free.y / design_size.y);

	pos_eat = Vec2(pos_eat.x * game_size.width, pos_eat.y * game_size.height);
	pos_fire = Vec2(pos_fire.x * game_size.width, pos_fire.y * game_size.height);
	pos_free = Vec2(pos_free.x * game_size.width, pos_free.y * game_size.height);

	_cardPhomBox[idx]->setPosEat(pos_eat);
	_cardPhomBox[idx]->setPosFire(pos_fire);
	_cardPhomBox[idx]->setPosFree(pos_free);
}

void PhomScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
		if (type_widget == UI_BUTTON && name == "btn_fire_card")
		{
			//_cardPhomBox[1]->fireCard(8);
			actionFire();
			//int new_card = CardUtils::getCardTestRandom();
			//_myFreeCards.insert(_myFreeCards.begin() + CCRANDOM_0_1() * _myFreeCards.size(), new_card);
			//_cardPhomBox[0]->getCard(new_card, _myFreeCards);
		}
		else if (type_widget == UI_BUTTON && name == "btn_get_card")
		{
			actionGetCard();
			//onGetCard(14542542, 47);
		}
		else if (type_widget == UI_BUTTON && name == "btn_eat_card")
		{
			actionEat();
		}
		else if (type_widget == UI_BUTTON && name == "btn_down_phom")
		{
			actionDownPhom();
		}
		else if (type_widget == UI_BUTTON && name == "btn_send_card")
		{
			sendCardSend();
		}
		else
		{
			GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
		}
}

void PhomScreen::OnBeginFadeIn()
{
	GameScreen::OnBeginFadeIn();
	//resetGame();
}

void PhomScreen::OnFadeInComplete()
{
	GameScreen::OnFadeInComplete();
}

int PhomScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}
	VecCard list_card = _cardPhomBox[0]->getFreeCards();
	for (Card* c : _card_pools)
	{
		c->Update(dt);
	}
	return 1;
}


void PhomScreen::initFake()
{
	resetGame();

	for (int i = 0; i < 3; i++)
	{
		Player* p = new TienLenPlayer();
		p->IDDB = i;
		p->name = StringUtils::format("test %d", i);
		addPlayer(i + 1, p);
	}
	_myInfo = p_list_player_group[0];

	CardUtils::initCardTest();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			_cardPhomBox[i]->addFreeCard(CardUtils::getCardTestRandom());
		}
		for (int j = 0; j < 4; j++)
		{
			_cardPhomBox[i]->addFireCard(CardUtils::getCardTestRandom());
		}
		for (int j = 0; j < 6; j++)
		{
			_cardPhomBox[i]->addPhomCard(CardUtils::getCardTestRandom());
		}
		_cardPhomBox[i]->layoutBox();
	}


	VecByte cards;
	//for (int i = 0; i < 10; i++)
	//{
	//	cards.push_back(CardUtils::getCardTestRandom());
	//}
	//"44 51 46 38 30 5 2 4 45 22"
	//11 17 20 22 23 26 36 46 51
	//1 2 12 16 20 29 37 42 48
	cards.push_back(CardUtils::getCardTest(1));
	cards.push_back(CardUtils::getCardTest(2));
	cards.push_back(CardUtils::getCardTest(12));
	cards.push_back(CardUtils::getCardTest(16));
	cards.push_back(CardUtils::getCardTest(20));
	cards.push_back(CardUtils::getCardTest(29));
	cards.push_back(CardUtils::getCardTest(37));
	cards.push_back(CardUtils::getCardTest(42));
	cards.push_back(CardUtils::getCardTest(48));
	_myFreeCards = cards;
	std::string test = "";
	for (size_t i = 0; i < cards.size(); i++)
	{
		test += StringUtils::format(" %d", cards.at(i));
	}
	CCLOG("TTTTTTTTTTTTTT %s", test.c_str());
	
	//onStartGame(30, cards, 0, 0);
	//onGetCard(14542542, 47);
	//onFire(1, 0, 35, 1);
	//actionEat();
	//onEatCard(0);
	showGameButton(false, true, false, false, false, false);
	static_cast<Button*>(p_btn_get_card->GetResource())->setBright(false);
	p_btn_get_card->SetDisable(true);
	//p_btn_fire_card->SetInteractive(false);
	setInfo(0, 3);
	setInfo(1, 4);
	setInfo(2, 4);
	setInfo(3, 3);
	setScore(0, 10);
	setScore(1, 20);
	setScore(2, 30);
	setScore(3, 40);
}

void PhomScreen::setMovePlayer(int userID)
{
	PlayerGroup* pg = getPlayerGroup(userID);
	if (pg) pg->startTime(_totalTimes, 0);

	pg = getPlayerGroup(_last_move_player);
	if(pg) pg->stopTime();

	_cur_move_player = userID;
}

Card* PhomScreen::getCard(int cardId)
{
	return _card_pools[cardId];
}
/**
* tìm phỏm và sắp xếp lại cọc bài của mình bài trong cọc dựa trên 5 danh
* sách myFreeCards myPhoms myEatCards myDealCards myFiredCards
*/
void PhomScreen::updateAndLayoutMyPhomBox() {

	if (_isDownMyPhom) {
		// đã hạ phỏm thì ko phá phỏm đã hạ, chỉ có thêm vô là phỏm tạo bằng
		// bài rác
		// bai free hien tai bao gom bai phom va bai free con trong deal card
		VecByte newFreeOld;
		VecByte newEatCard;
		for (size_t i = 0;i< _myDealCards.size();i++)
		{
			if(K_VEC_CONTAIN(_myFreeCards, _myDealCards.at(i)))
				newFreeOld.push_back(_myDealCards.at(i));

			for (size_t j = 0;j<_myPhoms.size();j++)
			{
				if(K_VEC_CONTAIN(_myPhoms.at(j)._cards, _myDealCards.at(i)))
				{
					newFreeOld.push_back(_myDealCards.at(i));
				}
			}
		}
		for (size_t j = 0; j < _myPhoms.size(); j++) {
			for (size_t i = 0; i < _myEatCards.size(); i++) {

				if(K_VEC_CONTAIN(_myPhoms.at(j)._cards, _myEatCards.at(i))) {
					newEatCard.push_back(_myEatCards.at(i));
				}
			}
		}

		VecByte newFree;

		std::vector<Phom> phoms = PhomUtils::findPhoms(newFreeOld, newEatCard,newFree, true);
		// khi da ha phom phai thuc hien check va add lai toan bo phom va free card
		_myPhoms.clear();
		_myFreeCards.clear();
		K_VEC_INSERT(_myFreeCards, newFree);
		if (phoms.size() > 0) {
			K_VEC_INSERT(_myPhoms, phoms);
		}
	}
	else {
		std::vector<Phom> phomsHistory = _myPhoms;
		_myPhoms = PhomUtils::findPhoms(_myDealCards, _myEatCards, _myFreeCards, true);
		//Log.log(TAG, "updateAndLayoutMyPhomBox: " + myPhoms.toString());
		sendMyPhomToServer(GetRoomID(), GetBoardID(), _myPhoms, _userSendPhoms);
		// check lai mot lan nua neu tat ca bai an khong duoc nam trong phom thi khong cho doi phom
		// giu lai phom moi
		// vi thuat toan tim phom chua on dinh 
		if (!checkHaveEatCardInAllPhom(_myPhoms, _myEatCards))
		{
			_myPhoms.clear();
			_myPhoms.insert(_myPhoms.begin(), phomsHistory.begin(), phomsHistory.end());
			sendMyPhomToServer(GetRoomID(), GetBoardID(), _myPhoms, _userSendPhoms);
		}
	}

	addMyCardsToMyBox(false);
}
void PhomScreen::onStartGame(ubyte interval, VecByte myCards, int moveFirst, int downFirst)
{
	resetGame();

	std::string test = "";
	for (size_t i = 0; i < myCards.size(); i++)
	{
		test += StringUtils::format(" %d", myCards.at(i));
	}
	CCLOG("onStartGame %s", test.c_str());

	_totalTimes = interval;
	K_VEC_INSERT(_myDealCards, myCards);

	for (auto c : myCards)
	{
		_cardPhomBox[0]->addFreeCard(c);
	}
	_cardPhomBox[0]->layoutFreeCard();
	setDownFirstPlayer(downFirst);
	setMovePlayer(downFirst);
	showGameButton(false, false, false, downFirst == GameController::myInfo->IDDB, false, false);
	updateAndLayoutMyPhomBox();
	OnHideCountDown();
}

void PhomScreen::onFire(int currentPlayer, int nextPlayer, int fireCardID, int numCards)
{
	_cur_fire_cardID = fireCardID;
	_last_move_player = _cur_move_player;

	int fireSeat = GetSeatFromPlayerID(currentPlayer);
	if (fireSeat == 0) {
		_myFiredCards.push_back(fireCardID);
		K_VEC_REMOVE(_myFreeCards, fireCardID);
	}

	//removeCardFromBox(fireSeat, fireCardID, CardPhomBox.FLOOR_FREE, false);
	_cardPhomBox[fireSeat]->fireCard(fireCardID);
	getCard(fireCardID)->showHighlight(true);
/*	if (fireSeat == 0)
		dealCards.get(fireCardID).setPosition(x, y);
	addCardToBox(fireSeat, fireCardID, CardPhomBox.FLOOR_FIRED, false,
		isUpdateMyPhom)*/;
	setMovePlayer(nextPlayer);

	//

	if (nextPlayer == _myInfo->GetIDDB()) {
		// đến lượt mình
		// btnFire.setDisabled(true);
		showGameButton(!_isDownMyPhom, true, checkCanEat(numCards), false,
			false, false);
	}
	if (currentPlayer == _myInfo->GetIDDB()) {
		// mình vừa đánh ,nếu numCards >= 4 tức là đã hạ phỏm thì ko đc
		// làm gì nữa
		showGameButton(!_isDownMyPhom && _myFiredCards.size() < 4, false,
			false, false, false, false);
	}
}

int PhomScreen::getPanelBottomY()
{
	return p_panel_bottom->GetPosition().y;
}

void PhomScreen::setDownFirstPlayer(int userID)
{
	_down_first_player = userID;
}

void PhomScreen::showGameButton(bool changePhom, bool getCard, bool eat, bool fire, bool downPhom, bool sendCard)
{
	p_btn_change_phom->SetVisible(false);
	p_btn_get_card->SetVisible(getCard);
	p_btn_eat_card->SetVisible(eat);
	p_btn_fire_card->SetVisible(fire);
	p_btn_fire_card->SetInteractive(fire);
	p_btn_down_phom->SetVisible(downPhom);
	p_btn_send_card->SetVisible(sendCard);


	if (getCard && !eat) {
		p_btn_fire_card->SetVisible(true);
		p_btn_fire_card->SetInteractive(false);
	}	

	layoutListBtn();
}

void PhomScreen::layoutListBtn()
{
	Vec2 btn_size = _list_btns[0]->GetSize();
	int posY = _list_btns[0]->GetPosition().y;
	int num_btn_visible = 0;
	const int off_btn = 4;
	Vec2 game_size = GetGameSize();
	for (size_t i = 0; i < _list_btns.size(); i++)
	{
		if (_list_btns[i]->IsVisible()) {
			_list_btns[i]->SetPosition(Vec2(game_size.x - off_btn - (btn_size.x + off_btn) * num_btn_visible, posY));
			num_btn_visible++;
		}
	}
}

/**
* Thực hiện gửi phỏm của mình lên server, nếu ở vòng hạ phỏm, mình có bài
* gửi trog phỏm thì phải gửi luôn cái phỏm có bài gửi lên mới đúng dc
*
* @param roomID
* @param boardID
* @param currentPhoms
* @param userSendPhoms
*/
void PhomScreen::sendMyPhomToServer(byte roomID, byte boardID,
	std::vector<Phom> currentPhoms,
	std::map<int, std::vector<std::vector<unsigned char>>> userSendPhoms) {
	std::vector<Phom> phoms;
	std::vector<std::vector<unsigned char>> mySendPhoms = userSendPhoms[GameController::myInfo->IDDB];
	if (!mySendPhoms.empty()) {
		// đối với từng phỏm của mình, nếu có lá bài chứa trong phỏm
		// gửi hiện tại thì add mấy lá bài gửi đó vô rồi sort lại
		byte id;
		VecByte cards;
		Phom ph;
		for (size_t i = 0; i < phoms.size(); i++) {
			ph = phoms.at(i);
			if (!ph._cards.empty()) {
				for (size_t j = 0; j < mySendPhoms.size(); j++) {
					cards = mySendPhoms.at(j);
					if (!cards.empty()) {
						if (std::find(cards.begin(), cards.end(), ph._cards.front()) != cards.end()) {
							// send phỏm này có chứa bài phỏm của mình tại i
							// thì lấy hết các lá bài không nằm trong phỏm
							// của mình add vào phỏm của mình
							for (size_t k = 0; k < cards.size(); k++) {
								id = cards.at(k);
								if (std::find( ph._cards.begin(), ph._cards.end(), cards.at(k)) != ph._cards.end()) {
									ph._cards.push_back(id);
								}
							}
							CardUtils::sortByValue_CRChB(true, ph._cards);
						}
					}
				}
			}
		}
		PhomService::refreshPhoms(roomID, boardID, phoms);
	}
	else {
		// không có bài gửi thì gửi phỏm lên luôn
		PhomService::refreshPhoms(roomID, boardID, currentPhoms);
	}
}

/**
* kiem tra xem tat ca card an co nam trong phom kh
* */
bool PhomScreen::checkHaveEatCardInAllPhom(std::vector<Phom> phoms, VecByte card) {
	int sum = 0;
	for (size_t i = 0; i < card.size(); i++) {
		if (checkHaveEatCard(phoms, card.at(i)))
		{
			sum++;
		}
	}
	if (sum == card.size())
	{
		return true;
	}
	else
		return false;
}

/**
* kiem tra xem bai an co nam  trong phom kh
* */
bool PhomScreen::checkHaveEatCard(std::vector<Phom> phoms, byte card) {
	for (size_t i = 0; i < phoms.size(); i++) {

		if (std::find(phoms.at(i)._cards.begin(), phoms.at(i)._cards.end(),card) != phoms.at(i)._cards.end())
			return true;
	}
	return false;
}

/**
* chỉ xử lý việc add các lá bài từ myFreeCards,myFiredCards,myPhoms vào box
* của mình
*/
void PhomScreen::addMyCardsToMyBox(bool addSendCard) {
	// trước tiên remove những lá ko còn nằm trong nọc hiện tại (để bài bay
	// ko phải bay từ giữa bàn)
	//Log.log(TAG, "addMyCardsToMyBox freecard" + myFreeCards);
	//Log.log(TAG, "addMyCardsToMyBox phomcard" + myPhoms);
	//Log.log(TAG, "addMyCardsToMyBox phomcard" + myPhomsDown);
	_cardPhomBox[0]->clearBox();
	removeCardInPhomFreeCard();
	Card* c;
	for (size_t i = 0; i < _myFreeCards.size(); i++) {
		_cardPhomBox[0]->addFreeCard(_myFreeCards.at(i));
	}
	Phom phom;
	for (size_t i = 0; i < _myPhoms.size(); i++) {
		phom = _myPhoms.at(i);
		for (int j = (int)phom._cards.size() - 1; j >= 0; j--) {
			c = getCard(phom._cards.at(j));
			_cardPhomBox[0]->addPhomToFreeCard(c);
		}
	}

	for (size_t i = 0; i < _myPhomsDown.size(); i++) {
		phom = _myPhomsDown.at(i);
		CardUtils::sortByValue_CRChB(true, phom._cards);
		{
			{
				for (int j = (int)phom._cards.size() - 1; j >= 0; j--) {
					_cardPhomBox[0]->addPhomCard(phom._cards.at(j));
				}
			}
		}

	}

	for (size_t i = 0; i < _myFiredCards.size(); i++) {
		_cardPhomBox[0]->addFireCard(_myFiredCards.at(i));
	}

	sortFreeCard();
	_cardPhomBox[0]->layoutBox();
	// highLightPhomInFreeCard();
/*	Log.log("addMyCardsToMyBox",
		".........................................")*/;
	//GameController.playSound(iWinSound.CHIABAI);
	if (addSendCard) {
		if (_userSendPhoms.find(GameController::myInfo->IDDB) != _userSendPhoms.end()) {
			addSendPhomsToCardBox(GameController::myInfo->IDDB);
		}
	}
	// code moi
	// highLightPhomInFreeCard();
	// sortFreeCard();
}

/**
* add bài phỏm gửi đã lưu vào khi tái phỏm (userSendPhoms.size > 0)
*
* @param userID
*/
void PhomScreen::addSendPhomsToCardBox(int userID) {
	if (userID > -1) {
		int seat = GetSeatFromPlayerID(userID);
		if (seat > -1) {
			std::vector<std::vector<unsigned char>> listPhomCards = _userSendPhoms.at(userID);
			Card* c = nullptr;
			byte id;
			VecByte cards;
			if (!listPhomCards.empty()) {
				int count = 0;
				for (size_t i = 0; i < listPhomCards.size(); i++) {
					cards = listPhomCards.at(i);
					if (!cards.empty()) {
						for (size_t j = 0; j < cards.size(); j++) {
							id = cards.at(j);
							if (id > -1) {
								// remove lá addCard ra khỏi 2 bài rác
								// và bài của mình
								auto it = std::find(_myFreeCards.begin(), _myFreeCards.end(), id);
								if (it != _myFreeCards.end()) {
									_myFreeCards.erase(it);
									count++;
								}
							}
						}
					}
				}

				if (count > 0) {
					_cardPhomBox[0]->clearFreeCard();

					for (size_t i = 0; i < _myFreeCards.size(); i++) {
						_cardPhomBox[0]->addFreeCard(_myFreeCards.at(i));
					}

					// cardPhomBoxs[0].layoutBox(true);
					addMyCardsToMyBox(false);
				}

				//_cardPhomBox[seat]->clearPhomCard();

				for (size_t i = 0; i < listPhomCards.size(); i++) {
					cards = listPhomCards.at(i);
					if (!cards.empty()) {
						CardUtils::sortByValue_CRChB(true, cards);
						_cardPhomBox[seat]->replacePhom(cards);
					}
				}
				_cardPhomBox[seat]->layoutBox();
				//GameController.playSound(iWinSound.CHIABAI);
			}
		}
	}
}
/**
* xoa cac card da nam trong phom va van nam trong free card
*
* */
void PhomScreen::removeCardInPhomFreeCard() {
	for (size_t i = 0; i < _myPhoms.size(); i++) {
		for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) {
			auto it = std::find( _myFreeCards.begin(), _myFreeCards.end(), _myPhoms.at(i)._cards.at(j));
			if (it != _myFreeCards.end()) {
				_myFreeCards.erase(it);
			}
		}
	}

}


/**
* sap xep lai bai rac cua minh theo kieu phom, doi, rac,
*
* */
void PhomScreen::sortFreeCard() {
	// add phom vao phombox

	// add toan bo card du vao
	// het add code test
	// kiem tra card phom co bi trum khong 
	// fix bug UI card bi mat do co phom trung reset lai card de trach bug nay
	// kiem tra lai 1 lan nua xem card co bi trung kh
	removeSameValue(_myFreeCards);
	checkandfixhidecard();
	if (!_isDownMyPhom)
		PhomUtils::addMoreCardIntoPhom(_myPhoms, _myFreeCards);
	removeCardInPhomFreeCard();
	_cardPhomBox[0]->setMyPhomEat(_myPhoms, _myEatCards);
	//Log.log(TAG, "1 sortFreeCard myFreeCards " + myFreeCards);
	//Log.log(TAG,
	//	"1 sortFreeCard freeCardsPhomBox "
	//	+ cardPhomBoxs[0].getGroup(CardPhomBox.FLOOR_FREE));
	VecByte cardPhom;

	for (size_t i = 0; i < _myPhoms.size(); i++) {
		for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) 
		{
				cardPhom.push_back(_myPhoms.at(i)._cards.at(j));
		}
	}
	/**
	* ket qua sort bai doi va bai rac khong chua phom
	* */
	VecByte result;
	VecByte resultDoi;
	VecByte resultCaSanh;
	VecByte resultFree;

	// tim doi dua ra truoc
	for (int i = 0; i < (int)_myFreeCards.size() - 1; i++) {
		if (_myFreeCards.at(i) / 4 == _myFreeCards.at(i + 1) / 4) {
			if(!K_VEC_CONTAIN(result, _myFreeCards.at(i))){
				resultDoi.push_back(_myFreeCards.at(i));
				result.push_back(_myFreeCards.at(i));
			}
			if(!K_VEC_CONTAIN(result, _myFreeCards.at(i+1))){
				resultDoi.push_back(_myFreeCards.at(i + 1));
				result.push_back(_myFreeCards.at(i + 1));
			}
		}
	}
	// tim 2 con co the tao thanh phom
	for (size_t i = 0; i < _myFreeCards.size(); i++)
		for (size_t j = 0; j < _myFreeCards.size(); j++) {
			if (!K_VEC_CONTAIN(result, _myFreeCards.at(i))
				&& !K_VEC_CONTAIN(result, _myFreeCards.at(j))) {
				if (_myFreeCards.at(i) % 4 == _myFreeCards.at(j) % 4
					&& abs(_myFreeCards.at(i) / 4
						- _myFreeCards.at(j) / 4) <= 2
					&& abs(_myFreeCards.at(i) / 4
						- _myFreeCards.at(j) / 4) > 0) {

					result.push_back(_myFreeCards.at(i));
					result.push_back(_myFreeCards.at(j));

					resultCaSanh.push_back(_myFreeCards.at(i));
					resultCaSanh.push_back(_myFreeCards.at(j));
				}
			}
		}
	// sap lai cac con con lai
	for (size_t i = 0; i < _myFreeCards.size(); i++) {
		if (!K_VEC_CONTAIN(result, _myFreeCards.at(i))) {
			{
				result.push_back(_myFreeCards.at(i));
				resultFree.push_back(_myFreeCards.at(i));
			}
		}
	}
	// thuc hien add them card free vao cac doi de tao thanh ca moi
	result.clear();
	//result = addMoreCardIntoCa(resultDoi, resultCaSanh, resultFree);
	//result.addAll(addMoreCardIntoCa(resultDoi, resultCaSanh, resultFree));
	VecByte add_cards = addMoreCardIntoCa(resultDoi, resultCaSanh, resultFree);
	K_VEC_INSERT(result, add_cards);
	K_VEC_INSERT(result, resultFree);

	VecCard resultCard;
	//		for (int i = 0; i < result.size; i++) {
	//			for (int j = 0; j < cardPhomBoxs[0].getList(CardPhomBox.FLOOR_FREE).size; j++) {
	//				if (cardPhomBoxs[0].getList(CardPhomBox.FLOOR_FREE).get(j).id == result
	//						.get(i)) {
	//					resultCard.add(cardPhomBoxs[0].getList(
	//							CardPhomBox.FLOOR_FREE).get(j));
	//				}
	//			}
	//		}

	for (size_t i = 0; i < result.size(); i++) {
		resultCard.push_back(getCard(result.at(i)));
	}

	VecCard resultPhom;

	for (size_t i = 0; i < cardPhom.size(); i++) {
		resultPhom.push_back(getCard(cardPhom.at(i)));
	}
	_cardPhomBox[0]->clearFreeCard();
	_cardPhomBox[0]->addFreeCard(resultPhom);
	_cardPhomBox[0]->addFreeCard(resultCard);
	//_cardPhomBox[0]->unselectFreeCards();
}

void PhomScreen::removeSameValue(VecByte& byte_array)
{
	for (size_t i = 0; i < byte_array.size(); i++)
	{
		for (size_t j = i + 1; j < byte_array.size(); j++)
		{
			if (byte_array[i] == byte_array[j])
			{
				byte_array.erase(byte_array.begin() + j);
			}
		}
	}

}

void PhomScreen::checkandfixhidecard()
{
	if (_myPhoms.size() == 2)
	{
		if (PhomUtils::checkPhomHaveSameCard(_myPhoms.at(0), _myPhoms.at(1)))
			updateAndLayoutMyPhomBox();
	}

	if (_myPhoms.size() == 3)
	{
		if (PhomUtils::checkPhomHaveSameCard(_myPhoms.at(0), _myPhoms.at(1), _myPhoms.at(2)))
			updateAndLayoutMyPhomBox();
	}
}

/**
* add them card vao ca
* */
VecByte PhomScreen::addMoreCardIntoCa(VecByte resultDoi,
	VecByte resultCaSanh, VecByte& resultFree) {
	VecByte resultCard;
	if (resultDoi.size()>0)
		for (size_t i = 0; i < resultDoi.size() - 1; i += 2) {
			VecByte vec_temp;
			if (resultDoi.at(i) < resultDoi.at(i + 1)) {
				vec_temp = addcardInOnCa(resultDoi.at(i), resultFree, 0);
				K_VEC_INSERT(resultCard, vec_temp);
				vec_temp = addcardInOnCa(resultDoi.at(i + 1), resultFree, 1);
				K_VEC_INSERT(resultCard, vec_temp);
			}
			else {
				vec_temp = addcardInOnCa(resultDoi.at(i + 1), resultFree, 0);
				K_VEC_INSERT(resultCard, vec_temp);
				vec_temp = addcardInOnCa(resultDoi.at(i), resultFree, 1);
				K_VEC_INSERT(resultCard, vec_temp);
			}

		}
	if (resultDoi.size() == 1)
	{
		K_VEC_INSERT(resultCard, resultDoi);
	}
	if (resultCaSanh.size()>0)
		for (size_t i = 0; i < resultCaSanh.size() - 1; i += 2) {
			VecByte vec_temp;
			if (resultCaSanh.at(i) < resultCaSanh.at(i + 1)) {
				vec_temp = addcardInOnCa(resultCaSanh.at(i), resultFree, 0);
				K_VEC_INSERT(resultCard, vec_temp);
				vec_temp = addcardInOnCa(resultCaSanh.at(i + 1), resultFree, 1);
				K_VEC_INSERT(resultCard, vec_temp);
			}
			else {
				vec_temp = addcardInOnCa(resultCaSanh.at(i + 1), resultFree, 0);
				K_VEC_INSERT(resultCard, vec_temp);
				vec_temp = addcardInOnCa(resultCaSanh.at(i), resultFree, 1);
				K_VEC_INSERT(resultCard, vec_temp);
			}
		}
	if (resultCaSanh.size() == 1)
	{
		K_VEC_INSERT(resultCard, resultCaSanh);
	}
	return resultCard;
}

/**
* xu ly add cac con co the tao thanh ca type 0: no la con nho 1 no la con
* lon
* */
VecByte PhomScreen::addcardInOnCa(byte idCard, VecByte& freeCard, int type) {
	VecByte resultCardAdd;
	resultCardAdd.push_back(idCard);
	bool canaddMore = false;
	do {
		canaddMore = false;
		for (size_t i = 0; i < resultCardAdd.size(); i++) {
			VecByte temp;
			temp.push_back(resultCardAdd.at(i));
			VecByte list_card = cardCanMakeCa(temp);
			for (size_t j = 0; j < freeCard.size(); j++) {
				if (K_VEC_CONTAIN(list_card, freeCard.at(j))) {
					resultCardAdd.push_back(freeCard.at(j));
					K_VEC_REMOVE(freeCard, freeCard.at(j));
					canaddMore = true;
				}
			}
		}
	} while (canaddMore);

	K_VEC_REMOVE(resultCardAdd, idCard);
	CardUtils::sortByValue_CRChB(true, resultCardAdd);

	if (type == 0) {
		resultCardAdd.push_back(idCard);
	}
	else {
		resultCardAdd.insert(resultCardAdd.begin(), idCard);
	}
	return resultCardAdd;
}

/**
* chi lay ra cac card co the tao thanh card
* */
VecByte PhomScreen::cardCanMakeCa(VecByte idCard) {
	VecByte temCardCanMakeCa;
	int value;
	for (size_t i = 0; i < idCard.size(); i++) {
		for (int j = -2; j <= 2; j++) {
			value = (idCard.at(i) / 4 + j) * 4 + idCard.at(i) % 4;
			if (value >= 0
				&& value <= 51)
				temCardCanMakeCa.push_back((byte)value);
		}
	}
	return temCardCanMakeCa;
}


void PhomScreen::resetGame()
{
	_isDownMyPhom = false;
	_userIdU = -1;
	setDownFirstPlayer(-1);

	for (int i = 0; i < p_max_player; i++)
	{
		_cardPhomBox[i]->reset();
	}

	_myFreeCards.clear();
	_myFiredCards.clear();
	_myEatCards.clear();
	_myPhoms.clear();
	_myPhomsDown.clear();
	_myDealCards.clear();
	_winnerID = -1;
	_winScore = 1000;
	_countFinishPlayer = 0;
	_last_action = -1;
}

/**
*
* @param lastUserNumFiredCards
*            : số lá bài người trước đó đánh
* @return
*/
bool PhomScreen::checkCanEat(int lastUserNumFiredCards) {
	bool canEat = false;
	///*
	//* nếu người ta đánh và đến lượt mình, nếu ăn được thì nhấc 2 lá cần để
	//* ăn lên
	//*/

	if (_cur_move_player == _myInfo->getPlayer()->IDDB && lastUserNumFiredCards < 4) {
		std::vector<Phom> newPhoms;
		VecByte newFreeCards;
		VecByte needCards;

		// chổ này đúng nhưng thuật toán chưa ngon nên bị sai
		// nếu chưa hạ phỏm thì lấy tất cả bài để check,nếu đã hạ phỏm thì
		// ko được phá phỏm, lấy bài rác để check (lúc này ko tính bài ăn vì
		// chỉ tạo phỏm bằng bài rác hiện tại)
		// canEat = Phom.canEat(currentFireCardID, isDownMyPhom ? null
		// : myEatCards, isDownMyPhom ? myFreeCards : myDealCards,
		// needCards, newPhoms, newFreeCards);

		/*
		* nếu chưa hạ phỏm, sẽ tìm 2 lần (nguyên bộ bài và chỉ bài rác), sẽ
		* lấy lần nào tạo được nhiều phỏm nhất. Nếu đã hạ phỏm thì chỉ tìm
		* trong bài rác
		*/

		if (!_isDownMyPhom) {
			std::vector<Phom> newPhoms1;
			VecByte newFreeCards1;
			VecByte needCards1;
			std::vector<Phom> newPhoms2;
			VecByte newFreeCards2;
			VecByte needCards2;
			bool canEat1, canEat2;
			canEat1 = PhomUtils::canEat(_cur_fire_cardID, _myEatCards,
				_myDealCards, needCards1, newPhoms1, newFreeCards1);
			// canEat2 = Phom.canEat(currentFireCardID, null, myFreeCards,
			// needCards2, newPhoms2, newFreeCards2);
			VecByte tmp_array;
			canEat2 = PhomUtils::canEat(_cur_fire_cardID, tmp_array,
				_myFreeCards, needCards2, newPhoms2, newFreeCards2);
			if (canEat1 != canEat2) {
				// một trong 2 thằng ăn đc lấy thằng ăn được
				canEat = true;
				if (canEat1) {
					needCards = needCards1;
					newPhoms = newPhoms1;
					newFreeCards = newFreeCards1;
				}
				else {
					needCards = needCards2;
					newPhoms = newPhoms2;
					newFreeCards = newFreeCards2;
				}
			}
			else if (canEat1) {
				canEat = true;
				// 2 thằng đều ăn được, lấy thằng nhiều phỏm hơn
				if (newPhoms1.size() >= newPhoms2.size()) {
					needCards = needCards1;
					newPhoms = newPhoms1;
					newFreeCards = newFreeCards1;
				}
				else {
					needCards = needCards2;
					newPhoms = newPhoms2;
					newFreeCards = newFreeCards2;
				}
			}
		}
		else {
			// khi da ha phom se con lai phom chua ha va bai rac
			VecByte allFreeCard;
			for (size_t i = 0; i < _myPhoms.size(); i++)
				K_VEC_INSERT(allFreeCard, _myPhoms.at(i)._cards);
			K_VEC_INSERT(allFreeCard, _myFreeCards);
			VecByte tmp_array;
			canEat = PhomUtils::canEat(_cur_fire_cardID, tmp_array, allFreeCard,
				needCards, newPhoms, newFreeCards);
		}
		// dua la bai co the an len tren
		if (canEat) {
			for (size_t i = 0; i < needCards.size(); i++) {
				_cardPhomBox[0]->selectFreeCard(needCards.at(i));
			}
			// float timeAction=1.12f;
			// SequenceAction sequence=new
			// SequenceAction(Actions.fadeIn(2*timeAction),
			// Actions.fadeOut(timeAction));
			// dealCards.get(currentFireCardID).addAction(Actions.forever(sequence));
			Card* cardClone = nullptr;
			int position = 0;
			for (size_t i = 0; i < p_max_player; i++) {
				if (_cardPhomBox[i]->hasFireCard(_cur_fire_cardID))
				{
					position = i;
					cardClone = getCard(_cur_fire_cardID);
					break;
				}
			}
			if (cardClone != nullptr) {
				//imgEffectEatCard.setRotation(0);
				//imgEffectEatCard.clearActions();
				//float x1;
				//float y1;
				//if (cardPhomBoxs[position].getList(CardPhomBox.FLOOR_FIRED).size > 1) {
				//	x1 = cardPhomBoxs[position]
				//		.getList(CardPhomBox.FLOOR_FIRED)
				//		.get(cardPhomBoxs[position]
				//			.getList(CardPhomBox.FLOOR_FIRED).size - 2)

				//		.localToStageCoordinates(new Vector2(0, 0)).x;
				//	y1 = cardPhomBoxs[position].getGroup(
				//		CardPhomBox.FLOOR_FIRED).getY()
				//		+ cardPhomBoxs[position].getGroup(
				//			CardPhomBox.FLOOR_FIRED).getHeight();
				//	x1 += cardClone.getWidth() / 2;
				//}
				//else {
				//	x1 = cardPhomBoxs[position].getGroup(
				//		CardPhomBox.FLOOR_FIRED)
				//		.localToStageCoordinates(new Vector2(0, 0)).x
				//		+ (cardPhomBoxs[position].getGroup(
				//			CardPhomBox.FLOOR_FIRED).getWidth() - cardClone
				//			.getWidth()) / 2;
				//	y1 = cardPhomBoxs[position].getGroup(
				//		CardPhomBox.FLOOR_FIRED).getY()
				//		+ cardPhomBoxs[position].getGroup(
				//			CardPhomBox.FLOOR_FIRED).getHeight();
				//}
				//float timeActionMove = 0.2f;
				//int type = 1;
				//if (position == 1 || position == 2)
				//{
				//	imgEffectEatCard.setRotation(180);
				//	y1 = y1 - cardClone.getHeight() - imgEffectEatCard.getHeight();
				//	type = -1;
				//	//imgEffectEatCard.setPosition(x1, y1-cardClone.getHeight()-imgEffectEatCard.getHeight());
				//}

				//imgEffectEatCard.setPosition(x1, y1);

				//SequenceAction sequence = new SequenceAction(
				//	Actions.moveTo(x1, y1, timeActionMove), Actions.moveTo(
				//		x1, y1 + type*cardClone.getHeight() / 4,
				//		timeActionMove));
				//imgEffectEatCard.addAction(Actions.forever(sequence));
				//gameLayer.addActor(imgEffectEatCard);
			}
			/*dealCards.get(currentFireCardID)
				.setTouchable(Touchable.enabled);
			dealCards.get(currentFireCardID).addListener(
				new ClickListener(){
				@Override
				public void clicked(InputEvent event, float x,
					float y) {
				Log.log(TAG, "click eat");

				doAction(ACTION_EAT);

			}
			});*/

		}
	}
	return canEat;
}


DealCardLayer* PhomScreen::getDealLayer()
{
	return _deal_card_layer;
}

SpecialCardsBound* PhomScreen::getFreeSpecialCardBound()
{
	for (SpecialCardsBound* s : _card_bounds) {
		if (!s->isVisible()) return s;
	}
	SpecialCardsBound* card_bound = new SpecialCardsBound();
	card_bound->init();
	card_bound->autorelease();
	NODE(p_panel_game_card)->addChild(card_bound);
	_card_bounds.push_back(card_bound);
	return card_bound;
}

void PhomScreen::clearAllCardBound()
{
	for (SpecialCardsBound* s : _card_bounds) {
		s->setVisible(false);
	}
}

void PhomScreen::actionFire()
{
	if (_cur_move_player == _myInfo->GetIDDB())
	{
		VecByte selectedCards = _cardPhomBox[0]->getCardSelected();
		if (selectedCards.size() == 1) {
			if(K_VEC_CONTAIN(_myEatCards, selectedCards.at(0))){
				//GameController.playSound(iWinSound.POONG);
				// ko được đánh bài ăn
				getPlayerGroup(_myInfo->GetIDDB())->SetTextChat(LangMgr->GetString("can't_fire_the_eaten"));
			}
			else {
				if (!_isDownMyPhom) {
					std::vector<Phom> tmp_phom;
					VecByte tmp_vec_byte;
					if (PhomUtils::canFire(selectedCards.at(0), _myDealCards,
						_myEatCards, _myFreeCards, tmp_phom, tmp_vec_byte)) {
						PhomService::fire(_roomID, _boardID,
							selectedCards.at(0));
						p_btn_fire_card->SetInteractive(false);
						p_btn_change_phom->SetInteractive(false);
						p_btn_create_phom->SetInteractive(false);
					}
					else {
						//GameController.playSound(iWinSound.POONG);
						// không thể đánh vì phỏm mới còn bài ăn nằm
						// ngoài
						//onChat(myInfo.IDDB, L.gL(956));
						getPlayerGroup(_myInfo->GetIDDB())->SetTextChat(LangMgr->GetString("can't_break_this_rummy"));
					}
				}
				// theo ru moi do van con phom chua ha ben duoi nhung
				// van duoc danh
				else {
					// đã hạ phỏm, chỉ đc đánh bài rác
					// gio nay kh con phom an nua nen kh can check
					// if (myFreeCards.contains(selectedCards.get(0),
					// true))
					{
						PhomService::fire(_roomID, _boardID,
							selectedCards.at(0));
						p_btn_fire_card->SetInteractive(false);
						//cardPhomBoxs[0]
						//	.setTouchable(Touchable.disabled);
						p_btn_change_phom->SetInteractive(false);
						p_btn_create_phom->SetInteractive(false);
					}
					// else {
					// GameController.playSound(iWinSound.POONG);
					// onChat(myInfo.IDDB, L.gL(956));
					// }
				}
			}
		}
		else if (selectedCards.size() > 1) {
			//GameController.playSound(iWinSound.POONG);
			onChat(_myInfo->GetIDDB(), LangMgr->GetString("you_can_only_give_one").GetString());
		}
		else {
			//GameController.playSound(iWinSound.POONG);
			onChat(_myInfo->GetIDDB(), LangMgr->GetString("choose_a_card_to_fire").GetString());
		}
	}
	_last_action = ACTION_FIRE;
}

void PhomScreen::actionGetCard()
{
	PhomService::getCard(_roomID, _boardID);
	p_btn_eat_card->SetInteractive(false);
	p_btn_get_card->SetInteractive(false);
	p_btn_change_phom->SetInteractive(false);
	p_btn_create_phom->SetInteractive(false);
	_last_action = ACTION_GETCARD;
}

/**
* rút bài
*
* @param userID
*            : thằng rút = current move player
* @param cardID
*            : lá bài rút
*/
void PhomScreen::onGetCard(int userID, byte cardID) {
	CCLOG("Get Card: %d %d", userID, cardID);
	_card_get_now = cardID;
	getCard(_card_get_now)->showHighlight(true);

	_on_reconnect = false;
	_is_get_start = true;

	for (int i = 0; i < p_max_player; i++) {
		_cardPhomBox[i]->layoutBox();
	}
	//for (int i = 0; i < cardPhomBoxs.length; i++) {
	//	cardPhomBoxs[i].clearListenInFire();
	//}
	// xoa action cham an cua con bai vua danh
	/*if (dealCards.get(currentFireCardID) != null)
		dealCards.get(currentFireCardID).clearActions();
	imgEffectEatCard.remove()*/;
	/*Log.log("onGetCard",
		"thằng rút:" + getPlayerName(userID) + " ,"
		+ Card.card(cardID, Card.TYPE_BAICAO_XIZACH_PHOM));*/
	//cardPhomBoxs[0].layoutBox(false);

	int seat = GetSeatFromPlayerID(userID);
	if (userID == _myInfo->GetIDDB()) {
		showGameButton(!_isDownMyPhom, false, false,
			_myFiredCards.size() != 3, _myFiredCards.size() == 3, false);
		p_btn_eat_card->SetInteractive(true);
		p_btn_get_card->SetInteractive(true);
		//cardPhomBoxs[0].setTouchable(Touchable.enabled);
		p_btn_change_phom->SetInteractive(true);
		p_btn_create_phom->SetInteractive(true);

	
		_myDealCards.push_back(cardID);
		_myFreeCards.push_back(cardID);
		// xử lý tìm phỏm trong bài rác hiện tại, ko phá phỏm ,để tránh
		// tình trạng phá phỏm ăn mà user muốn
		// chỉ cần gọi updateAndLayoutMyPhomBox(); nếu muốn để tự động xử lý
		VecByte newFree;
		VecByte test_array;
		std::vector<Phom> phoms = PhomUtils::findPhoms(_myFreeCards, test_array,
			newFree, true);
		if (phoms.size() > 0) {
			K_VEC_INSERT(_myPhoms, phoms);
			_myFreeCards.clear();
			K_VEC_INSERT(_myFreeCards, newFree);

		}
		CardUtils::sortByValue_CRChB(true, _myFreeCards);
		addMyCardsToMyBox(true);

		if (_myFiredCards.size() == 3) {
			// khi đa u thi kh can nang card phom len nua
			if (_userIdU < 0)
				_cardPhomBox[0]->selectAllCardPhom(_myPhoms);
		}
		// moi lan sap bai co tim phom phai gui len server 
		sendMyPhomToServer(_roomID, _boardID, _myPhoms, _userSendPhoms);
	}
	else {
		//cardPhomBoxs[seat].addCard(c, CardPhomBox.FLOOR_FREE);
		//cardPhomBoxs[seat].layoutBox(true);
	}
	// if (userID == myInfo.IDDB&&myFiredCards.size == 3) {
	// selectAllCardPhom();
	// }
}

/**
* trả về khi có thằng ăn bài vừa đánh
*
* @param downFirst
*            : thằng hạ bài đầu tiên mới
*/
void PhomScreen::onEatCard(int downFirst) {
	_on_reconnect = false;
	NODE(getCard(_cur_fire_cardID))->stopAllActions();
	//imgEffectEatCard.remove();
	//if (cardPhomBoxs[0] != null)
	//	cardPhomBoxs[0].setReconnect(onReconnect);
	//Log.log("onEatCard",
	//	"downFirst:"
	//	+ getPlayerName(downFirst)
	//	+ " ,bài ăn:"
	//	+ Card.card(currentFireCardID,
	//		Card.TYPE_BAICAO_XIZACH_PHOM));
	//cardPhomBoxs[0].layoutBox(false);
	// highLightPhomInFreeCard();

	if (_last_move_player == _myInfo->GetIDDB()) {
		// mình bị ăn
		//GameController.playSound(iWinSound.BI_AN_GA);
	}
	//GameController.playSound(iWinSound.AN_BAI);

	// chuyển lá bài vừa đánh qua cho thằng current , chuyển bài từ thằng
	// downFirst sang thằng lastMove (nếu downFirst!=lastMove)
	int seatLastMove = GetSeatFromPlayerID(_last_move_player);
	int seatCurrentMove = GetSeatFromPlayerID(_cur_move_player);
	int seatDownFirst = GetSeatFromPlayerID(_down_first_player);

	_cardPhomBox[seatLastMove]->removeFiredCard(_cur_fire_cardID);

	if (_cur_move_player != _myInfo->GetIDDB()) 
	{
		_cardPhomBox[seatCurrentMove]->addEatCard(_cur_fire_cardID);
		_cardPhomBox[seatCurrentMove]->layoutEatCard();
		K_VEC_REMOVE(_myFiredCards, _cur_fire_cardID);
	}
	else if(_needCards.size() >= 2 ){
		// xử lý lấy 2 lá bài nâng lúc gửi lên để tạo phỏm,chứ ko tìm phỏm
		// lại, để tránh trường hợp lấy phỏm ko đúng ý user
		//Card c = dealCards.get(currentFireCardID);
		//c.setShow(true);
		//c.setColor(COLOR_EAT_CARD);
		_myEatCards.push_back(_cur_fire_cardID);
		Phom phom;
		phom._cards.push_back(_cur_fire_cardID);
		K_VEC_INSERT(phom._cards, _needCards);
		CardUtils::sortByValue_CRChB(true, phom._cards);
		phom._type = PhomUtils::getPhomTypeAlone(phom._cards, _myEatCards);
		//Log.log("Ăn bài", "Phỏm ăn : " + phom.toString());
		// xóa phỏm chứa 2 lá bài need nếu có,đem các lá bài còn lại trog
		// phỏm bị xóa ra freeCards
		Phom p;
		byte cb;
		byte needCard0 = _needCards.at(0), needCard1 = _needCards.at(1);
		for (size_t i = 0; i < _myPhoms.size(); i++) {
			p = _myPhoms.at(i);
			if(K_VEC_CONTAIN(p._cards, needCard0)
				|| K_VEC_CONTAIN(p._cards, needCard1)) {
				// phỏm này chứa needCards
				for (size_t j = 0; j < p._cards.size(); j++) {
					cb = p._cards.at(j);
					//if (!needCards.contains(cb, true)) {
					if(!K_VEC_CONTAIN(_needCards, cb)) {
						// lá bài phỏm này ko nằm trong needCards, đem
						// nó ra bài rác
						_myFreeCards.push_back(cb);
					}
				}
				_myPhoms.erase(_myPhoms.begin() + i);
				i = 0;
			}
		}

		_myPhoms.push_back(phom);
		//Log.log("Ăn bài", "Phỏm sau khi ăn : " + myPhoms.toString());
		// xóa 2 lá bài need ra khỏi free
		K_VEC_REMOVE(_myFreeCards, _needCards.at(0));
		K_VEC_REMOVE(_myFreeCards, _needCards.at(1));

		CardUtils::sortByValue_CRChB(true, _myFreeCards);

		if (_myPhoms.size() > 0) {
			// trường hợp có 4 con 4 ,3 chuồn, ăn con 5 chuồn ,thì 2 con 4
			// còn
			// thừa rớt xuống bài rác ,nên phải tạo phỏm của bài rác tiếp
			// cho
			// chắc là còn phỏm nào sót lại trong bài rác hay không
			VecByte newFree;
			VecByte tmp_byte;
			std::vector<Phom> tmp_phom = PhomUtils::findPhoms(_myFreeCards, tmp_byte, newFree, true);
			K_VEC_INSERT(_myPhoms, tmp_phom);
			_myFreeCards.clear();
			K_VEC_INSERT(_myFreeCards, newFree);

		}
		// updateAndLayoutMyPhomBox();
		// chi muon sap lai bai nhung khong tim lai phom 
		// tranh truong hop an bai kh dung y 
		// nhung phai gui tiep phom moi tao len cho server
		sendMyPhomToServer(GetRoomID(), GetBoardID(), _myPhoms, _userSendPhoms);
		// kiem tra lai neu co bai an rot ra ngoai thi thuc hien tim lai phom 
		// do gio nay dang uu tien truong hop phom an 
		if (checkHaveEatCardInAllPhom(_myPhoms, _myEatCards))
		{
			addMyCardsToMyBox(true);
		}
		else
		{
			updateAndLayoutMyPhomBox();
		}

		_needCards.clear();
	}

	/*
	* xử lý xoay rác (chuyển bài rác từ thằng hạ bài đầu tiên sang thằng
	* đánh trước đó)
	*/
	if (_down_first_player != downFirst && seatDownFirst > -1) {
		char lastFiredCard_DownFirst = _cardPhomBox[seatDownFirst]->getLastFireCardID();
		if (lastFiredCard_DownFirst > -1
			&& lastFiredCard_DownFirst != _cur_fire_cardID) {

			_cardPhomBox[seatDownFirst]->removeFiredCard(lastFiredCard_DownFirst);
			_cardPhomBox[seatLastMove]->addFireCard(lastFiredCard_DownFirst);

		}
	}

	// downFirst chuyển qua cho thằng kế bên phải downFirst hiện tại
	// (downFirst mới server trả về)
	setDownFirstPlayer(downFirst);

	if (_cur_move_player == _myInfo->GetIDDB()) {
		showGameButton(!_isDownMyPhom, false, false,
			_myFiredCards.size() != 3, _myFiredCards.size() == 3, false);
		p_btn_eat_card->SetInteractive(true);
		p_btn_get_card->SetInteractive(true);
		//cardPhomBoxs[0].setTouchable(Touchable.enabled);
		p_btn_change_phom->SetInteractive(true);
		p_btn_create_phom->SetInteractive(true);

		if (_myFiredCards.size() == 3) {
			// khi đa u thi kh can nang card phom len nua
			//if (_userIdU < 0)
			//	_cardPhomBox[0]->selectAllCardPhom(_myPhoms, _myEatCards);
		}
	}
}

void PhomScreen::OnCardTouchDown()
{
	VecCard free_cards = _cardPhomBox[0]->getFreeCards();
	for (Card *c : free_cards) {
		if (InputMgr->IsPressOnBound(c->GetBound())) {
			NODE(c)->setTag(1);
			return;
		}
	}
}

void PhomScreen::OnCardTouchMove()
{
	VecCard free_cards = _cardPhomBox[0]->getFreeCards();
	for (Card* c : free_cards) {
		if (InputMgr->IsPressOnBound(c->GetBound()) && NODE(c)->getTag() != 1) {
			c->showHighlight(true);
			NODE(c)->setTag(1);
		}
	}
}
void PhomScreen::OnCardTouchEnd()
{
	int numAciton = 0;
	VecCard free_cards = _cardPhomBox[0]->getFreeCards();
	for (Card *c : free_cards) {
		if (NODE(c)->getTag() == 1) {
			c->SelectCardAction(!c->isSelected());
		}
		c->showHighlight(false);
		NODE(c)->setTag(0);
	}


}

void PhomScreen::actionEat()
{
	_needCards.clear();
	VecByte selectedCards = _cardPhomBox[0]->getCardSelected();
	// if (selectedCards.size == 2)
	{
		bool canEat;
		/*
		* dùng phần code này để tự động check tìm 2 con để ăn if
		* (!isDownMyPhom) { canEat = Phom.canEat(currentFireCardID,
		* myEatCards, myDealCards, selectedCards, null, null); }
		* else { Array<Byte> tmpEatCards = new Array<Byte>(1);
		* tmpEatCards.add(currentFireCardID); canEat =
		* Phom.canEat(currentFireCardID, tmpEatCards, myFreeCards,
		* selectedCards, null, null); }
		*/
		VecByte tmpEatCards;
		K_VEC_INSERT(tmpEatCards, _myEatCards);
		tmpEatCards.push_back(_cur_fire_cardID);
		//Array<Byte> tmp3Cards = new Array<Byte>(selectedCards);
		VecByte tmp3Cards;
		K_VEC_INSERT(tmp3Cards, selectedCards);
		tmp3Cards.push_back(_cur_fire_cardID);
		std::vector<Phom> new_phom;
		VecByte new_free_cards;
		canEat = PhomUtils::canMakePhoms(tmp3Cards, tmpEatCards, _myDealCards, new_phom, new_free_cards, true);
		// ///////////////////////////////////
		if (canEat) {
			//_needCards.addAll(selectedCards);
			K_VEC_INSERT(_needCards, selectedCards);
			PhomService::eatCard(_roomID, _boardID, _cur_fire_cardID, selectedCards);
			//cardPhomBoxs[0].setTouchable(Touchable.disabled);
			p_btn_eat_card->SetInteractive(false);
			p_btn_get_card->SetInteractive(false);
			p_btn_change_phom->SetInteractive(false);
			p_btn_create_phom->SetInteractive(false);
		}
		else {
			//GameController.playSound(iWinSound.POONG);
			onChat(_myInfo->GetIDDB(), LangMgr->GetString("lift_two_fit_card_to_eat").GetString());

		}
	}
	_last_action = ACTION_EAT;
}

void PhomScreen::onDownPhom(std::vector<Phom> phoms)
{
	//GameController.playSound(iWinSound.HA_PHOM);
	//cardPhomBoxs[0].layoutBox(false);
	// highLightPhomInFreeCard();
	if (_cur_move_player != _myInfo->GetIDDB()) {
		int seat = GetSeatFromPlayerID(_cur_move_player);
		if (seat > -1) {
			_cardPhomBox[seat]->clearPhomCard();
			Card* c = nullptr;
			Phom phom;
			for (size_t i = 0; i < phoms.size(); i++) {
				phom = phoms.at(i);
				for (size_t j = 0; j < phom._cards.size(); j++) {
					_cardPhomBox[seat]->addPhomCard(phom._cards.at(j));
				}
			}
			_cardPhomBox[seat]->layoutPhomCard();
		}
	}
	else {
		// int seat = getSeatFromPlayerID(currentMovePlayer);
		//cardPhomBoxs[0].setTouchable(Touchable.enabled);

		//cardPhomBoxs[0].disablePhomFloor(true);
		// neu chay thi tat luon false
		if (isNotChay())
			showGameButton(false, false, false, true, false, true);
		else
			showGameButton(false, false, false, true, false, false);
		// hien thi nut gui bai luc nay xu ly
		p_btn_down_phom->SetInteractive(true);
		p_btn_change_phom->SetInteractive(true);
		p_btn_create_phom->SetInteractive(true);

		// xu ly ha phom khi minh ha
		_myPhomsDown.clear();
		K_VEC_INSERT(_myPhomsDown, phoms);
		VecByte cardPhomNotDown;
		bool inPhomDown = false;
		for (size_t i = 0; i < _myPhoms.size(); i++) {
			// xu ly add cac bai khong ha vao bai rac
			for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) {
				inPhomDown = false;
				for (size_t k = 0; k < phoms.size(); k++)
					if(K_VEC_CONTAIN(phoms.at(k)._cards, _myPhoms.at(i)._cards.at(j)))
						inPhomDown = true;
				if (!inPhomDown)
					cardPhomNotDown.push_back(_myPhoms.at(i)._cards.at(j));
			}
		}
		std::vector<Phom> tempPhomNotDown;
		VecByte newFreeCardNotDown;
		VecByte tmp_byte;
		tempPhomNotDown = PhomUtils::findPhoms(cardPhomNotDown,
			tmp_byte, newFreeCardNotDown, true);

		K_VEC_INSERT(_myFreeCards, newFreeCardNotDown);
		_myPhoms.clear();
		K_VEC_INSERT(_myPhoms, tempPhomNotDown);
		//

		_isDownMyPhom = true;
		//_cardPhomBox[0].hiddenHighLightPhomInFreeCard();
		addMyCardsToMyBox(false);

	}

	// add các lá bài gửi vô trở lại (tái phỏm)
	if (_userSendPhoms.find(_cur_move_player) != _userSendPhoms.end()) {
		addSendPhomsToCardBox(_cur_move_player);
	}
}

/**
* tra ve true khi no khong phai chay false khi no chay
* */
bool PhomScreen::isNotChay() {

	if (_myPhoms.size() > 0)
		return true;
	return false;
}

void PhomScreen::actionDownPhom()
{
	if (checkSelectCardHaveAllEat()) {
		std::vector<Phom> phomSend;
		std::vector<Phom> tmp_phoms = getPhomInCardSelect();
		K_VEC_INSERT(phomSend, tmp_phoms);
		if (!_isDownMyPhom)
			if (!checkHaveEatCardInAllPhom(phomSend, _myEatCards))
			{
				// co bai an kh ha
				_cardPhomBox[0]->selectAllCardPhom(_myPhoms);
				onChat(_myInfo->GetIDDB(), LangMgr->GetString("eaten_card_must_be").GetString());
				return;
			}

		// PhomNewService.downPhom(roomID, boardID, myPhoms);
		// gui phom trong phom an va phom duoc chon
		PhomService::downPhom(GetRoomID(), GetBoardID(), phomSend);
		p_btn_down_phom->SetInteractive(false);
		//cardPhomBoxs[0].setTouchable(Touchable.disabled);
		p_btn_change_phom->SetInteractive(false);
		p_btn_create_phom->SetInteractive(false);
		// khi nhan ha phom xoa cac select hien tai
		_cardPhomBox[0]->unselectFreeCards();
	}
	else {
		// co bai an kh ha
		for (size_t i = 0; i < _myEatCards.size(); i++)
			_cardPhomBox[0]->selectFreeCard(_myEatCards.at(i));
		onChat(_myInfo->GetIDDB(), LangMgr->GetString("eaten_card_must_be").GetString());
	}
	_last_action = ACTION_DOWN_PHOM;
}

bool PhomScreen::checkSelectCardHaveAllEat() {
	bool check = true;
	VecByte free_selected = _cardPhomBox[0]->getCardSelected();
	for (size_t i = 0; i < _myEatCards.size(); i++) {
		if(!K_VEC_CONTAIN(free_selected, _myEatCards.at(i)))
			check = false;
	}
	// luc nay la tai phom, cac phom an da ha lan truoc kh can check nua
	if (!check && _myPhomsDown.size() > 0)
		check = true;
	return check;
}

/**
* lay tat ca phom an va phom trong card duoc chon
* */
std::vector<Phom> PhomScreen::getPhomInCardSelect() {

	std::vector<Phom> result;
	VecByte newFree;
	VecByte newFreeBefore;
	VecByte free_selected = _cardPhomBox[0]->getCardSelected();
	// lay phom nam tren card phom co bai an
	for (size_t i = 0; i < _myPhoms.size(); i++) {
		Phom tempPhomResult;
		if (checkHaveEatCard(_myPhoms.at(i))) {
			for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) {
				//if (_cardPhomBox[0]->getSelectedFreeCardIDs().contains(
					//myPhoms.get(i).cards.get(j), true)) {
				if(K_VEC_CONTAIN(free_selected, _myPhoms.at(i)._cards.at(j))){
					tempPhomResult._cards.push_back(_myPhoms.at(i)._cards.at(j));
				}
			}
			if (checkHaveEatCard(tempPhomResult)) {
				if (tempPhomResult._cards.size() >= 3) {
					result.push_back(tempPhomResult);
					for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) {
						if(!K_VEC_CONTAIN(tempPhomResult._cards, _myPhoms.at(i)._cards.at(j)))
							_myFreeCards.push_back(_myPhoms.at(i)._cards.at(j));
					}
				}
				else
					result.push_back(_myPhoms.at(i));
			}
			else {
				result.push_back(_myPhoms.at(i));
			}

		}
	}
	/**
	* add luon card phom khong co bai an vao de xem user co chon kh
	* */
	VecByte freeCardAll;
	for (size_t i = 0; i < _myPhoms.size(); i++) {
		if(!checkContainPhom(result, _myPhoms.at(i))
		&& !checkHaveEatCard(_myPhoms.at(i)))
			for (size_t j = 0; j < _myPhoms.at(i)._cards.size(); j++) {
				freeCardAll.push_back(_myPhoms.at(i)._cards.at(j));
			}
	}
	K_VEC_INSERT(freeCardAll, _myFreeCards);

	for (size_t i = 0; i < free_selected.size(); i++)
		if(K_VEC_CONTAIN(freeCardAll, free_selected.at(i)))
			newFreeBefore.push_back(free_selected.at(i));
	VecByte tmp_byte;
	std::vector<Phom> phoms = PhomUtils::findPhoms(newFreeBefore, tmp_byte, newFree, true);
	if (phoms.size() != 0)
		K_VEC_INSERT(result, phoms);

	//Log.log(TAG, "phom select " + result.toString());


	std::vector<Phom> resultPhom;

	for (size_t i = 0;i<result.size();i++)
	{
		Phom temp;
		VecByte byte_array1, byte_array2;
		std::vector<Phom> tmp_phoms = PhomUtils::findPhoms(result.at(i)._cards, byte_array1, byte_array2, true);
		K_VEC_INSERT(resultPhom, tmp_phoms);
		//resultPhom.addAll(temp.findPhoms(result.get(i).cards, byte_array1, byte_array2, true));
	}


	//Log.log(TAG, "selectPhomDown: " + resultPhom);
	PhomService::refreshPhoms(GetRoomID(), GetBoardID(), resultPhom);
	return resultPhom;
}

/**
* kiem tra xem phom do bai an khong
* */
bool PhomScreen::checkHaveEatCard(Phom phom) {
	for (size_t i = 0; i < phom._cards.size(); i++) {
		for (size_t j = 0; j < _myEatCards.size(); j++)
			if (phom._cards.at(i) == _myEatCards.at(j)) {
				return true;
			}
	}
	return false;
}

/**
* trả về khi có thằng nào kết thúc ,trả về từng thằng 1 (giống tiến lên)
*
* @param finishPlayer
* @param freeCardScore
*            : điểm của bài rác cằm trên tay , <0 : cháy
* @param freeCards
*            : các lá bài rác
* @param moneyPlayer
*            so tien ma no thang
*/
void PhomScreen::onFinish(int finishPlayer, int freeCardScore, VecByte freeCards, int moneyPlayer) {
	//Log.log("onFinish", getPlayerName(finishPlayer) + " ,freeCardScore="
		//+ freeCardScore + " freeCard " + freeCards);

	// xoa action cham an va hieu ung di kem
	/*if (dealCards.get(currentFireCardID) != null) {
		for (int i = 0; i < cardPhomBoxs.length; i++) {
			cardPhomBoxs[i].clearListenInFire();
		}
		dealCards.get(currentFireCardID).clearActions();
		imgEffectEatCard.remove();
	}*/
	// thang thang la thang dc server tra ve dau tien
	if (_winnerID == -1) {
		_winScore = freeCardScore;
		_winnerID = finishPlayer;
		setInfo(GetSeatFromPlayerID(_winnerID), _winScore >= 0
			&& _winScore < 3 ? _winScore : 4);
	}
	// if (freeCardScore >= 0 && freeCardScore < winScore) {
	// winScore = freeCardScore;
	// winnerID = finishPlayer;
	// }
	//if (moneyPlayer > 0)
	//	Log.log(TAG, "OnFinish myPhom " + myPhoms + " phomDown "
	//		+ myPhomsDown);
	_countFinishPlayer++;
	int realPlayer = getNumberPlayer();

	if (_countFinishPlayer == realPlayer) {
		if(_card_get_now != -1)
			getCard(_card_get_now)->showHighlight(false);
		_card_get_now = -1;

		//GameService::getBetRange();
		//Log.log("onFinish", "winnerID : " + getPlayerName(winnerID));
		if (_winScore == 1 || _winScore == 2 || _winScore == 0) {
			// xem lại no co u hay do diem no =1,2
			if (_userIdU != _winnerID)
				setInfo(GetSeatFromPlayerID(_winnerID), 4);
			_userIdU = -1;
		}
		else
		{
			setInfo(GetSeatFromPlayerID(_winnerID), _winScore >= 0
				&& _winScore < 3 ? _winScore : 4);

			//				else
			//				setInfo(getSeatFromPlayerID(winnerID), winScore >= 0
			//						&& winScore < 3 ? winScore : 4);
		}


		_cardPhomBox[0]->unselectFreeCards();
		//cardPhomBoxs[0].SetIsSendCard(false);
		//cardPhomBoxs[0].layoutBox(true);

		//Log.log(TAG, "OnFinish myPhom " + myPhoms + " phomDown "
			//+ myPhomsDown);
		// thuc hien show hieu ung danh cho dua win
		int seat = GetSeatFromPlayerID(_winnerID);
		//playerGroups[seat].showHaloEffectUserWin();
		//for (int i = 0; i < dealCards.size; i++) {
		//	dealCards.get((byte)i).clearActions();
		//}
		for (int i = 0; i < p_max_player; i++) {
			if (_cardPhomBox[i] != nullptr) {
				_cardPhomBox[i]->hideFireCards();
			}
		}
	}

	int seat = GetSeatFromPlayerID(finishPlayer);

	if (seat > -1) {
		if (finishPlayer != _myInfo->GetIDDB()) {
			if (freeCards.size() > 0) {
				for (size_t i = 0; i < freeCards.size(); i++) {
					_cardPhomBox[seat]->addFreeCard(freeCards.at(i));
				}
				_cardPhomBox[seat]->layoutFreeCard();
			}
		}
		else {
			//_cardPhomBoxs[0].setTouchable(Touchable.disabled);
			addMyCardsToMyBox(true);
			//cardPhomBoxs[0].layoutBox(false);
			// highLightPhomInFreeCard();
		}
		//playerGroups[seat].stop();
		if (freeCardScore > 0) {
			setScore(seat, freeCardScore);
		}
		else if (freeCardScore < 0) {
			if (freeCardScore == -1)
			{
				if (_winnerID != finishPlayer)
					setInfo(seat, -1);// cháy
			}
			
		}
		
	}
	// cardPhomBoxs[seat].getList(CardPhomBox.FLOOR_FIRED)
	// cardPhomBoxs[seat].clearFloor(CardPhomBox.FLOOR_FIRED);
	_cardPhomBox[seat]->hideFireCards();

	_isGameEnd = true;
	//cardPhomBoxs[0].setIsGameEnd(isGameEnd);
	setMovePlayer(-1);
	showGameButton(false, false, false, false, false, false);
	// chatLayer.hideNapWinButton();
	//setContinueButton();
	//Log.log(TAG, "OnFinish myPhom " + myPhoms + " phomDown " + myPhomsDown);
}

/**
* set trạng thái kết quả
*
* @param seat
* @param type
*            0 : ù binh thuong, 1 : u khan, 2 : u den ,-1:cháy ,3:thua,
*            4:thắng
*/
void PhomScreen::setInfo(int seat, int type) {
	_cardPhomBox[seat]->showInfo(type);
}


/**
* set điểm bài rác
*
* @param seat
* @param score
*/
void PhomScreen::setScore(int seat, int score) {
	if (seat > -1) {
		_cardPhomBox[seat]->showScore(score);
	}
}

/**
* trả về khi gửi bài thành công
*
* @param userIDs
* @param userAddCards
*            : bài gửi hợp lệ (thứ tự tương ứng userIDs)
* @param userAddPhomCards
*            : bài phỏm gửi vào (thứ tự tương ứng userIDs) (>=4 lá)
*/
void PhomScreen::onSendCard(std::vector<int> userIDs, VecByte userAddCards, std::vector<std::vector<unsigned char>> userAddPhomCards) {

	//_cardPhomBox[0].layoutBox(false);
	// highLightPhomInFreeCard();
	if (userIDs.size() > 0) {
		/*
		* có người nhận được ,tiến hành tìm phỏm chứa các lá bài userCards
		* của người đó thêm là addCard vào
		*/
		// int seat;
		int userID;
		byte addCardID;
		// int seatCurrentMove = getSeatFromPlayerID(currentMovePlayer);
		// Card c;
		for (size_t i = 0; i < userIDs.size(); i++) {
			userID = userIDs.at(i);
			addCardID = userAddCards.at(i);
			//Log.log("onSendCard",
			//	"user : "
			//	+ getPlayerName(userID)
			//	+ " ,addCard : "
			//	+ Card.card(addCardID,
			//		Card.TYPE_BAICAO_XIZACH_PHOM)
			//	+ " ,addPhomCards : "
			//	+ Card.cards(userAddPhomCards.get(i),
			//		Card.TYPE_BAICAO_XIZACH_PHOM));
			if (userID > -1) {
				// lưu các bài gửi lại
;
				if(_userSendPhoms.find(userID) == _userSendPhoms.end())
				{
					std::vector<std::vector<unsigned char>> tmp;
					_userSendPhoms[userID] = tmp;
				}
				_userSendPhoms[userID].push_back(userAddPhomCards.at(i));
				// //////////////////////

				addSendPhomsToCardBox(userID);
			}
		}
	}
	if (_cur_move_player == _myInfo->GetIDDB()) {
		// nếu người gửi là mình, khi nhận về thì ẩn nút gửi đi
		showGameButton(false, false, false, true, false, false);
		p_btn_send_card->SetInteractive(false);
		p_btn_change_phom->SetInteractive(true);
		p_btn_create_phom->SetInteractive(true);
		//cardPhomBoxs[0].setTouchable(Touchable.enabled);
		//
		//addMyCardsToMyBox(false);
		for (size_t i = 0; i < userAddCards.size();i++)
			K_VEC_REMOVE(_myDealCards, userAddCards.at(i));

		updateAndLayoutMyPhomBox();
		addMyCardsToMyBox(true);
		//Log.log("Send card", myDealCards.toString());
	}
	// addMyCardsToMyBox(false);
}

/**
* nhan bai co the send tu server gui ve
* */
void PhomScreen::OnCardCanSend(VecByte cards) {

	//Log.log("phom nhan card", cards.toString() + "");
	if (cards.size() > 0) {
		p_btn_send_card->SetInteractive(true);
		p_btn_send_card->SetVisible(true);
		for (size_t i = 0; i < cards.size(); i++) {
			{
				//if (isInArrayCard(myFreeCards, cards[i]))
				if(K_VEC_CONTAIN(_myFreeCards, cards[i]))
					_cardPhomBox[0]->selectFreeCard(cards[i]);
				for (size_t j = 0; j < _myPhoms.size(); j++)
					if(K_VEC_CONTAIN(_myPhoms.at(j)._cards, cards[i]))
						_cardPhomBox[0]->selectFreeCard(cards[i]);
			}
		}
		//cardPhomBoxs[0].SetIsSendCard(true);
		//cardPhomBoxs[0].layoutBox(true);
	}
	else
	{
		p_btn_send_card->SetVisible(true);
		p_btn_send_card->SetInteractive(false);
	}
}

/**
* trả về khi đi ko được, đi lỗi, đi sai
*
* @param info
*/
void PhomScreen::onMoveError(std::string info) {
	//GameController.playSound(iWinSound.POONG);
	// đổi phỏm lỗi sẽ thông báo ở chổ mình
	onChat(_last_action == ACTION_CREATE_PHOM ? _myInfo->GetIDDB()
		: _cur_move_player, info);
	//cardPhomBoxs[0].layoutBox(false);

	_needCards.clear();
	switch (_last_action) {
	case ACTION_DOWN_PHOM:
		p_btn_down_phom->SetInteractive(true);
		break;
	case ACTION_GETCARD:
	case ACTION_EAT:
		p_btn_eat_card->SetInteractive(true);
		p_btn_get_card->SetInteractive(true);
		break;
	case ACTION_FIRE:
		p_btn_fire_card->SetInteractive(true);
		break;
	case ACTION_SEND_CARD:
		p_btn_send_card->SetInteractive(true);
		showGameButton(false, false, false, true, false, false);
		break;
	}
	//cardPhomBoxs[0].setTouchable(Touchable.enabled);
	_last_action = -1;
}

void PhomScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	//handle msg return
	if (p_list_msg_return.size() > 0)
	{
		for (auto msg : p_list_msg_return)
		{
			OnHandleMsg(msg);
			delete msg;
		}
	}
	p_list_msg_return.clear();

	//initFake();
}

void PhomScreen::sendCardSend() {
	PhomService::sendCard(GetRoomID(), GetBoardID(), _cardPhomBox[0]->getCardSelected());
}

/**
*
* @param interval
*            : thời gian cho mỗi lượt đánh (second)
* @param currentMoveTimeRemain
*            : thời gian còn lại của lượt đánh hiện tại (second)
* @param currentPlayer
*            : thằng đánh hiện tại
* @param lastMovePlayer
*            : thằng đánh trước đó
* @param downFirst
*            : thằng hạ bài đầu tiên
* @param currentFireCardID
*            : lá bài vừa đánh
* @param userIDs
*            : danh sách playerID
* @param userFiredCards
*            : bài đã đánh tương ứng với userIDs
* @param userEatCards
*            : bài ăn tương ứng với userIDs
* @param userPhomCards
*            : bài phỏm tương ứng với userIDs
* @param myCards
*            : bài của mình (server trả về luôn bài ăn của mình trong này)
*/
void PhomScreen::OnReconnect(byte interval, int currentMoveTimeRemain,
	int currentPlayer, int lastMovePlayer, int downFirst,
	byte currentFireCardID, std::vector<int> userIDs,
	std::vector<std::vector<unsigned char>> userFiredCards, std::vector<std::vector<unsigned char>> userEatCards,
	std::vector<std::vector<Phom>> userPhomCards, VecByte myCards) {
	
	onReconnect();
	_isStartGame = true;
	_isGameEnd = false;
	_on_reconnect = true;
	//_cardPhomBox[0].setReconnect(onReconnect);
	//_cardPhomBox[0]->setIsGameEnd(_isGameEnd);
	resetReady();
	hideReadyButton();
	_totalTimes = interval;
	_last_move_player = lastMovePlayer;

	//showCardPhomBoxs(true);
	setDownFirstPlayer(downFirst);
	_cur_fire_cardID = currentFireCardID;

	//if (currentFireCardID > -1 && currentFireCardID < dealCards.size) {
	//	dealCards.get(currentFireCardID).setHighlight(true);
	//}

	setMovePlayer(currentPlayer);
	// stopTimeBar();
	//for (int i = 0; i < playerGroups.length; i++) {
	//	if (playerGroups[i] != null) {
	//		playerGroups[i].stop();

	//	}
	//}
	// startTimeBar(interval * 1000, currentMoveTimeRemain * 1000);
	int seatUserMove = GetSeatFromPlayerID(currentPlayer);
	if (seatUserMove >= 0) {
		// playerGroups[seatUserMove].start(currentMoveTimeRemain * 1000);
		//Log.log(TAG, "thoigian " + interval * 1000 + " "
		//	+ currentMoveTimeRemain * 1000);
		p_list_player_group[seatUserMove]->startTime(interval * 1000, interval * 1000
			- currentMoveTimeRemain * 1000);
	}
	_myDealCards = myCards;
	CardUtils::sortByValue_CRChB(true, _myDealCards);
	_myFiredCards.clear();
	_myEatCards.clear();
	_myPhoms.clear();
	_myFreeCards.clear();

	int userID, seat;
	Phom phom;
	for (size_t i = 0; i < userIDs.size(); i++) {
		userID = userIDs.at(i);
		seat = GetSeatFromPlayerID(userID);
		if (seat > -1) {
			//Log.log("onReconnect",
			//	"user:"
			//	+ getPlayerName(userID)
			//	+ " ,userFiredCards:"
			//	+ Card.cards(userFiredCards.get(i),
			//		Card.TYPE_BAICAO_XIZACH_PHOM)
			//	+ " ,userEatCards:"
			//	+ Card.cards(userEatCards.get(i),
			//		Card.TYPE_BAICAO_XIZACH_PHOM)
			//	+ " ,userPhomCards:"
			//	+ userPhomCards.get(i).toString());
			// /////////////////////////////////////////
			for (size_t j = 0;j<userFiredCards.at(i).size();j++)
			{
				if (userFiredCards.at(i).at(j) == currentFireCardID)
				{
					userFiredCards.at(i).erase(userFiredCards.at(i).begin() + j);
					userFiredCards.at(i).push_back(currentFireCardID);
				}
			}
			if (userID == _myInfo->GetIDDB()) {
				// nếu là mình xử lý riêng
				VecByte tmp_byte;
				tmp_byte = userFiredCards.at(i);
				K_VEC_INSERT(_myFiredCards, tmp_byte);
				tmp_byte = userEatCards.at(i);
				K_VEC_INSERT(_myEatCards, _myEatCards);
				std::vector<Phom> tmp_phom_array = userPhomCards.at(i);
				K_VEC_INSERT(_myPhoms, tmp_phom_array);
				K_VEC_INSERT(_myPhomsDown, tmp_phom_array);

				if (userPhomCards.at(i).size() > 0 && _myFiredCards.size() >= 3) {
					_isDownMyPhom = true;
				}
			}
			else {
				// thêm bài đánh
				VecByte tmp_byte_array;
				tmp_byte_array = userFiredCards.at(i);
				for (byte c : tmp_byte_array) {
					_cardPhomBox[seat]->addFireCard(c);
				}

				if (userPhomCards.at(i).size() == 0) {
					// nếu không có phỏm,tức là chưa hạ phỏm ,thêm bài ăn
					// vào
					tmp_byte_array = userEatCards.at(i);
					for (byte c : tmp_byte_array) {
						_cardPhomBox[seat]->addEatCard(c);
					}
				}
				else {
					// có phỏm, đã hạ phỏm, thêm các phỏm vào
					for (size_t j = 0; j < userPhomCards.at(i).size(); j++) {
						phom = userPhomCards.at(i).at(j);
						for (byte c : phom._cards) {
							_cardPhomBox[seat]->addPhomCard(c);
						}
					}
				}
			}

			_cardPhomBox[seat]->layoutBox();
		}
	}
	/*
	* xử lý remove bài ăn trong myDealCards ra
	*/
	byte tmp_value;
	for (size_t i = 0; i < _myEatCards.size(); i++) {
		tmp_value = _myEatCards.at(i);
		K_VEC_REMOVE(_myDealCards, tmp_value);
	}
	/*
	* xử lý lấy bài rác của mình ,và add bài phỏm mà ko phải bài ăn vào
	* dealCards nếu có (vì khi đã hạ phỏm ,myCards server trả về ko có bài
	* đã hạ phỏm)
	*/
	K_VEC_INSERT(_myFreeCards, _myDealCards);
	for (size_t i = 0; i < _myPhomsDown.size(); i++) {
		phom = _myPhomsDown.at(i);
		for (size_t j = 0; j < phom._cards.size(); j++) {
			tmp_value = phom._cards.at(j);
			K_VEC_REMOVE(_myFreeCards, tmp_value);

			if(!K_VEC_CONTAIN(_myDealCards, tmp_value)
				&& !K_VEC_CONTAIN(_myEatCards, tmp_value)) {
				_myDealCards.push_back(tmp_value);
				_isDownMyPhom = true;
			}
		}
	}

	// bài cằm trên tay đc tính gồm bài tự có và bài ăn
	int numMyCards = _myDealCards.size() + _myEatCards.size();

	//Log.log("onReconnect",
	//	"myDealCards : "
	//	+ Card.cards(myDealCards, Card.TYPE_BAICAO_XIZACH_PHOM));
	//Log.log("onReconnect",
	//	"myFreeCards : "
	//	+ Card.cards(myFreeCards, Card.TYPE_BAICAO_XIZACH_PHOM));
	//Log.log("onReconnect",
	//	"myEatCards : "
	//	+ Card.cards(myEatCards, Card.TYPE_BAICAO_XIZACH_PHOM));

	// if (myFiredCards.size <= 3 && !isDownMyPhom)
	updateAndLayoutMyPhomBox();
	// else
	// addMyCardsToMyBox(false);

	/*
	* xử lý hiệu ứng đánh dấu bài ăn
	*/
	//byte cardid;
	//for (int i = 0; i < userIDs.size(); i++) {
	//	for (int j = 0; j < userEatCards.at(i).size(); j++) {
	//		cardid = userEatCards.at(i).at(j);
	//		if (cardid > -1 && cardid < dealCards.size()) {
	//			dealCards.get(cardid).setColor(COLOR_EAT_CARD);
	//		}
	//	}
	//}

	/*
	* xử lý hiện các nút action có thể sử dụng
	*/
	if (currentPlayer == _myInfo->GetIDDB()) {
		// đang là lượt mình
		if (_myFiredCards.size() <= 3) {
			/*
			* cằm 10 lá, và đánh 3 lá : thì hạ phỏm
			*/
			bool isDownPhom = numMyCards >= 10 && _myFiredCards.size() == 3
				&& !_isDownMyPhom;

			bool isFire = !isDownPhom && numMyCards >= 10
				&& _myFiredCards.size() <= 3;

			/*
			* chưa đánh lá cuối cùng được đổi phỏm
			*/
			bool isChangePhom = _myFiredCards.size() <= 3 && !_isDownMyPhom;

			bool isSendCard = _myFiredCards.size() == 3 && _isDownMyPhom;

			/*
			* cằm trên tay 9 lá, chưa đánh lá cuối cùng
			*/
			bool isGetCard = numMyCards < 10 && !isSendCard;

			showGameButton(isChangePhom, isGetCard,
				checkCanEat(isGetCard ? 3 : 4), isFire, isDownPhom,
				isSendCard);
			// luc nay can thuc hien reset vi tri card lai va thuc hien check nang bai lan nua
			_cardPhomBox[0]->layoutBox();
			checkCanEat(isGetCard ? 3 : 4);
		}
		else {
			// đã đánh lá cuối cùng, ko còn làm gì đc nữa
			showGameButton(false, false, false, false, false, false);
		}
	}
	else {
		// myFiredCards.size <= 3 tức là chưa hạ phỏm và đánh lá cuối cùng
		showGameButton(_myFiredCards.size() <= 3 && !_isDownMyPhom, false,
			false, false, false, false);
	}

}

void PhomScreen::onU(int userID, byte u, VecByte cards) {
	//Log.log("onU", getPlayerName(userID) + " : " + u);
	//Log.log(TAG, "Onu myPhom " + myPhoms + " phomDown " + myPhomsDown);
	//GameController.playSound(iWinSound.U);
	// cardPhomBoxs[0].layoutBox(false);

	// highLightPhomInFreeCard();
	_userIdU = userID;
	// kiem tra neu la u da co danh bai thi add cac phom len tren
	// if(checkU()&&cardPhomBoxs[0].getList(CardPhomBox.FLOOR_PHOM).size>=0)

	// neu da ha phom
	if (_isDownMyPhom) {
		K_VEC_INSERT(_myPhomsDown, _myPhoms);
	}

	addMyCardsToMyBox(true);
	//Log.log(TAG, "Onu myPhom " + myPhoms + " phomDown " + myPhomsDown);
	if (userID != _myInfo->GetIDDB()) {
		int seat = GetSeatFromPlayerID(userID);
		if (seat > -1) {
			setInfo(seat, u);
			_cardPhomBox[seat]->clearPhomCard();

			for (size_t i = 0; i < cards.size(); i++) {
				if((char)cards[i] > -1)
					_cardPhomBox[seat]->addPhomCard(cards[i]);
			}
			_cardPhomBox[seat]->layoutBox();
			//GameController.playSound(iWinSound.CHIABAI);
		}
	}
	else
	{
		int seat = GetSeatFromPlayerID(_myInfo->GetIDDB());
		if (seat > -1)
		{
			setInfo(seat, u);
		}
	}

}

bool PhomScreen::checkContainPhom(std::vector<Phom> phoms, Phom phom)
{
	for (size_t i = 0; i < phoms.size(); i++)
	{
		if (phoms[i]._cards.size() == phom._cards.size())
		{
			bool card_exist = false;
			for (size_t ii = 0; ii < phom._cards.size(); ii++)
			{
				card_exist = phoms.at(i).containCard(phom._cards[ii]);
				if (!card_exist) break;
			}
			if (card_exist) return true;
		}
	}
	return false;
}
