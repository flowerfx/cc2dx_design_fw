

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

#include "CoTuongScreen.h"
#include "Utils.h"
#include "AnimUtils.h"
#include "CoTuongBoard.h"
#include "CoTuongPiece.h"
#include "Common/Common.h"

CoTuongScreen* CoTuongScreen::getInstance()
{
	return static_cast<CoTuongScreen*>(GamePlay->GetCurrentGameScreen());
}

CoTuongScreen::CoTuongScreen()
{
	SetMaxPlayer(2);
}

CoTuongScreen::~CoTuongScreen()
{
}


bool CoTuongScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);
	GetWidgetChildByName("main_background.bg_table")->SetVisible(false);
	XMLMgr->OnLoadXMLData<CoTuongScreen>("CoTuong_Screen", std::mem_fn(&CoTuongScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("CoTuong_Screen");
	_board_widget = GetWidgetChildByName("HUD_screen.panel_custom_menu.custom_root.board_bg");
	static_cast<ImageView*>(_board_widget->GetResource())->addTouchEventListener(CC_CALLBACK_2(CoTuongScreen::onTouchBoard, this));
	static_cast<ImageView*>(_board_widget->GetResource())->setTouchEnabled(true);
	
	_board = new CoTuongBoard();
	_board->setGameScreen(this);
	_board->setBoardNode(_board_widget->GetResource());
	_board->init(_board_widget->GetWidgetChildByName(".banco_win")->GetPosition(), _board_widget->GetWidgetChildByName(".banco_win")->GetSize());


	for (int i = 0; i < 2; i++)
	{
		_player_quotes[i] = TextBMFont::create("", File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(1)->path_fnt).GetString());
		addChild(_player_quotes[i]);
	}
	_curSeatUpdateTime = -1;
	_last_piece = nullptr;
	/*p_panel_bottom = GetWidgetChildByName("HUD_screen.panel_custom_menu.back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path")*/;

	return true;
}

void CoTuongScreen::resetToNewBoard(int myColor, int turnColor)
{	
	int start_board[10][9] = {{PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_ADVISOR, PIECE_KING, PIECE_ADVISOR, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK},
							{PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
							{PIECE_INVALID, PIECE_CANNON, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_CANNON, PIECE_INVALID },
							{PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN },
							{PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
							{PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
							{PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN, PIECE_INVALID, PIECE_PAWN },
							{PIECE_INVALID, PIECE_CANNON, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_CANNON, PIECE_INVALID },
							{PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID, PIECE_INVALID },
							{PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_ADVISOR, PIECE_KING, PIECE_ADVISOR, PIECE_BISHOP, PIECE_KNIGHT, PIECE_ROOK}
	};
	std::vector<int> vec_data;
	for (int i = 0; i < 10; i++)
	{
		for (int ii = 0; ii < 9; ii++)
		{
			vec_data.push_back(start_board[i][ii]);
		}
	}
	_board->loadBoard(vec_data, myColor, turnColor);

}

void CoTuongScreen::OnProcessWidgetChild(RKString name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_xinhoa")
		{
			sendCommandChess((ubyte)30, GetRoomID(), GetBoardID());
		}
		else if (name == "btn_xinthua")
		{
			ScrMgr->OnShowDialog("notice", "are_you_sure_you_want_to", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", [this](const char* call_back, const char* btn_name) {
				if (strcmp("ok_2_btn", btn_name) == 0) {
					this->sendCommandChess((ubyte)35, this->GetRoomID(), this->GetBoardID());
				}
			});
		}
	}
}

int CoTuongScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (_curSeatUpdateTime != -1) {
		if (_curSeatUpdateTime == 0){
			_totalTimeToPlayPlayer1 -= dt;
			_player_quotes[_curSeatUpdateTime]->setString(formatTimer(_totalTimeToPlayPlayer1));
		} else {
			_totalTimeToPlayPlayer2 -= dt;
			_player_quotes[_curSeatUpdateTime]->setString(formatTimer(_totalTimeToPlayPlayer2));
		}
	}

	return 1;
}

void CoTuongScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	auto p_panel_custom = GetWidgetChildByName("HUD_screen.panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("custom_root"), p_panel_custom, "panel_custom_menu");
	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();
}

void CoTuongScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	if (p_list_msg_return.size() > 0)
	{
		for (auto msg : p_list_msg_return)
		{
			OnHandleMsg(msg);
			delete msg;
		}
	}
	p_list_msg_return.clear();

	Vec2 group_size;
	for (int i = 0; i < p_max_player; i++) {
		_player_quotes[i]->setText("Pham Do Truong");
		_player_quotes[i]->setPosition( p_list_player_group[i]->getWidget()->GetPosition());
	}
}

/**
* tinh toan duong di cua cac quan co
*/
/**
*
* @param p
* @param turnColor
* @param isLogic
*            : xac dinh trang thai dang tim de danh hay de check lam viec
*            khac (de khong ve co an duoc)
* @return
*/
std::vector<Vec2> CoTuongScreen::findMove(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	switch (p->getID()) {
	case PIECE_PAWN:
		listCanMove = findPawn(p, turnColor, isLogic);
		break;
	case PIECE_CANNON:
		listCanMove = findCannon(p, turnColor, isLogic);
		break;
	case PIECE_ROOK:
	{
		int tmp_arr[4] = { -1, -1, -1, -1 };
		listCanMove = findRook(p, turnColor, tmp_arr, isLogic);
	}
	break;
	case PIECE_KNIGHT:
		listCanMove = findKnight(p, turnColor, isLogic);
		break;
	case PIECE_BISHOP:
		listCanMove = findBishop(p, turnColor, isLogic);
		break;
	case PIECE_ADVISOR:
		listCanMove = findAdvisor(p, turnColor, isLogic);
		break;
	case PIECE_KING:
		listCanMove = findKing(p, turnColor, isLogic);
		break;
	case PIECE_FACE:
		int id = getID_CoUp(p);
		if (id > 0) {
			CoTuongPiece* pp = CoTuongPiece::create();
			pp->setID(getID_CoUp(p), p->getMau());
			pp->setRow(p->getRow());
			pp->setCol(p->getCol());
			pp->setIsFaceUp(false);
			listCanMove = findMove(pp, turnColor, isLogic);
		}
		break;
	}
	return listCanMove;
}

int CoTuongScreen::getID_CoUp(CoTuongPiece* p) {
	if (p->getCol() % 2 == 0 && (p->getRow() == 3 || p->getRow() == 6)) {
		return PIECE_PAWN;
	}
	if ((p->getCol() == 1 || p->getCol() == 7) && (p->getRow() == 2 || p->getRow() == 7)) {
		return PIECE_CANNON;
	}
	if ((p->getCol() == 0 || p->getCol() == 8) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_ROOK;
	}
	if ((p->getCol() == 1 || p->getCol() == 7) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_KNIGHT;
	}
	if ((p->getCol() == 2 || p->getCol() == 6) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_BISHOP;
	}
	if ((p->getCol() == 3 || p->getCol() == 5) && (p->getRow() == 0 || p->getRow() == 9)) {
		return PIECE_ADVISOR;
	}
	return 0;
}

