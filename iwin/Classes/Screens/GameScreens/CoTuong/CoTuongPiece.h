#ifndef __CO_TUONG_PIECE_H__
#define __CO_TUONG_PIECE_H__

#include "cocos2d.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"

USING_NS_CC;
using namespace ui;

#define PIECE_INVALID 0// tuong
#define PIECE_KING 1// tuong
#define PIECE_ADVISOR 2 // si
#define PIECE_BISHOP 3 // tuong
#define PIECE_KNIGHT 4// ma
#define PIECE_ROOK 5 // xe
#define PIECE_CANNON 6// phao
#define PIECE_PAWN 7// chot
#define PIECE_FACE 8// up

#define PIECE_SIZE 32 

class CoTuongPiece: public Button
{
	CC_SYNTHESIZE( int,	_id, ID);
	CC_SYNTHESIZE( bool,	_mau, Mau);
	CC_SYNTHESIZE( int,	_row, Row);
	CC_SYNTHESIZE( int,	_col, Col);
	CC_SYNTHESIZE( bool,	_isFaceUp, IsFaceUp);
	CC_SYNTHESIZE( bool,	_isFood, IsFood);


	ImageView*		_image_co;
	ImageView*		_image_select;
public:
	CoTuongPiece();
	virtual ~CoTuongPiece();

	virtual bool init();
	virtual void setSize(cocos2d::Size size);

	void setID(int id, bool mau);
	void select();
	void unselect();

	void move(int new_row, int new_col);

	static CoTuongPiece* create();
	static bool isValidId(int id);
};


#endif //__CO_TUONG_BOARD_H__