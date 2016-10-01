#include "Card.h"
#include "AnimUtils.h"
//12 = 2 , 0 = 3
int MAP_MAUBINH[13] = { 12, 0, 1, 2, 3, 4, 5,
6, 7, 8, 9, 10, 11 };
 int MAP_TIENLEN[13] = { 0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12 };
 int MAP_BAICAO[13] = { 11, 12, 0, 1, 2, 3, 4,
5, 6, 7, 8, 9, 10 };

int MAP_RENDER[13] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 1 };


//int*	Card::_mapping_value= nullptr;
Card::Card()
{
	p_name_card = NAME_CARD::CARD_A;
	p_element = ELEMENT_CARD::E_SPADE;
	p_UP_status = true;
	p_ID = -1;
	_isSelected = false;
	p_IsActive = true;

	_mapping_value = nullptr;
}

Card::Card(RKString name_object, NAME_CARD name, ELEMENT_CARD e)
{
	p_name_card = name;
	p_element = e;
	p_UP_status = true;
	_isSelected = false;
	Init(name_object);
	p_IsActive = true;
}

Card::Card(RKString name_object, int id, DECK_TYPE deck_type)
{
	SetID(id, deck_type);

	Init(name_object);

	this->GetResource()->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->GetResource()->setPosition(Vec2::ZERO);
	//this->addChild(this->GetResource());
	p_IsActive = true;
}

Card::~Card()
{
	p_IsActive = true;
	p_IsOnControl = false;

	_mapping_value = nullptr;
}

void Card::Init(RKString name_obj)
{
	if (p_IsInit)
	{
		CCLOG("object already init !");
		return;
	}
	bool res = OnInitWideObject(name_obj);

	if (p_ID < -1)
	{
		//SetStatus(false);
	}
	else
	{
		InitElementChild();

		CCASSERT(res, "init object failed !");
		SetStatus(true);
	}
	p_IsInit = true;
	_isSelected = false;
	_is_lock_select = false;
}

void Card::InitElementChild()
{
	auto card_blank = this->GetObjectByName(".card_blank");

	RKString name_card = "name_";
	if (p_element < E_DIAMOND)
	{
		name_card += "black_";
	}
	else
	{
		name_card += "red_";
	}

	name_card += std::to_string(p_name_card + 1);

	card_blank->InsertObjectXMLDec(XMLMgr->GetWideObjectByName(name_card));

	card_blank->InsertObjectXMLDec(XMLMgr->GetWideObjectByName("icon_element_small_" + std::to_string(p_element + 1)));

	RKString symbal_big = "icon_element_";
	if (p_name_card <= NAME_CARD::CARD_A && p_element <= ELEMENT_CARD::E_SPADE)
	{
		symbal_big += "1_1";
	}
	else if (p_name_card >= NAME_CARD::CARD_JACK)
	{
		if (p_name_card == NAME_CARD::CARD_JACK)
		{
			symbal_big += "jack_";
		}
		else if (p_name_card == NAME_CARD::CARD_QUEEN)
		{
			symbal_big += "queen_";
		}
		else if (p_name_card == NAME_CARD::CARD_KING)
		{
			symbal_big += "king_";
		}

		if (p_element < ELEMENT_CARD::E_DIAMOND)
		{
			symbal_big += "black";
		}
		else if (p_element >= ELEMENT_CARD::E_DIAMOND)
		{
			symbal_big += "red";
		}

	}
	else
	{
		symbal_big += std::to_string(p_element + 1);
	}

	card_blank->InsertObjectXMLDec(XMLMgr->GetWideObjectByName(symbal_big));
}

void Card::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Draw(renderer, transform, flags);
}

void Card::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Visit(renderer, transform, flags);
}

void Card::Update(float dt)
{
	if (!p_IsActive)
		return;

	ObjectEntity::Update(dt);
}
bool Card::isSelected()
{
	return _isSelected;
}

void Card::ResetID(int id, DECK_TYPE deck_type)
{
	if (p_ID == id)
		return;
	if (id < 0)
	{
		this->GetObjectByName(".card_blank")->SetVisible(false);
		this->GetObjectByName(".card_up")->SetVisible(true);
		p_ID = id;
		return;
	}
	else
	{
		this->GetObjectByName(".card_blank")->SetVisible(true);
		this->GetObjectByName(".card_up")->SetVisible(false);
	}

	SetID(id, deck_type);

	auto card_blank = this->GetObjectByName(".card_blank");
	card_blank->RemoveObjectChild("", true);
	InitElementChild();
	_isSelected = false;
}

