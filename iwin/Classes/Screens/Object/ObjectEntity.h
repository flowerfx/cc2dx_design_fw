#ifndef __BASIC_UNIT_H__
#define __BASIC_UNIT_H__

#include "Object/wObject.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace Utility;
using namespace Utility::wObject;
enum NAME_CARD
{
	CARD_A = 0,
	CARD_2,
	CARD_3,
	CARD_4,
	CARD_5,
	CARD_6,
	CARD_7,
	CARD_8,
	CARD_9,
	CARD_10,
	CARD_JACK,
	CARD_QUEEN,
	CARD_KING
};

enum ELEMENT_CARD
{
	E_SPADE = 0,
	E_CLUB,
	E_DIAMOND,
	E_HEART
};

enum DECK_TYPE
{
	DECK_TIENLEN = 0,
	DECK_MAUBINH,
	DECK_BAICAO
};

#define NUMBER_CARD 52

class ObjectEntity : public wBasicObject
{
private:
	RKString p_type_unit;

protected:
	//the object is on active or not
	bool p_IsActive;

	//the object is on select or not
	bool p_IsOnControl;

	bool p_IsInit;
public:
	ObjectEntity();
	ObjectEntity(RKString text);
	virtual ~ObjectEntity();

	virtual void Init(RKString name_obj);
	virtual void Update(float dt);

	RKString GetType() { return p_type_unit; }

	virtual bool OnFinishAction();
};
#define _RES(x) x->GetResource()
#endif //__BASIC_UNIT_H__