/**
*
* @param p
* @param turnColor
*            : mau dang di
*/
std::vector<Vec2> CoTuongScreen::findAdvisor(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 1, p->getCol() - 1 };
	int r[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow, minCol, maxCol;

	if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS) {
		minCol = 3;
		maxCol = 5;
		if (turnColor == _board->getMyColor()) {
			minRow = 0;
			maxRow = 2;
		}
		else {
			minRow = 7;
			maxRow = 9;
		}
	}
	else {
		if (!p->getIsFaceUp()) {
			minCol = 3;
			maxCol = 5;
		}
		else {
			minCol = 0;
			maxCol = 8;
		}
		minRow = 0;
		maxRow = 9;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (c[j] >= minCol && c[j] <= maxCol && r[i] >= minRow && r[i] <= maxRow) {
				CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( r[i], c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back( Vec2 ( r[i], c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findBishop(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 2, p->getCol() - 2 };
	int r[2] = { p->getRow() + 2, p->getRow() - 2 };
	int cc[2] = { p->getCol() + 1, p->getCol() - 1 };
	int rc[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow;

	if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS) {
		if (turnColor == _board->getMyColor()) {
			minRow = 0;
			maxRow = 4;
		}
		else {
			minRow = 5;
			maxRow = 9;
		}
	}
	else {
		minRow = 0;
		maxRow = 9;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (c[j] >= 0 && c[j] <= 8 && r[i] >= minRow && r[i] <= maxRow && _board->getBoardValue(rc[i], cc[j]) == 0) {
				CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( r[i], c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back(Vec2 ( r[i], c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findKing(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[2] = { p->getCol() + 1, p->getCol() - 1 };
	int r[2] = { p->getRow() + 1, p->getRow() - 1 };
	int minRow, maxRow;
	if (turnColor == _board->getMyColor()) {
		minRow = 0;
		maxRow = 2;
	}
	else {
		minRow = 7;
		maxRow = 9;
	}
	for (int i = 0; i < 2; i++) {
		if (r[i] >= minRow && r[i] <= maxRow) {
			CoTuongPiece* dest = _board->getPiece(r[i], p->getCol());
			if (dest != nullptr && dest->getMau() != turnColor) {
				if (!isLogic) {
					dest->setIsFood(true);
				}
				listCanMove.push_back(Vec2( r[i], p->getCol()));
			}
			else if (dest == nullptr) {
				listCanMove.push_back(Vec2( r[i], p->getCol() ));
			}
		}
	}
	for (int j = 0; j < 2; j++) {
		if (c[j] >= 3 && c[j] <= 5) {
			CoTuongPiece* dest = _board->getPiece(p->getRow(), c[j]);
			if (dest != nullptr && dest->getMau() != turnColor) {
				if (!isLogic) {
					dest->setIsFood( true);
				}
				listCanMove.push_back(Vec2( p->getRow(), c[j] ));
			}
			else if (dest == nullptr) {
				listCanMove.push_back(Vec2 ( p->getRow(), c[j] ));
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findPawn(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int r;
	bool isUp = false, isLeftRight = false;
	if (turnColor == _board->getMyColor()) {
		r = p->getRow() + 1;
		if (p->getRow() <= 9) {
			if (r <= 9) {
				isUp = true;
			}
			if (p->getRow() >= 5) {
				isLeftRight = true;
			}
		}
	}
	else {
		r = p->getRow() - 1;
		if (p->getRow() >= 0) {
			if (r >= 0) {
				isUp = true;
			}
			if (p->getRow() <= 4) {
				isLeftRight = true;
			}
		}
	}

	if (isUp) {
		CoTuongPiece* dest = _board->getPiece(r, p->getCol());
		if (dest != nullptr && dest->getMau() != turnColor) {
			if (!isLogic) {
				dest->setIsFood(true);
			}
			listCanMove.push_back(Vec2( r, p->getCol() ));
		}
		else if (dest == nullptr) {
			listCanMove.push_back(Vec2( r, p->getCol() ));
		}
	}
	if (isLeftRight) {
		int c[2] = { p->getCol() + 1, p->getCol() - 1 };
		for (int j = 0; j < 2; j++) {
			if (c[j] >= 0 && c[j] <= 8) {
				CoTuongPiece* dest = _board->getPiece(p->getRow(), c[j]);
				if (dest != nullptr && dest->getMau() != turnColor) {
					if (!isLogic) {
						dest->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), c[j] ));
				}
				else if (dest == nullptr) {
					listCanMove.push_back(Vec2( p->getRow(), c[j] ));
				}
			}
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findKnight(CoTuongPiece* p, int turnColor, bool isLogic) {
	std::vector<Vec2> listCanMove;
	int c[4] = { p->getCol() - 2, p->getCol() - 1, p->getCol() + 1, p->getCol() + 2 };
	int r[4] = { p->getRow() + 1, p->getRow() + 2, p->getRow() - 2, p->getRow() - 1 };
	int cc[4] = { p->getCol() - 1, p->getCol(), p->getCol(), p->getCol() + 1 };
	int rc[4] = { p->getRow(), p->getRow() + 1, p->getRow() - 1, p->getRow() };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (c[j] >= 0 && c[j] <= 8 && r[i] >= 0 && r[i] <= 9
				&& ((std::abs(c[j] - p->getCol()) == 2 && std::abs(r[i] - p->getRow()) == 1)
					|| (std::abs(c[j] - p->getCol()) == 1 && std::abs(r[i] - p->getRow()) == 2))) {
				if (_board->getBoardValue(rc[i], cc[j]) == 0) {
					CoTuongPiece* dest = _board->getPiece(r[i], c[j]);
					if (dest != nullptr && dest->getMau() != turnColor) {
						if (!isLogic) {
							dest->setIsFood(true);
						}
						listCanMove.push_back(Vec2( r[i], c[j] ));
					}
					else if (dest == nullptr) {
						listCanMove.push_back(Vec2( r[i], c[j] ));
					}
				}
			}
		}
	}
	return listCanMove;
}

/**
*
* @param p
* @param turnColor
* @param nutBiChan
*            : mang 4 phan tu -1 dua vao de tinh duoc cac diem bi chan cho
*            quan Phao
* @param isLogic
* @return
*/
std::vector<Vec2> CoTuongScreen::findRook(CoTuongPiece* p, int turnColor, int nutBiChan[], bool isLogic) {
	std::vector<Vec2> listCanMove;
	// l,r,u,d
	// left
	for (int i = p->getCol() - 1; i >= 0; i--) {
		if (_board->getBoardValue(p->getRow(), i) == 0) {
			listCanMove.push_back(Vec2( p->getRow(), i ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2(p->getRow(), i ));
				}
			}
			nutBiChan[0] = i;
			break;
		}
	}
	// right
	for (int i = p->getCol() + 1; i <= 8; i++) {
		if (_board->getBoardValue(p->getRow(), i) == 0) {
			listCanMove.push_back(Vec2( p->getRow(), i ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), i ));
				}
			}
			nutBiChan[1] = i;
			break;
		}
	}
	// up
	for (int i = p->getRow() + 1; i <= 9; i++) {
		if (_board->getBoardValue(i, p->getCol()) == 0) {
			listCanMove.push_back(Vec2( i, p->getCol() ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
			}
			nutBiChan[2] = i;
			break;
		}
	}
	// down
	for (int i = p->getRow() - 1; i >= 0; i--) {
		if (_board->getBoardValue(i, p->getCol()) == 0) {
			listCanMove.push_back(Vec2( i, p->getCol() ));
		}
		else {
			// co con chan duong
			if (p->getID() == PIECE_ROOK) {
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
			}
			nutBiChan[3] = i;
			break;
		}
	}
	return listCanMove;
}

std::vector<Vec2> CoTuongScreen::findCannon(CoTuongPiece* p, int turnColor, bool isLogic) {
	int chan[4] ={ -1, -1, -1, -1 };
	std::vector<Vec2> listCanMove = findRook(p, turnColor, chan, isLogic);
	// left
	if (chan[0] > -1) {
		for (int i = chan[0] - 1; i >= 0; i--) {
			if (_board->getBoardValue(p->getRow(), i ) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2(p->getRow(), i ));
				}
				break;
			}
		}
	}
	// right
	if (chan[1] > -1) {
		for (int i = chan[1] + 1; i <= 8; i++) {
			if (_board->getBoardValue(p->getRow(), i) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(p->getRow(), i);
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( p->getRow(), i ));
				}
				break;
			}
		}
	}
	// up
	if (chan[2] > -1) {
		for (int i = chan[2] + 1; i <= 9; i++) {
			if (_board->getBoardValue(i, p->getCol()) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
				break;
			}
		}
	}
	// down
	if (chan[3] > -1) {
		for (int i = chan[3] - 1; i >= 0; i--) {
			if (_board->getBoardValue(i, p->getCol()) != 0) {
				// gap con an
				CoTuongPiece* q = _board->getPiece(i, p->getCol());
				if (q != nullptr && q->getMau() != turnColor) {
					if (!isLogic) {
						q->setIsFood(true);
					}
					listCanMove.push_back(Vec2( i, p->getCol() ));
				}
				break;
			}
		}
	}
	return listCanMove;
}

void CoTuongScreen::onStartGame(int whoFirst, int interval)
{
	OnHideCountDown();
	_totalTimes = interval;
	_whoFirst = whoFirst;
	_myColor = getMyColor(whoFirst);
	totalTimes = interval;
	PlayerGroup* player_group = getPlayerGroup(whoFirst);
	player_group->startTime(interval, 0);
	resetToNewBoard(_myColor, 0);
	updateLabelTime(whoFirst);
}


void CoTuongScreen::moveCoTuong(ubyte supCommand, ubyte roomId, ubyte boardId, ubyte xS, ubyte yS, ubyte xD, ubyte yD)
{
	CCLOG("%d %d %d %d", xS, yS, xD, yD);
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomId);
	m->putByte(boardId);
	m->putByte(supCommand);
	m->putByte(xS);
	m->putByte(yS);
	m->putByte(xD);
	m->putByte(yD);
	GlobalService::sendMessage(m);
}

/**
*
* @param subCommand
* @param roomID
* @param boardID
* @param request
*            0 = undo_want; 1 = undo_accept; -1 = undo_deny
*/
void CoTuongScreen::sendCommandUndoChess(ubyte subCommand, ubyte roomID, ubyte boardID, ubyte request) {
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	m->putByte(request);
	GlobalService::sendMessage(m);
}

/**
* command danh cho co tuong moi 77
*
* @param subCommand
*/
void CoTuongScreen::sendCommandChess(ubyte subCommand, ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
		|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) ? IwinProtocol::CO_UP_COMMAND
		: IwinProtocol::CO_TUONG_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	GlobalService::sendMessage(m);
}

/**
* xác định màu cờ ở vị trí của mình
*
* @param moveFirst
*            : người đi trước
* @return
*/
int CoTuongScreen::getMyColor(int moveFirst) {
	int seatFirst = GetSeatFromPlayerID(moveFirst);
	if (_isViewer) {
		return seatFirst;
	}
	else {
		return moveFirst == _myInfo->GetIDDB() ? 0 : 1;
	}
}

void CoTuongScreen::setTimeQuota(int userId, int time)
{
	for (int i = 0; i < p_list_player_group.size(); i++)
	{
		if (p_list_player_group[i]->GetIDDB() == userId) {
			_player_quotes[i]->setText(StringUtils::format("%d", time));
		}
	}

	int seat = GetSeatFromPlayerID(userId);
	if (seat == 0) _totalTimeToPlayPlayer1 = time;
	else _totalTimeToPlayPlayer2 = time;
}


void CoTuongScreen::onTouchBoard(Ref* target, Widget::TouchEventType touch_type)
{
	Widget* board = static_cast<Widget*>(target);
	switch (touch_type)
	{
	case Widget::TouchEventType::ENDED:
	{
		Vec2 board_touch = board->getTouchEndPosition();
		board_touch = board->convertToNodeSpace(board_touch);
		Vec2 row_col = _board->convertToRowCol(board_touch);
		CCLOG("%f %f", row_col.x, row_col.y);
		if (_last_piece) {
			moveCoTuong(25, GetRoomID(), GetBoardID(), _last_piece->getRow(), _last_piece->getCol(), row_col.x, row_col.y);
		}

		//_board->move()
	}
	break;
	}
}

void CoTuongScreen::onTouchPiece(Ref* target, Widget::TouchEventType touch_type)
{
	CoTuongPiece* piece = static_cast<CoTuongPiece*>(target);
	switch (touch_type)
	{
	case Widget::TouchEventType::ENDED:
	{
		CCLOG("Piece Touch %d %d", piece->getRow(), piece->getCol());
		std::vector<Vec2> list_move = findMove(piece, _board->getTurnColor(), false);
		_board->resetSelect();
		_board->addSuggestMove(list_move);
		piece->select();
		_last_piece = piece;
	}
	break;
	}
}

void CoTuongScreen::onMove(int whoMove, ubyte xS, ubyte yS, ubyte xD, ubyte yD, int nextMove, ubyte newPieceType) {
	bool isCoUp = GameController::isHiddenChess();
/*
	if (timer != null) {
		timer.cancel();
	}*/
	_board->resetSelect();
	_board->clearMoves();

	CoTuongPiece* p = _board->getPiece(xS, yS);
	CoTuongPiece* pEnd = _board->getPiece(xD, yD);
	if (isCoUp && CoTuongPiece::isValidId(p->getID()) && CoTuongPiece::isValidId(newPieceType)) {
		newPieceType = -1;
	}
		
	if (pEnd) {
		_board->move(p, yD, xD, isCoUp ? newPieceType : -1);
		pEnd->removeFromParent();
	}
	else {
		_board->move(p, yD, xD, isCoUp ? newPieceType : -1);
	}

	//int seatMove = getSeatFromPlayerID(nextMove);
	//startTimeBar(seatMove, this.totalTimes);

	PlayerGroup* pg = getPlayerGroup(whoMove);
	pg->stopTime();
	pg = getPlayerGroup(nextMove);
	pg->startTime(totalTimes, 0);
	updateLabelTime(nextMove);


}

void CoTuongScreen::updateLabelTime(int playerId)
{
	int seat = GetSeatFromPlayerID(playerId);
	_curSeatUpdateTime = seat;
	//_player_quotes[seat]->
}

void CoTuongScreen::onOpponentWantUndo()
{
	ScrMgr->OnShowDialog("notice", "can_i_take_a_bad", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", nullptr, nullptr, [this](const char* call_back, const char* btn_name) {
		if (strcmp("cancel_fb_btn", btn_name) == 0) {
			this->sendCommandUndoChess((ubyte)26, this->GetRoomID(), this->GetBoardID(), (ubyte)1);
		}
		else if (strcmp("cancel_2_btn", btn_name) == 0) {
			this->sendCommandUndoChess((ubyte)26, this->GetRoomID(), this->GetBoardID(), (ubyte)-1);
		}
	});
}

void CoTuongScreen::onOpponentWantDraw()
{
	ScrMgr->OnShowDialog("notice", "opponent_want_draw_are", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", nullptr, nullptr, [this](const char* call_back, const char* btn_name) {
		if (strcmp("cancel_fb_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)30, this->GetRoomID(), this->GetBoardID());
		}
		else if (strcmp("cancel_2_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)31, this->GetRoomID(), this->GetBoardID());
		}
	});
}

void CoTuongScreen::onOpponentWantLose()
{
	ScrMgr->OnShowDialog("notice", "opponent_want_lose_are", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", nullptr, nullptr, [this](const char* call_back, const char* btn_name) {
		if (strcmp("cancel_fb_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)35, this->GetRoomID(), this->GetBoardID());
		}
		else if (strcmp("cancel_2_btn", btn_name) == 0) {
			this->sendCommandChess((ubyte)36, this->GetRoomID(), this->GetBoardID());
		}
	});
}

std::string CoTuongScreen::formatTimer(int time)
{
	return StringUtils::format("%02d:%02d", time / 60, time % 60);
}