
#include "CoTuongPiece.h"


CoTuongPiece* CoTuongPiece::create() {
	CoTuongPiece* ret = new CoTuongPiece();
	ret->autorelease();
	ret->init();
	return ret;
}

bool CoTuongPiece::isValidId(int id)
{
	return id == PIECE_KING || id == PIECE_ADVISOR || id == PIECE_BISHOP || id == PIECE_KNIGHT || id == PIECE_ROOK
		|| id == PIECE_CANNON || id == PIECE_PAWN;
}


CoTuongPiece::CoTuongPiece()
{
	_image_co = nullptr;
}

CoTuongPiece::~CoTuongPiece()
{

}

bool CoTuongPiece::init()
{
	const char* name_fake = "loginscreen/nine/whitepixel.png";
	Button::init(name_fake, name_fake, name_fake, TextureResType::PLIST);
	setOpacity(0);
	setScale9Enabled(true);
	setContentSize(cocos2d::Size(PIECE_SIZE, PIECE_SIZE));
	setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	_image_select = ImageView::create("gamescreen-cotuong-screen/chon.png", TextureResType::PLIST);
	_image_select->ignoreContentAdaptWithSize(false);
	_image_select->setContentSize(cocos2d::Size(PIECE_SIZE, PIECE_SIZE));
	_image_select->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_image_select->setPosition(Vec2::ZERO);
	_image_select->setVisible(false);
	addChild(_image_select);

	_image_co = ImageView::create("gamescreen-cotuong-screen/chot.png", TextureResType::PLIST);
	_image_co->ignoreContentAdaptWithSize(false);
	_image_co->setContentSize(cocos2d::Size(PIECE_SIZE, PIECE_SIZE));
	_image_co->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_image_co->setPosition(Vec2::ZERO);
	addChild(_image_co);
	return true;
}

void CoTuongPiece::setID(int id, bool mau)
{
	_mau = mau;
	_id = id;
	std::string frame_name;
	switch (_id)
	{
	case PIECE_ADVISOR:
		frame_name = "gamescreen-cotuong-screen/si";
		break;
	case PIECE_BISHOP:
		frame_name = "gamescreen-cotuong-screen/tuong";
		break;
	case PIECE_CANNON:
		frame_name = "gamescreen-cotuong-screen/phao";
		break;
	case PIECE_KING:
		frame_name = "gamescreen-cotuong-screen/nguyen-soai";
		break;
	case PIECE_KNIGHT:
		frame_name = "gamescreen-cotuong-screen/ma";
		break;
	case PIECE_PAWN:
		frame_name = "gamescreen-cotuong-screen/chot";
		break;		
	case PIECE_ROOK:
		frame_name = "gamescreen-cotuong-screen/xe";
	}
	if (!mau) frame_name += "-den";
	frame_name += ".png";
	_image_co->loadTexture(frame_name, TextureResType::PLIST);
}

void CoTuongPiece::setSize(cocos2d::Size size)
{
	Button::setSize(size);
	if (_image_co) {
		_image_co->setContentSize(size);
		_image_co->setPosition(Vec2(size.width / 2, size.height / 2));
		_image_select->setContentSize(size);
		_image_select->setPosition(Vec2(size.width / 2, size.height / 2));
	}
}

void CoTuongPiece::select()
{
	_image_select->setVisible(true);
	_image_select->stopAllActions();
	_image_select->setScale(1.0f);
	_image_select->runAction(RepeatForever::create( Sequence::create(ScaleTo::create(0.3f, 1.2f), ScaleTo::create(0.3f, 1.0f), nullptr)));
}

void CoTuongPiece::unselect()
{
	_image_select->stopAllActions();
	_image_select->setVisible(false);
}

void CoTuongPiece::move(int new_row, int new_col)
{
	//MoveTo* action = MoveTo::create(.5f, )
}

