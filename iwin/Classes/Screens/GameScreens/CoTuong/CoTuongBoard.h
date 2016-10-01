#ifndef __CO_TUONG_BOARD_H__
#define __CO_TUONG_BOARD_H__

//#include "cocos2d.h"

//#include "UI/UIImageView.h"
#include "Screens/ScreenManager.h"
//USING_NS_CC;
using namespace ui;

class CoTuongPiece;
class CoTuongScreen;

class CoTuongBoard
{
	Vec2		_size_cell;
	Vec2		_pos_board;

	std::vector<CoTuongPiece*> _pool_piece;
	std::vector<ImageView*> _pool_move;

	int			_board[10][9];

	CC_SYNTHESIZE(cocos2d::Node*, _board_node, BoardNode)
	CC_SYNTHESIZE(int, _myColor, MyColor)
	CC_SYNTHESIZE(int, _turnColor, TurnColor)
	CC_SYNTHESIZE(int, _lastRow, LastRow)
	CC_SYNTHESIZE(int, _lastCol, LastCol)
	CC_SYNTHESIZE(CoTuongScreen*, _game_screen, GameScreen)


		void		addPiece(int id, bool mau);
	CoTuongPiece*		getPiece(int id, bool mau);
	ImageView*		getFreeMove();
	
public:
	CoTuongBoard();
	virtual ~CoTuongBoard();
	void init(Vec2 pos_board, Vec2 size_board);

	void loadBoard(std::vector<int> board, int myColor, int turnColor);
	CoTuongPiece* getPiece(int row, int col);
	int				getBoardValue(int row, int col);
	void addSuggestMove(std::vector<Vec2> moves);
	Vec2 getPosFromRowCol(int row, int col);
	void clearMoves();
	void resetSelect();
	void move(CoTuongPiece*, int toRow, int toCol, int new_piece_id);
	Vec2 convertToRowCol(Vec2 pos_touch);

	void resetToNewBoard();

};


#endif //__CO_TUONG_BOARD_H__