
#include "UI/AdvanceUI/wLayout.h"

#include "CoTuongBoard.h"
#include "CoTuongPiece.h"
#include "CoTuongScreen.h"

#define COTUONG_NUM_ROW 10 
#define COTUONG_NUM_COL 9 

CoTuongBoard::CoTuongBoard()
{

}

CoTuongBoard::~CoTuongBoard()
{
	for (auto p : _pool_piece)
	{
		p->release();
	}
	_pool_piece.clear();
}

void CoTuongBoard::init(Vec2 pos_board, Vec2 size_board)
{
	_size_cell = Vec2(size_board.x / (COTUONG_NUM_COL - 1), size_board.y / (COTUONG_NUM_ROW - 1));
	_pos_board = pos_board;

	for (int i = 0; i < 2; i++)
	{
		addPiece(PIECE_KING, i == 0);
		addPiece(PIECE_ADVISOR, i == 0);
		addPiece(PIECE_ADVISOR, i == 0);
		addPiece(PIECE_BISHOP, i == 0);
		addPiece(PIECE_BISHOP, i == 0);
		addPiece(PIECE_KNIGHT, i == 0);
		addPiece(PIECE_KNIGHT, i == 0);
		addPiece(PIECE_ROOK, i == 0);
		addPiece(PIECE_ROOK, i == 0);
		addPiece(PIECE_CANNON, i == 0);
		addPiece(PIECE_CANNON, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
		addPiece(PIECE_PAWN, i == 0);
	}

}

void CoTuongBoard::addPiece(int id, bool mau)
{
	CoTuongPiece* piece = CoTuongPiece::create();
	piece->retain();
	piece->setID(id, mau);
	_pool_piece.push_back(piece);
	piece->addTouchEventListener(CC_CALLBACK_2(CoTuongScreen::onTouchPiece, _game_screen));
}

CoTuongPiece* CoTuongBoard::getPiece(int id, bool mau)
{
	for (CoTuongPiece* p : _pool_piece)
	{
		if (!p->getParent() && p->getID() == id && p->getMau() == mau)
		{
			return p;
		}
	}
	return nullptr;
}

void CoTuongBoard::loadBoard(std::vector<int> board, int myColor, int turnColor)
{
	_myColor = myColor;
	_turnColor = turnColor;
	int myId = myColor * 10;
	int oppId = (1 - myColor) * 10;
	int mau;
	for (int i = 0; i < COTUONG_NUM_ROW; i++)
	{
		for (int j = 0; j < COTUONG_NUM_COL; j++)
		{
			_board[i][j] = board[i * COTUONG_NUM_COL + j];
			if (_board[i][j] == 0) continue;
			_board[i][j] = (i < 5 ? myId : oppId) + board[i * COTUONG_NUM_COL + j];
			mau = (i < 5 ? myColor : (1 - myColor));
			CoTuongPiece* piece = getPiece(_board[i][j] % 10, (mau > 0));
			if (piece && !piece->getParent()) {
				_board_node->addChild(piece, 2);
				piece->setRow(j);
				piece->setCol(i);
				piece->setPosition(Vec2(_pos_board.x + j * _size_cell.x, _pos_board.y + i * _size_cell.y));
				piece->setSize(cocos2d::Size(_size_cell.x , _size_cell.y ));
			}
		}
	}
}

CoTuongPiece* CoTuongBoard::getPiece(int row, int col)
{
	for (CoTuongPiece* p : _pool_piece)
	{
		if (p->getParent() && p->getRow() == row && p->getCol() == col)
		{
			return p;
		}
	}
	return nullptr;
}

int CoTuongBoard::getBoardValue(int row, int col)
{
	return _board[row][col];
}

Vec2 CoTuongBoard::getPosFromRowCol(int row, int col)
{
	return Vec2(_pos_board.x + col * _size_cell.x, _pos_board.y + row * _size_cell.y);
}

ImageView* CoTuongBoard::getFreeMove()
{
	for (ImageView* img : _pool_move) {
		if ( !img->isVisible()) {
			return img;
		}
	}

	ImageView* new_img = ImageView::create();
	new_img->loadTexture("gamescreen-cotuong-screen/di.png", TextureResType::PLIST);
	_pool_move.push_back(new_img);
	_board_node->addChild(new_img, 5);
	new_img->setContentSize(cocos2d::Size(100, 100));
	return new_img;
}

void CoTuongBoard::addSuggestMove(std::vector<Vec2> moves)
{
	clearMoves();
	ImageView* move_node = getFreeMove();
	for (Vec2 pos : moves) {
		move_node = getFreeMove();
		move_node->setVisible(true);
		move_node->setPosition(getPosFromRowCol(pos.x, pos.y));
	}
}

void CoTuongBoard::clearMoves()
{
	for (ImageView* img : _pool_move) {
		img->setVisible(false);
	}
}

void CoTuongBoard::resetSelect()
{
	for (CoTuongPiece* p : _pool_piece) {
		p->unselect();
	}
}

void CoTuongBoard::move(CoTuongPiece* piece, int toRow, int toCol, int new_piece_id)
{
	_lastRow = piece->getRow();
	_lastCol = piece->getCol();
	_board[_lastRow][_lastCol] = 0;
	_board[toRow][toCol] = piece->getMau() * 10 + piece->getID();
		
	CoTuongPiece* eat_piece = getPiece(toRow, toCol);
	if (!eat_piece) {
		piece->runAction(MoveTo::create(.2f, getPosFromRowCol(toRow, toCol)));
	}
	else {
		eat_piece->removeFromParent();
		piece->runAction(MoveTo::create(.2f, getPosFromRowCol(toRow, toCol)));
		piece->setRow(toRow);
		piece->setCol(toCol);
	}

	_turnColor = 1 - _turnColor;

}


Vec2 CoTuongBoard::convertToRowCol(Vec2 pos_touch)
{
    cocos2d::Rect cell;
	int touch_w = _size_cell.x / 2;
	int touch_h = _size_cell.y / 2;
	for (int i = 0; i < COTUONG_NUM_ROW; i++)
	{
		for (int j = 0; j < COTUONG_NUM_COL; j++)
		{
			Vec2 pos = getPosFromRowCol(i, j);
			cell = cocos2d::Rect( pos.x - touch_w / 2, pos.y - touch_h / 2, touch_w, touch_h);
			if (cell.containsPoint(pos_touch)) {
				return Vec2(j, i);
			}
		}
	}
	return Vec2(-1, -1);
}