void Card::SetID(int id, DECK_TYPE deck_type)
{
	p_deck_type = deck_type;
	p_ID = id;
	if (id < -1)
	{
		return;
	}

	int number_card = id / 4; //3 bich = number_car 0 , 2 co = number_card 
	int card_type = id % 4; // bich - chuon - ro - co

	p_element = (ELEMENT_CARD)(card_type);
	if (deck_type == DECK_TYPE::DECK_TIENLEN)
	{
		_mapping_value = MAP_TIENLEN;
	}
	else if (deck_type == DECK_TYPE::DECK_BAICAO)
	{
		_mapping_value = MAP_BAICAO;
	}
	else if (deck_type == DECK_TYPE::DECK_MAUBINH)
	{
		_mapping_value = MAP_MAUBINH;
	}

	p_value = number_card;//_mapping_value[number_card];
	p_name_card = (NAME_CARD)MAP_RENDER[_mapping_value[number_card]];
}

Card * Card::NewInstance(int id, DECK_TYPE deck_type)
{
	int tmpId = id;
	if (id == -1)
	{
		id = 0;
	}
	auto ca =  new Card(id == -2 ? "card_up" : "card", id, deck_type);
	if (tmpId == -1) {
		ca->ResetID(tmpId, deck_type);
	}
	return ca;
}

void Card::SetStatus(bool stat)
{
	p_UP_status = stat;

	if (p_UP_status)
	{
		this->GetObjectByName(".card_blank")->SetVisible(true);
		this->GetObjectByName(".card_up")->SetVisible(false);
	}
	else
	{
		this->GetObjectByName(".card_blank")->SetVisible(false);
		this->GetObjectByName(".card_up")->SetVisible(true);
	}
}


char Card::getID()
{
	return p_ID;
}

int Card::GetCardType()
{
	return p_element;
}

int Card::GetCardValue()
{
	return p_value;
}

int Card::GetDeckType()
{
	return p_deck_type;
}

int Card::GetCardName()
{
	return p_name_card;
}

void Card::SelectCardAction(bool isSelect, bool anim)
{
	if (isSelect == _isSelected)
	{
		return;
	}
	_isSelected = isSelect;
	
	if (!anim) return;

	if (_is_lock_select)
	{
		PASSERT2(false, "this action is lock, why call this, check again !!!");
		return;
	}

	//_is_lock_select = true;
	if (isSelect)
	{
		AnimUtils::EffectSelectCard(this, CallFunc::create([this]() {
			this->EventActionFinish();
			auto node = this->GetObjectByName(".effect_danh_1");
			node->SetVisible(true);
			node->RunActionAnimate(CallFunc::create([this, node]() {
				node->EventActionFinish();
				node->SetVisible(false);
			}));
		}));		
	}
	else
	{
		AnimUtils::EffectDeselectCard(this, nullptr);
	}
}

void Card::ChiaBaiAction(int idx)
{
	float time_delay = 0.1f + 0.05*idx;
	Vec2 position_move_to = Vec2(150 + 50 * idx, 130);
	xml::BasicDec dec;
	dec.InsertDatafloat("DELAY", time_delay);
	dec.InsertDataVector2("POSITION", position_move_to);
	SetHookCommand("CHIA_BAI", "act0", 0, 1, dec, "DELAY");
	SetHookCommand("CHIA_BAI", "act0", 0, 2, dec, "POSITION");

	SetActionCommandWithName("CHIA_BAI");

	ClearHookCommand("CHIA_BAI", "act0", 0, 1);
	ClearHookCommand("CHIA_BAI", "act0", 0, 2);
}

void Card::enableHint(bool enable)
{
	GetObjectByName(".card_blank")->SetColor(enable ? Color4B::YELLOW : Color4B::WHITE);
}

bool Card::isHint()
{
	return GetObjectByName(".card_blank")->GetColor() == Color4B::YELLOW;
}

void Card::showHighlight(bool isShow)
{
	this->GetObjectByName(".card_hightlight")->SetVisible(isShow);
}

bool Card::isHighlight()
{
	return this->GetObjectByName(".card_hightlight")->IsVisible();
}

void Card::showTrans(int opacity)
{
	GetObjectByName(".trans")->SetVisible(true);
	GetObjectByName(".trans")->SetColor(Color4B::BLACK);
	GetObjectByName(".trans")->SetOpacity(opacity);
}

void Card::hideTrans()
{
	GetObjectByName(".trans")->SetVisible(false);
}

bool Card::containsPoint(Vec2 pos)
{
	cocos2d::Size card_size = GetResource()->getContentSize();
	cocos2d::Rect bound = cocos2d::Rect(.0f, .0f, card_size.width, card_size.height);
	Vec2 new_pos = GetResource()->convertToNodeSpace(pos);
	return bound.containsPoint(new_pos);
}