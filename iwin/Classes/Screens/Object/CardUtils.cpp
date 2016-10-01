#include "Card.h"
#include "CardUtils.h"
#include "Common/GameController.h"
#include "Screens/ScreenManager.h"
#include "Screens/GameScreens/TienLen/TienLenPlayer.h"
USING_NS_CC;

std::map<NAME_CARD, std::map<ELEMENT_CARD, int>> CardUtils::GetNumberCardSameName(std::vector<Card*> cards)
{
	std::map<NAME_CARD, std::map<ELEMENT_CARD, int>> return_value;
	int idx = 0;
	for (auto c : cards)
	{
		NAME_CARD name = (NAME_CARD)c->GetCardName();
		ELEMENT_CARD type = (ELEMENT_CARD)c->GetCardType();
		if (return_value.find(name) == return_value.end())
		{
			std::map<ELEMENT_CARD, int> tm;
			tm.insert(std::pair<ELEMENT_CARD, int>(type , idx));
			return_value.insert(std::pair<NAME_CARD, std::map<ELEMENT_CARD, int>>(name, tm));
		}
		else
		{
			auto current_card = return_value.at(name);	
		//	PASSERT2(current_card.find(type) != current_card.end(), "already element in list");
			return_value.erase(name);
			current_card.insert(std::pair<ELEMENT_CARD, int>(type, idx));
			return_value.insert(std::pair<NAME_CARD, std::map<ELEMENT_CARD, int>>(name, current_card));
		}
		idx++;
	}
	return return_value;

}

std::map<ELEMENT_CARD, std::map<NAME_CARD, int>> CardUtils::GetNumberCardSameType(std::vector<Card*> cards)
{
	std::map<ELEMENT_CARD, std::map<NAME_CARD, int>> return_value;
	int idx = 0;
	for (auto c : cards)
	{
		NAME_CARD name = (NAME_CARD)c->GetCardName();
		ELEMENT_CARD type = (ELEMENT_CARD)c->GetCardType();
		if (return_value.find(type) == return_value.end())
		{
			std::map<NAME_CARD, int> tm;
			tm.insert(std::pair<NAME_CARD, int>(name , idx));
			return_value.insert(std::pair<ELEMENT_CARD, std::map<NAME_CARD, int>>(type, tm));
		}
		else
		{
			auto current_card = return_value.at(type);
			//PASSERT2(current_card.find(name) != current_card.end(), "already element in list");
			return_value.erase(type);
			current_card.insert(std::pair<NAME_CARD, int>(name, idx));
			return_value.insert(std::pair<ELEMENT_CARD, std::map<NAME_CARD, int>>(type, current_card));
		}
		idx++;
	}
	return return_value;
}

int CardUtils::ConvertToCardID(NAME_CARD name, ELEMENT_CARD element, GameType gameType)
{
	int id = 0;
	if (gameType == GameType::GAMEID_TIENLEN || 
		gameType == GameType::GAMEID_TIENLEN_MB ||
		gameType == GameType::GAMEID_TIENLEN_SOLO)
	{
		if (name == NAME_CARD::CARD_A)
		{
			id = 11;
		}
		else if (name == NAME_CARD::CARD_2)
		{
			id = 12;
		}
		else
		{
			id = ((int)name) - 2;
		}
	}
	else if (gameType == GameType::GAMEID_BINH)
	{
		if (name == NAME_CARD::CARD_A)
		{
			id = 1;
		}
		else if (name == NAME_CARD::CARD_2)
		{
			id = 0;
		}
	}
	else if (gameType == GameType::GAMEID_BAICAO
		|| gameType == GameType::GAMEID_BAICAORUA)
	{
		
	}

	id = id * 4;
	id = id + (int)element;
	return id;
}

std::pair<NAME_CARD, ELEMENT_CARD> CardUtils::ConvertToCardName(int id, GameType gameType)
{
	std::pair<NAME_CARD, ELEMENT_CARD> return_value;
	if (gameType == GameType::GAMEID_TIENLEN ||
		gameType == GameType::GAMEID_TIENLEN_MB ||
		gameType == GameType::GAMEID_TIENLEN_SOLO)
	{
		NAME_CARD name;
		int type_card = id / 4;
		ELEMENT_CARD element_card = (ELEMENT_CARD)(id % 4);
		if (type_card == 12)
		{
			name = NAME_CARD::CARD_2;
		}
		else if (type_card == 11)
		{
			name = NAME_CARD::CARD_A;
		}
		else
		{
			name = NAME_CARD(type_card + 2);
		}
		return_value = std::pair<NAME_CARD, ELEMENT_CARD>(name, element_card);
		return return_value;
	}
	else if (gameType == GameType::GAMEID_BINH)
	{
		NAME_CARD name;
		int type_card = id / 4;
		ELEMENT_CARD element_card = (ELEMENT_CARD)(id % 4);

		if (type_card == 1)
		{
			name = NAME_CARD::CARD_A;
		}
		else if (type_card == 0)
		{
			name = NAME_CARD::CARD_2;
		}
		else
		{
			name = NAME_CARD(type_card);
		}
		return_value = std::pair<NAME_CARD, ELEMENT_CARD>(name, element_card);
		return return_value;
	}
	else if (gameType == GameType::GAMEID_BAICAO
		|| gameType == GameType::GAMEID_BAICAORUA)
	{
		NAME_CARD name;
		int type_card = id / 4;
		ELEMENT_CARD element_card = (ELEMENT_CARD)(id % 4);
		name = NAME_CARD(type_card);
		return_value = std::pair<NAME_CARD, ELEMENT_CARD>(name, element_card);
		return return_value;

	}

	return return_value;
}

int CardUtils::GetNameSizeID(NAME_CARD card, GameType gameType)
{
	int pre_name_id = card;

	if (gameType == GameType::GAMEID_TIENLEN ||
		gameType == GameType::GAMEID_TIENLEN_MB ||
		gameType == GameType::GAMEID_TIENLEN_SOLO)
	{
		pre_name_id -= 2;
		if (card == NAME_CARD::CARD_A)
		{
			pre_name_id = 11;
		}
		else if (card == NAME_CARD::CARD_2)
		{
			pre_name_id = 12;
		}
		return pre_name_id;
	}
	else if (gameType == GameType::GAMEID_BINH)
	{
		if (card == NAME_CARD::CARD_A)
		{
			pre_name_id = 1;
		}
		else if (card == NAME_CARD::CARD_2)
		{
			pre_name_id = 0;
		}
		return pre_name_id;
	}
	else if (gameType == GameType::GAMEID_BAICAO
		|| gameType == GameType::GAMEID_BAICAORUA)
	{
		return pre_name_id;
	}
	return pre_name_id;
}

std::vector<unsigned char> CardUtils::FindSeries(std::vector<Card*> cards, int number_series, int highestNumber, GameType gameType, int begin_idx )
{
	std::vector<unsigned char> return_value;

	if (cards.size() < number_series)
	{
		return return_value;
	}
	int idx = begin_idx;

	for (int i = begin_idx ; i < cards.size() - number_series; i ++)
	{
		auto c = cards[i];

		if (c->GetCardName() == NAME_CARD::CARD_2)
			continue;

		return_value.push_back(idx);
		int jdx = 1;

		while(jdx <= number_series)
		{
			int j = idx + jdx;

			auto c1 = cards[j];
			int size_name = GetNameSizeID((NAME_CARD)c->GetCardName(), gameType);
			int size_name_1 = GetNameSizeID((NAME_CARD)c1->GetCardName(), gameType);
			if (size_name + jdx == size_name_1)
			{
				return_value.push_back(j);
				if (return_value.size() == number_series)
				{
					auto current_id = ConvertToCardID(
						(NAME_CARD)c1->GetCardName(),
						(ELEMENT_CARD)c1->GetCardType(),
						gameType);

					if (current_id > highestNumber)
					{
						return return_value;
					}
					else
					{
						int k = j + 1;
						while (k < cards.size())
						{
							if (cards[k]->GetCardName() != c1->GetCardName())
							{
								break;
							}

							auto current_id_1 = ConvertToCardID(
									(NAME_CARD)cards[k]->GetCardName(),
									(ELEMENT_CARD)cards[k]->GetCardType(),
									gameType);

							if (current_id > highestNumber)
							{
								return_value.erase(return_value.begin() + return_value.size());
								return_value.push_back(k);
								return return_value;
							}

							k++;
						}
					}
				}
			}

			jdx++;
		}

		return_value.clear();



		idx++;
	}

	return return_value;
}

//id1 is > id2 or otherwise
bool CompareCard(int id1, int id2, GameType gameType)
{
	return id1 > id2;
}

bool CardUtils::swap2Cards(std::vector<Card*>* cards, int cIndex, int nIndex)
{
	if (cIndex < 0 && (size_t)cIndex >= cards->size()) {
		return false;
	}
	nIndex = ((size_t)nIndex >= cards->size()) ? 0 : (nIndex < 0 ? (cards->size() - 1) : nIndex);
	Card* nCard = cards->at(nIndex);
	Card* cCard = cards->at(cIndex);
	(*cards)[nIndex] = cCard;
	(*cards)[cIndex] =  nCard;
	// #if DefaultConfiguration
	// @ // System.out.println("Sort: " + cCard.x + " to " + nCard.x);
	// #endif
	float tempX = nCard->GetX();
	if (cIndex > nIndex) {
		nCard->SetX(cCard->GetX());// nCard.x = cCard.x;//
								 // TienLenPlayer.deltaX;
		cCard->SetX(tempX);// TienLenPlayer.deltaX;
	}
	else {
		nCard->SetX(cCard->GetX());// TienLenPlayer.deltaX;
		cCard->SetX(tempX);// TienLenPlayer.deltaX;
	}
	return true;
}

void CardUtils::sortByOrder(std::vector<Card*>& cards) {
	if (cards.empty()) return;
	Card* temp;
	for (size_t i = 0; i < cards.size() - 1; i++) {
		for (size_t j = i + 1; j < cards.size(); j++) {
			if (cards.at(i)->getID() > cards.at(j)->getID()) {
				temp = cards.at(i);
				cards[i] = cards.at(j);
				cards[j] = temp;
			}
		}
	}
}

void CardUtils::sortByOrderTypeCatte(std::vector<Card*>* cards)
{
	for (size_t i = 0; i < cards->size() - 1; i++) {
		for (size_t j = i + 1; j < cards->size(); j++) {
			if ((cards->at(i)->getID() + 4) % 52 >(cards->at(j)->getID() + 4) % 52) {
				//GameController.playSound(iWinSound.CHIABAI);
				swap2Cards(cards, i, j);
			}
		}
	}
}


void CardUtils::sortByOrderTypeCatte(unsigned char* cards, int len)
{
	for (int i = 0; i < len - 1; i++) {
		for (int j = i + 1; j < len; j++) {
			if ((cards[i] + 4) % 52 >(cards[j] + 4) % 52) {
				//GameController.playSound(iWinSound.CHIABAI);
				ubyte temp;
				temp = cards[i];
				cards[i] = cards[j];
				cards[j] = temp;
			}
		}
	}
}


void CardUtils::sortBySuit(std::vector<Card*>* cards)
{
	for (size_t i = 0; i < cards->size() - 1; i++) {
		for (size_t j = i + 1; j < cards->size(); j++) {
			if (cards->at(i)->GetCardType() > cards->at(j)->GetCardType()) {
				//GameController.playSound(iWinSound.CHIABAI);
				swap2Cards(cards, i, j);
			}
		}
	}
}


int CardUtils::compareBySuitOrder(Card* o1, Card* o2)
{
	// dinh nghia lai lon la gi trong viec sap xep dong chat tang dan
	// lon hon la 1 nho hon la 0
	if ((o1->GetCardValue() == 12 || o2->GetCardValue() == 12) && o1->GetCardType() != DECK_TYPE::DECK_MAUBINH) {
		// truong hop co heo
		if (o1->GetCardValue() == o2->GetCardValue()) {
			if (o1->GetCardType() > o2->GetCardType())
				return 1;
			else
				return -1;
		}
		else {
			if (o1->GetCardValue() > o2->GetCardValue())
				return 1;
			else
				return -1;
		}
	}
	else if (o1->GetCardType() < o2->GetCardType()) {
		return -1;
	}
	else if (o1->GetCardType() > o2->GetCardType()) {
		return 1;
	}
	else {
		if (o1->getID() < o2->getID()) {
			return -1;
		}
		else {
			return 1;
		}
	}
}

void CardUtils::sortBySuitOrder(std::vector<Card*>& cards)
{
	Card* temp;
	for (size_t i = 0; i < cards.size() - 1; i++) {
		for (size_t j = i + 1; j < cards.size(); j++) {
			if (compareBySuitOrder(cards.at(i), cards.at(j)) > 0)
			{
				temp = cards.at(i);
				cards[i] = cards.at(j);
				cards[j] = temp;
			}
		}
	}
}

void CardUtils::sort(std::vector<unsigned char>& bai) {
	if (bai.size() <= 0)
	{
		return;
	}
	for (size_t i = 0; i < bai.size() - 1; i++) {
		for (size_t j = i + 1; j < bai.size(); j++) {
			if (bai[i] > bai[j]) {
				ubyte temp = bai[i];
				bai[i] = bai[j];
				bai[j] = temp;
			}
		}
	}
}

void CardUtils::sortTypeCatte(std::vector<unsigned char> bai) {
	for (size_t i = 0; i < bai.size() - 1; i++) {
		for (size_t j = i + 1; j < bai.size(); j++) {
			if ((bai[i] + 4) % 52 >(bai[j] + 4) % 52) {
				ubyte temp = bai[i];
				bai[i] = bai[j];
				bai[j] = temp;
			}
		}
	}
}

int CardUtils::setCardPosition(int xCenter, int y, int maxWidth, std::vector<Card*> cards)
{
	if (cards.empty()) return 0;
	Vec2 card_size = cards[0]->GetSize();
	int width = 0;
	if (cards.size() > 1)
	{
		int delta = (int)(maxWidth - card_size.x) / (cards.size() - 1);
		if (delta > card_size.x) delta = card_size.x;
		int startX = xCenter - (delta * (cards.size() - 1) + card_size.x) / 2;
		for (size_t i = 0; i < cards.size(); i++) {
			cards.at(i)->SetPosition(Vec2(startX, cards.at(i)->isSelected() ? cards.at(i)->GetY() : y));
			startX += delta;
			width += delta;
		}
	}
	else
	{
		cards.at(0)->SetPosition(Vec2(xCenter - card_size.x / 2, y));
		width = card_size.x;
	}
	return width;
}

bool CardUtils::checkForceFinish(std::vector<Card*>* cards, bool newgame) {
	std::vector<unsigned char> bai;
	for (size_t i = 0; i < cards->size(); i++) {
		bai.push_back(cards->at(i)->getID());
	}
	int i;
	int count;
	// tu quy heo
	for (i = 9; i < 13; i++) {
		if (bai[i] / 4 != 12) {
			break;
		}
	}
	if (i == 13) {
		return true;
	}
	// tu quy 3 khi new game
	for (i = 0; i < 4; i++) {
		if (bai[i] / 4 != 0) {
			break;
		}
	}
	if (i == 4 && newgame) {
		return true;
	}
	// sanh rong

	if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN
		|| GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_SOLO
		|| GameController::getCurrentGameType() == GameType::GAMEID_XAM) {// tien
																		// len
																		// mien
																		// nam
		count = 0;
		for (i = 0; i < 12; i++) {
			if (bai[i] / 4 == bai[i + 1] / 4 - 1 && bai[i + 1] / 4 != 12) {
				count++;
			}
			else if (bai[i] / 4 != bai[i + 1] / 4) {
				break;
			}
		}
		if (count >= 11) {
			return true;
		}
	}
	else if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_MB) {// tien
																				 // len
																				 // mien
																				 // bac
		int countSanhRong = 0;
		for (i = 0; i < 12; i++) {
			for (size_t j = i + 1; j < 13; j++) {
				if (bai[i] % 4 == bai[j] % 4 && bai[i] / 4 == bai[j] / 4 - 1 && bai[j] / 4 != 12) {
					countSanhRong++;
					break;
				}
			}
		}
		if (countSanhRong >= 11) {
			// #if DefaultConfiguration
			// @ //System.out.println(" toi trang sanh rong: " +
			// @ // countSanhRong);
			// @ Gdx.app.log("Card Utils", " toi trang sanh rong: " +
			// @ // countSanhRong);
			// #endif
			return true;
		}
	}
	// 6 doi bat ky
	if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN
		|| GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_SOLO
		|| GameController::getCurrentGameType() == GameType::GAMEID_XAM) {// tien
																		// len
																		// mien
																		// nam
		count = 0;
		for (i = 0; i < 12; i++) {
			if (bai[i] / 4 == bai[i + 1] / 4) {
				count++;
				i++;
			}
		}
		if (count >= 6) {
			return true;
		}
	}
	// 5 doi thong
	if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN
		|| GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_SOLO
		|| GameController::getCurrentGameType() == GameType::GAMEID_XAM) {
		if (demDoiThong(bai) >= 5) {
			return true;
		}
	}
	else if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_MB) {

		//if (is5DoiThong(bai) >= 5) {
		//	return true;
		//}
	}

	if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_MB) {// tien
																			// len
																			// mien
																			// bac
																			// 3 doi thong dau co quan 3 bich trong tien len mien bac
		int temp = 0;
		for (i = 0; i < 13; i++) {
			if (bai[i] == 0 || bai[i] == 1 || bai[i] == 4 || bai[i] == 5 || bai[i] == 8 || bai[i] == 9) {
				temp++;
			}
		}
		if (temp == 6) {
			// #if DefaultConfiguration
			// @ // System.out.println("toi trang 3 doi thong co 3 pich");
			// @ Gdx.app.log("Card Utils",
			// @ // "toi trang 3 doi thong co 3 pich");
			// #endif
			return true;
		}
		// truong hop 6 doi le dong mau
		int countle = 0, countchan = 0;
		for (i = 0; i < 12; i++) {
			if (bai[i] % 4 == 0 && bai[i] / 4 == bai[i + 1] / 4 && bai[i] % 4 == bai[i + 1] % 4 - 1) {
				countle++;
				i++;
			}
			if (bai[i] % 4 == 2 && bai[i] / 4 == bai[i + 1] / 4 && bai[i] % 4 == bai[i + 1] % 4 - 1) {
				countchan++;
				i++;
			}
		}
		if (countle >= 6 || countchan >= 6) {
			// #if DefaultConfiguration
			// @
			// @ //
			// @ //
			// //System.out.println("toi trang truong hop 6 doi le dong
			// mau");
			// @ Gdx.app.log("Card Utils",
			// @ // "toi trang truong hop 6 doi le dong mau");
			// #endif
			return true;
		}
		// 4 sam co ko can lien tiep
		int countSamco = 0;
		for (i = 0; i < 12; i++) {
			for (size_t j = i + 1; j < 12; j++) {
				if (bai[i] / 4 == bai[j] / 4 && bai[i] / 4 == bai[j + 1] / 4) {
					countSamco++;
					break;
				}
			}

		}
		// #if DefaultConfiguration
		// @ Gdx.app.log("Card Utils", " 4 sam co: " + countSamco);
		// #endif
		if (countSamco >= 4) {
			// #if DefaultConfiguration
			// @ Gdx.app.log("Card Utils", "toi trang khi co 4 sam co: ");
			// #endif
			return true;
		}
	}

	return false;
}

int CardUtils::demDoiThong(std::vector<unsigned char> bai) {
	// boolean valid = false;
	int max = 0;
	int count = 0;
	for (size_t i = 0; i < bai.size() - 1; i++) {
		if (bai.size() >= 48) {
			break;
		}
		if (count == 0 && bai[i] / 4 == bai[i + 1] / 4) {
			count = 1;
			// valid = true;
		}
		else {
			if (count % 2 != 0) {
				if (bai[i] / 4 == bai[i + 1] / 4 - 1) {
					count++;
				}
				else if (bai[i] / 4 != bai[i + 1] / 4) {
					// valid = false;
					if (count > max) {
						max = count;
					}
					count = 0;
				}
			}
			else {
				if (bai[i] / 4 == bai[i + 1] / 4) {
					count++;
				}
				else {
					// valid = false;
					if (count > max) {
						max = count;
					}
					count = 0;
				}
			}
		}
	}
	if (count > max) {
		max = count;
	}
	return (max + 1) / 2;
}

void CardUtils::setSelect(Card* c, bool isSelect)
{
	return;
	if (c->isSelected() != isSelect)
	{
		c->SelectCardAction(isSelect);
	}
}

CardUtils::CardCombo CardUtils::getType(std::vector<unsigned char> bai) {
	if (is1(bai)) {
		return TYPE_1;
	}
	if (isDoi(bai)) {
		return TYPE_2;
	}
	if (is3la(bai)) {
		return TYPE_3;
	}
	if (is3DoiThong(bai)/*
						* &&GameController.getCurrentGameType()!=GameType.
						* GAMEID_XAM
						*/) {
		return TYPE_3DOITHONG;
	}
	if (is4DoiThong(bai)/*
						* &&GameController.getCurrentGameType()!=GameType.
						* GAMEID_XAM
						*/) {
		return TYPE_4DOITHONG;
	}
	if (isTuQuy(bai)) {
		return TYPE_TUQUY;
	}
	if (isSanh(bai)) {
		return TYPE_SANH;
	}
	if (is2TuQuy(bai)) {
		return TYPE_2TUQUY;
	}
	return TYPE_INVALID;
}

bool CardUtils::is2TuQuy(std::vector<unsigned char> bai) {
	if (bai.size() == 8) {
		if ((bai[0] / 4 == bai[1] / 4 && bai[1] / 4 == bai[2] / 4 && bai[2] / 4 == bai[3] / 4) &&
			(bai[4] / 4 == bai[5] / 4 && bai[5] / 4 == bai[6] / 4 && bai[6] / 4 == bai[7] / 4)) {
			return true;
		}
	}
	return false;
}

bool CardUtils::is1(std::vector<unsigned char> bai) {
	return bai.size() == 1;
}

bool ascending_func(int i, int j) { return (i<j); }

bool CardUtils::isSanh(std::vector<unsigned char> bai) {
	if (bai.size() < 3) {
		return false;
	}

	if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN
		|| GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_SOLO
		/* || GameController.getCurrentGameType() == GameType.GAMEID_XAM */) {// tien
																			  // len
																			  // mien
																			  // nam
		for (size_t i = 1; i < bai.size(); i++) {
			if (bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
		}

	}
	else if (GameController::getCurrentGameType() == GameType::GAMEID_XAM) {
		// list luu cardNumber d? ki?m tra s?nh
		// List<int> listCardNumber = new List<int>();
		if (IsHeo(bai)) {
			std::vector<int> listCardNumber;

			for (size_t i = 0; i < bai.size(); i++) {
				listCardNumber.push_back((int)((bai[i] / 4) + 2) % 13);
			}
			std::sort(listCardNumber.begin(), listCardNumber.end(), ascending_func);
			for (size_t i = 1; i < listCardNumber.size(); i++) {
				if (listCardNumber.at(i - 1) != listCardNumber.at(i) - 1) {
					return false;
				}
			}
		}
		else {
			for (size_t i = 1; i < bai.size(); i++) {
				if (bai[i - 1] / 4 != bai[i] / 4 - 1) {
					return false;
				}
			}
		}

	}
	/*
	* else if (GameController.getCurrentGameType() == GameType.GAMEID_XAM)
	* { if (bai[0]/4==0&&bai[bai.length-2]/4==11&&bai[bai.length-1]/4==12)
	* { if (bai.length>3) { for (size_t i = 1; i < bai.length-3; i++) { if
	* (bai[i - 1] / 4 != bai[i] / 4 - 1) { return false; } } }
	*
	* }else if(bai[0]/4==0&&bai[1]/4==1&&bai[bai.length-1]/4==12) { if
	* (bai.length>3) { for (size_t i = 2; i < bai.length-2; i++) { if (bai[i -
	* 1] / 4 != bai[i] / 4 - 1) { return false; } } } }else { for (size_t i =
	* 1; i < bai.length; i++) { if (bai[i - 1] / 4 != bai[i] / 4 - 1) {
	* return false; } } } }
	*/else if (GameController::getCurrentGameType() == GameType::GAMEID_TIENLEN_MB) {
		for (size_t i = 1; i < bai.size(); i++) {// tien len mien bac
			if (bai[i - 1] % 4 != bai[i] % 4 || bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
		}
	}
	// sanh khong tan cung bang 2
	if (GameController::getCurrentGameType() != GameType::GAMEID_XAM) {
		if (bai[bai.size() - 1] / 4 == 12) {
			return false;
		}
	}
	return true;
}

bool CardUtils::IsHeo(std::vector<unsigned char> cards) {
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards[i] / 4 == 12) {
			return true;
		}
	}
	return false;
}

bool CardUtils::isDoi(std::vector<unsigned char> bai) {
	if (bai.size() == 2) {
		GameType gameType = GameController::getCurrentGameType();
		if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO
			|| gameType == GameType::GAMEID_XAM) {// tien
												 // len
												 // mien
												 // nam
			if (bai[0] / 4 == bai[1] / 4) {
				return true;
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
														  // len
														  // mien
														  // bac
			if (bai[0] / 4 == 12 && bai[1] / 4 == 12) {
				return true;
			}
			if (((bai[0] % 4 == 0 && bai[1] % 4 == 1) || (bai[0] % 4 == 2 && bai[1] % 4 == 3)
				|| (bai[0] % 4 == 1 && bai[1] % 4 == 0) || (bai[0] % 4 == 3 && bai[1] % 4 == 2))
				&& (bai[0] / 4 == bai[1] / 4)) {
				return true;
			}
		}
	}
	return false;
}

bool CardUtils::is3la(std::vector<unsigned char> bai) {
	if (bai.size() == 3) {
		if (bai[0] / 4 == bai[1] / 4 && bai[1] / 4 == bai[2] / 4) {
			return true;
		}
	}
	return false;
}

bool CardUtils::isTuQuy(std::vector<unsigned char> bai) {
	if (bai.size() == 4) {
		if (bai[0] / 4 == bai[1] / 4 && bai[1] / 4 == bai[2] / 4 && bai[2] / 4 == bai[3] / 4) {
			return true;
		}
	}
	return false;
}

bool CardUtils::is3DoiThong(std::vector<unsigned char> bai) {
	if (bai.size() != 6) {
		return false;
	}
	GameType gameType = GameController::getCurrentGameType();
	if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO
		/* || gameType == GameType.GAMEID_XAM */) {// tien
												   // len
												   // mien
												   // nam
		for (size_t i = 1; i < bai.size(); i++) {
			if (i % 2 == 1 && bai[i - 1] / 4 != bai[i] / 4) {
				return false;
			}
			if (i % 2 == 0 && bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
			if (bai[i] / 4 == 12) {
				return false;
			}
		}
	}
	else if (gameType == GameType::GAMEID_XAM) {
		return false;
	}
	else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
													  // len
													  // mien
													  // bac
		for (size_t i = 1; i < bai.size(); i++) {
			if (i % 2 == 1 && bai[i - 1] / 4 != bai[i] / 4) {
				return false;
			}
			if (i % 2 == 0 && bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
			if (i % 2 == 1 && bai[i - 1] % 4 != bai[i] % 4 - 1) {
				return false;
			}
			if (i % 2 == 0 && bai[i] % 4 != bai[i + 1] % 4 - 1) {
				return false;
			}
			if (bai[i - 1] % 2 == bai[i] % 2 || bai[i] / 4 == 12) {// truong
																   // hop
																   // chuon
																   // va ro
																   // va
																   // heo
				return false;
			}
		}
	}
	return true;
}

bool CardUtils::is4DoiThong(std::vector<unsigned char> bai) {
	if (bai.size() != 8) {
		return false;
	}
	GameType gameType = GameController::getCurrentGameType();

	if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO
		/* || gameType == GameType.GAMEID_XAM */) {
		for (size_t i = 1; i < bai.size(); i++) {
			if (i % 2 == 1 && bai[i - 1] / 4 != bai[i] / 4) {
				return false;
			}
			if (i % 2 == 0 && bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
			if (bai[i] / 4 == 12) {
				return false;
			}
		}
	}
	else if (gameType == GameType::GAMEID_XAM) {
		return false;
	}
	else if (gameType == GameType::GAMEID_TIENLEN_MB) {
		for (size_t i = 1; i < bai.size(); i++) {
			if (i % 2 == 1 && bai[i - 1] / 4 != bai[i] / 4) {
				return false;
			}
			if (i % 2 == 0 && bai[i - 1] / 4 != bai[i] / 4 - 1) {
				return false;
			}
			if (i % 2 == 1 && bai[i - 1] % 4 != bai[i] % 4 - 1) {
				return false;
			}
			if (i % 2 == 0 && bai[i] % 4 != bai[i + 1] % 4 - 1) {
				return false;
			}
			if (bai[i - 1] % 2 == bai[i] % 2 || bai[i] / 4 == 12) {// truong
																   // hop
																   // chuon
																   // va ro
																   // va
																   // heo
				return false;
			}
		}
	}
	return true;
}

int CardUtils::is5DoiThong(std::vector<unsigned char> bai) {
	int countFiveDoiThong = 0;
	for (size_t i = 1; i < bai.size(); i++) {
		if (i % 2 == 1 && bai[i - 1] / 4 == bai[i] / 4 && bai[i - 1] % 4 == bai[i] % 4 - 1) {
			countFiveDoiThong++;
		}
		if (i % 2 == 0 && bai[i - 1] / 4 == bai[i] / 4 - 1 && bai[i] % 4 == bai[i + 1] % 4 - 1) {
			countFiveDoiThong++;
		}
		// if (i % 2 == 1 && bai[i - 1] % 4 == bai[i] % 4 - 1) {
		// }
		// if (i % 2 == 0 && bai[i] % 4 == bai[i + 1] % 4 - 1) {
		// }
		if (bai[i - 1] % 2 == bai[i] % 2 || bai[i] / 4 == 12) {// truong hop
															   // chuon va
															   // ro va heo
			break;
		}
	}
	return countFiveDoiThong;
}

bool CardUtils::available(std::vector<unsigned char> bai_sapdanh, int type_bai_sapdanh, std::vector<unsigned char> baidanh, int type_baidanh) {
	// penalty = 0;
	GameType gameType = GameController::getCurrentGameType();
	switch ((CardUtils::CardCombo)type_baidanh) {

	case -1:
		if (type_bai_sapdanh != -1) {
			return true;
		}

	case TYPE_1:
		if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO
			|| gameType == GameType::GAMEID_XAM) {// tien
												 // len
												 // mien
												 // nam
												 // #if DefaultConfiguration
												 // @ // System.out.println("TIEN LEN MIEN NAM");
												 // @ Gdx.app.log("Card Utils", "TIEN LEN MIEN NAM");
												 // #endif
			if (gameType == GameType::GAMEID_XAM) {
				if (type_bai_sapdanh == TYPE_1 && bai_sapdanh[0] > baidanh[0]
					&& bai_sapdanh[0] / 4 > baidanh[0] / 4) {
					return true;
				}
			}
			else if (type_bai_sapdanh == TYPE_1 && bai_sapdanh[0] > baidanh[0]) {
				return true;
			}
			// chat heo
			if (baidanh[0] / 4 == 12) {
				if (type_bai_sapdanh == TYPE_3DOITHONG || type_bai_sapdanh == TYPE_4DOITHONG
					|| type_bai_sapdanh == TYPE_TUQUY) {
					// if (baidanh[0] % 4 < 2) {
					// penalty = money / 2;
					// penaltyDes = L.gL(62);
					// } else {
					// penalty = money;
					// penaltyDes = L.gL(63);
					// }
					return true;
				}
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
														  // len
														  // mien
														  // bac
														  // #if DefaultConfiguration
														  // @ // System.out.println("TIEN LEN MIEN BAC");
														  // @ Gdx.app.log("Card Utils", "TIEN LEN MIEN BAC");
														  // #endif
			if (type_bai_sapdanh == TYPE_1 && bai_sapdanh[0] > baidanh[0]) {
				if (bai_sapdanh[0] / 4 == 12) {
					// #if DefaultConfiguration
					// @ // System.out.println(" Danh bat ky con 2 nao");
					// @ Gdx.app.log("Card Utils",
					// @ // " Danh bat ky con 2 nao");
					// #endif
					return true;
				}
				if (bai_sapdanh[0] % 4 == baidanh[0] % 4) {
					// #if DefaultConfiguration
					// @ // System.out.println(" Danh con bai so : " +
					// @ // bai_sapdanh[0]);
					// @ Gdx.app.log("Card Utils", " Danh con bai so : " +
					// @ // bai_sapdanh[0]);
					// #endif
					return true;
				}
			}
			// chat heo
			if (baidanh[0] / 4 == 12) {
				if (type_bai_sapdanh == TYPE_3DOITHONG || type_bai_sapdanh == TYPE_4DOITHONG
					|| type_bai_sapdanh == TYPE_TUQUY) {
					// if (baidanh[0] % 4 < 2) {
					// penalty = money / 2;
					// penaltyDes = L.gL(62);
					// } else {
					// penalty = money;
					// penaltyDes = L.gL(63);
					// }
					return true;
				}
			}
		}
		else if (gameType == GameType::GAMEID_CATTE) {
			if ((bai_sapdanh[0] / 4 + 1) % 13 > (baidanh[0] / 4 + 1) % 13 && bai_sapdanh[0] % 4 == baidanh[0] % 4) {
				return true;
			}
		}
		break;

	case TYPE_2:
		if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO
			|| gameType == GameType::GAMEID_XAM) {// tien
												 // len
												 // mien
												 // nam
												 // #if DefaultConfiguration
												 // @ //System.out.println("TIEN LEN MIEN NAM");
												 // @ Gdx.app.log("Card Utils", "TIEN LEN MIEN NAM");
												 // #endif
			if (gameType == GameType::GAMEID_XAM) {
				if (type_bai_sapdanh == TYPE_2 && bai_sapdanh[1] > baidanh[1]
					&& bai_sapdanh[1] / 4 > baidanh[1] / 4) {
					return true;
				}
			}
			else if (type_bai_sapdanh == TYPE_2 && bai_sapdanh[1] > baidanh[1]) {
				return true;
			}
			// chat doi heo
			if (baidanh[0] / 4 == 12) {
				if (type_bai_sapdanh == TYPE_TUQUY || type_bai_sapdanh == TYPE_4DOITHONG) {
					// if (baidanh[1] % 4 < 2) {
					// penalty = money;
					// penaltyDes = L.gL(83);
					// } else if (baidanh[0] % 4 >= 2) {
					// penalty = 2 * money;
					// penaltyDes = L.gL(84);
					// } else {
					// penalty = money + money / 2;
					// penaltyDes = L.gL(82);
					// }
					return true;
				}
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
														  // len
														  // mien
														  // bac
														  // #if DefaultConfiguration
														  // @ //System.out.println("TIEN LEN MIEN BAC");
														  // @ Gdx.app.log("Card Utils","TIEN LEN MIEN BAC");
														  // #endif
			if (type_bai_sapdanh == TYPE_2 && bai_sapdanh[1] > baidanh[1]) {
				if ((bai_sapdanh[0] / 4 == 12) && (bai_sapdanh[1] / 4 == 12)) {
					return true;
				}
				if ((bai_sapdanh[0] % 4 == baidanh[0] % 4) && (bai_sapdanh[1] % 4 == baidanh[1] % 4)) {
					return true;
				}
			}
			// chat doi heo
			if (baidanh[0] / 4 == 12) {
				if (type_bai_sapdanh == TYPE_TUQUY || type_bai_sapdanh == TYPE_4DOITHONG) {
					// if (baidanh[1] % 4 < 2) {
					// penalty = money;
					// penaltyDes = L.gL(83);
					// } else if (baidanh[0] % 4 >= 2) {
					// penalty = 2 * money;
					// penaltyDes = L.gL(84);
					// } else {
					// penalty = money + money / 2;
					// penaltyDes = L.gL(82);
					// }
					return true;
				}
			}
		}
		break;

	case TYPE_3:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_3 && bai_sapdanh[2] > baidanh[2] && bai_sapdanh[2] / 4 > baidanh[2] / 4) {
				return true;
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO) {// tien
																								   // len
																								   // mien
																								   // nam
																								   // #if DefaultConfiguration
																								   // @ // System.out.println("TIEN LEN MIEN NAM");
																								   // @ Gdx.app.log("Card Utils","TIEN LEN MIEN NAM");
																								   // #endif
			if (type_bai_sapdanh == TYPE_3 && bai_sapdanh[2] > baidanh[2]) {
				return true;
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
														  // len
														  // mien
														  // bac
														  // #if DefaultConfiguration
														  // @ // System.out.println("TIEN LEN MIEN BAC");
														  // @ Gdx.app.log("Card Utils","TIEN LEN MIEN BAC");
														  // #endif
			if (type_bai_sapdanh == TYPE_3 && bai_sapdanh[2] > baidanh[2]) {
				if ((bai_sapdanh[0] / 4 == 12) && (bai_sapdanh[1] / 4 == 12) && (bai_sapdanh[2] / 4 == 12)) {
					return true;
				}
				if ((bai_sapdanh[0] % 4 == baidanh[0] % 4) && (bai_sapdanh[1] % 4 == baidanh[1] % 4)
					&& (bai_sapdanh[2] % 4 == baidanh[2] % 4)) {
					return true;
				}
			}
		}
		break;

	case TYPE_SANH:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_SANH && bai_sapdanh.size() == baidanh.size()
				&& bai_sapdanh[bai_sapdanh.size() - 1] > baidanh[baidanh.size() - 1]
				&& bai_sapdanh[bai_sapdanh.size() - 1] / 4 > baidanh[baidanh.size() - 1] / 4) {
				return true;
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN || gameType == GameType::GAMEID_TIENLEN_SOLO) {// tien
																								   // len
																								   // mien
																								   // nam
																								   // #if DefaultConfiguration
																								   // @ // System.out.println("TIEN LEN MIEN NAM");
																								   // @ Gdx.app.log("Card Util", "TIEN LEN MIEN NAM");
																								   // #endif
			if (type_bai_sapdanh == TYPE_SANH && bai_sapdanh.size() == baidanh.size()
				&& bai_sapdanh[bai_sapdanh.size() - 1] > baidanh[baidanh.size() - 1]) {
				return true;
			}
		}
		else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
														  // len
														  // mien
														  // bac
														  // #if DefaultConfiguration
														  // @ // System.out.println("TIEN LEN MIEN BAC SANH");
														  // @ Gdx.app.log("Card Util", "TIEN LEN MIEN BAC SANH");
														  // #endif
			if (type_bai_sapdanh == TYPE_SANH && bai_sapdanh.size() == baidanh.size()
				&& bai_sapdanh[bai_sapdanh.size() - 1] > baidanh[baidanh.size() - 1]) {
				if (bai_sapdanh[bai_sapdanh.size() - 1] / 4 == 12) {
					return false;
				}
				for (size_t i = 1; i < baidanh.size(); i++) {
					if (bai_sapdanh[i - 1] % 4 != baidanh[i] % 4) {
						// #if DefaultConfiguration
						// @//
						// @ //
						// @ //
						// System.out.println(" truong hop ko hop le 1");
						// @ Gdx.app.log("Card Util",
						// @ // " truong hop ko hop le 1");
						// #endif
						return false;
					}
				}

				for (size_t i = 1; i < bai_sapdanh.size(); i++) {
					if (bai_sapdanh[i - 1] % 4 != bai_sapdanh[i] % 4) {
						// #if DefaultConfiguration
						// @ Gdx.app.log("Card Util",
						// @ // " truong hop ko hop le 2");
						// #endif
						return false;
					}
				}
				return true;
			}
		}
		break;

	case TYPE_3DOITHONG:
		if (type_bai_sapdanh == TYPE_3DOITHONG && bai_sapdanh[5] > baidanh[5] || type_bai_sapdanh == TYPE_TUQUY
			|| type_bai_sapdanh == TYPE_4DOITHONG) {
			// penalty = money;
			// penaltyDes = L.gL(0);
			return true;
		}
		break;

	case TYPE_TUQUY:
		if (type_bai_sapdanh == TYPE_TUQUY && bai_sapdanh[3] > baidanh[3] || type_bai_sapdanh == TYPE_4DOITHONG) {
			// penalty = money + money / 2;
			// penaltyDes = L.gL(2);
			return true;
		}
		break;

	case TYPE_4DOITHONG:
		if (type_bai_sapdanh == TYPE_4DOITHONG && bai_sapdanh[7] > baidanh[7]) {
			// penalty = 2 * money;
			// penaltyDes = L.gL(1);
			return true;
		}
		break;
	}

	return false;
}

bool CardUtils::availableXam(std::vector<unsigned char> bai_sapdanh, int type_bai_sapdanh, std::vector<unsigned char> baidanh, int type_baidanh) {
	// penalty = 0;
	GameType gameType = GameController::getCurrentGameType();
	switch (type_baidanh) {

	case -1:
		if (type_bai_sapdanh != -1) {
			return true;
		}

	case TYPE_1:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_1 && bai_sapdanh[0] > baidanh[0] && bai_sapdanh[0] / 4 > baidanh[0] / 4) {
				return true;
			}
		}
		// chat heo
		if (baidanh[0] / 4 == 12) {
			if (type_bai_sapdanh == TYPE_TUQUY) {
				return true;
			}
		}
		break;

	case TYPE_2:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_2
				&& bai_sapdanh[1] > baidanh[1]
				&& bai_sapdanh[1] / 4 > baidanh[1] / 4) {
				return true;
			}
			if (baidanh[0] / 4 == 12) {
				if (type_bai_sapdanh == TYPE_2TUQUY) {
					return true;
				}
			}
		}
		break;

	case TYPE_3:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_3 && bai_sapdanh[2] > baidanh[2] && bai_sapdanh[2] / 4 > baidanh[2] / 4) {
				return true;
			}
		}
		break;

	case TYPE_SANH:
		if (gameType == GameType::GAMEID_XAM) {
			if (type_bai_sapdanh == TYPE_SANH && bai_sapdanh.size() == baidanh.size()) {
				//Sảnh có 2
				if (baidanh[baidanh.size() - 1] / 4 == 12) {
					//sảnh có A
					if (baidanh[baidanh.size() - 2] / 4 == 11) {
						//sắp đánh sảnh có 2
						if (bai_sapdanh[bai_sapdanh.size() - 1] / 4 == 12) {
							//sắp đánh sảnh k có A
							if (bai_sapdanh[bai_sapdanh.size() - 2] / 4 != 11) {
								if (bai_sapdanh[bai_sapdanh.size() - 2] / 4 > baidanh[baidanh.size() - 3] / 4) {
									return true;
								}

							}
						}
						else {
							if (bai_sapdanh[bai_sapdanh.size() - 1] / 4 > baidanh[baidanh.size() - 3] / 4) {
								return true;
							}
						}

					}
					else {
						if (bai_sapdanh[bai_sapdanh.size() - 1] / 4 > baidanh[baidanh.size() - 2] / 4) {
							return true;
						}
					}
				}
				else {
					if (bai_sapdanh[bai_sapdanh.size() - 1] > baidanh[baidanh.size() - 1]
						&& bai_sapdanh[bai_sapdanh.size() - 1] / 4 > baidanh[baidanh.size() - 1] / 4) {
						return true;
					}
				}
			}
		}
		break;

	case TYPE_TUQUY:
		if (type_bai_sapdanh == TYPE_TUQUY && bai_sapdanh[3] > baidanh[3]) {
			return true;
		}
		break;
	case TYPE_2TUQUY:
		if (type_bai_sapdanh == TYPE_2TUQUY && bai_sapdanh[7] > baidanh[7]) {
			return true;
		}
		break;
	}
	return false;
}

/**
* Tự động chọn bài cho người chơi
*
* @param currentCards
* @return : null if can not found, or foundAt if found
* @popCardUp : true : Pop các lá bài đó lên false : Không pop các lá bài đó
*            lên
*/

//std::vector<unsigned char> CardUtils::autoSelect(std::vector<unsigned char> currentCards, TienLenPlayer* player, float defaultY, bool popCardUp) {
//	int remainCard = player->cards.size;// getNoOfRemainCard();
//	ubyte currentHighest = -1; // = -1 neu khong chua chon
//	std::vector<unsigned char> currentSelectedCards = player.getSelectedCardsValue();
//
//	if (currentCards != null && currentCards.length > 0) {
//		currentHighest = currentCards[currentCards.length - 1];
//	}
//	if (currentSelectedCards != null & currentSelectedCards.length > 0) {
//		if (currentSelectedCards[currentSelectedCards.length - 1] > currentHighest) {
//			currentHighest = currentSelectedCards[currentSelectedCards.length - 1];
//		}
//	}
//
//	GameType gameType = GameType.typeOf(GameController.gameId);
//	if (popCardUp) {
//		player.unSelectAllCards(defaultY);
//	}
//	if (currentCards == null || currentCards.length == 0) { // Neu khong co
//															// quan cai nao
//															// hien tai thi
//															// chon dai 1
//															// con
//		if (player->cards.size == 0) {
//			return null;
//		}
//		// Chon tu do bai de danh
//		Array<Card> temp = new Array<Card>();
//		for (size_t i = 0; i < remainCard; i++) {
//			if (remainCard == 2 && gameType == GameType.GAMEID_TIENLEN_MB) {// con
//																			// 2
//																			// con
//																			// thi
//																			// chon
//																			// heo
//				for (size_t j = 0; j < remainCard; j++) {
//					if (player->cards.get(j).id / 4 == 12) {
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//								defaultY);
//						}
//						return new byte[]{ (byte)j };
//					}
//				}
//			}
//			if (player->cards.get(i).id > currentHighest) {
//				if (gameType == GameType.GAMEID_XAM) {
//					temp.add(player->cards.get(i));
//					if (!isHeoEnd(player->cards, temp)) {
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);
//						}
//						return new byte[]{ (byte)i };
//					}
//				}
//				else {
//					if (popCardUp) {
//						setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected, defaultY);
//					}
//					return new byte[]{ (byte)i };
//				}
//
//			}
//		}
//		// Neu nhu khong chon duoc con nao thì chon con dau tien
//		if (gameType != GameType.GAMEID_XAM) {
//			if (popCardUp) {
//				setSelectWithAnimation((player->cards.get(0)), !player->cards.get(0).isSelected, defaultY);
//			}
//			return new byte[]{ (byte)0 };
//		}
//	}
//	byte[] foundAt = null;
//	// currentCards = 1 lá
//	if (currentCards.length == 1) {
//		if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//			|| gameType == GameType.GAMEID_XAM) {
//			// currentCards= Heo
//			if (currentCards[0] == 48 || currentCards[0] == 49 || currentCards[0] == 50 || currentCards[0] == 51) {
//				// tìm 3 đôi thông
//				if (gameType != GameType.GAMEID_XAM) {
//					foundAt = findDoubleSequence(-1, player->cards, 3);
//				}
//				if (foundAt == null) {
//					// tìm tứ quý
//					foundAt = findAndSelectQuart(-1, player->cards);
//					if (foundAt == null && gameType != GameType.GAMEID_XAM) {
//						// tìm 4 đôi thông
//						foundAt = findDoubleSequence(-1, player->cards, 4);
//					}
//				}
//			}
//			if (foundAt == null) {
//				if (gameType == GameType.GAMEID_XAM) {
//					for (size_t i = 0; i < remainCard; i++) {
//						Array<Card> temp = new Array<Card>();
//						temp.add(player->cards.get(i));
//						if (player->cards.get(i).id > currentHighest
//							&& (int)(player->cards.get(i).id / 4) > (int)(currentHighest / 4)
//							&& !isHeoEnd(player->cards, temp)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai
//							// nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								temp.clear();
//								temp.add(player->cards.get(j));
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (int)(player->cards.get(i).id / 4) >(int) (currentHighest / 4)
//									&& !isHeoEnd(player->cards, temp)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//									}
//									// true;
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//							}
//							// true;
//							return new byte[]{ (byte)i };
//						}
//					}
//
//				}
//				else {
//					for (size_t i = 0; i < remainCard; i++) {
//
//						if (player->cards.get(i).id > currentHighest) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai
//							// nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//									}
//									// true;
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// = true;
//							}
//
//							return new byte[]{ (byte)i };
//						}
//					}
//				}
//				// Neu khong chọn duoc con nao va currentHighest card lon
//				// hon
//				// current Card thi
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					for (size_t i = 0; i < remainCard; i++) {
//						Array<Card> temp = new Array<Card>();
//						temp.add(player->cards.get(i));
//						if (gameType != GameType.GAMEID_XAM) {
//							if (player->cards.get(i).id > currentHighest) { // hostCards[0]
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//										defaultY);// =
//								}
//								// true;
//								return new byte[]{ (byte)i };
//							}
//						}
//						else {
//							if (player->cards.get(i).id > currentHighest
//								&& (int)(player->cards.get(i).id / 4) > (int)(currentHighest / 4)
//								&& !isHeoEnd(player->cards, temp)) { // hostCards[0]
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//										defaultY);// =
//								}
//								// true;
//								return new byte[]{ (byte)i };
//							}
//						}
//
//					}
//				}
//			}
//		}
//		else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//			// Neu khong chon duoc thi tim xem co doi thong khong
//			if (currentCards[0] / 4 == 12) {
//				foundAt = findDoubleSequence(-1, player->cards, 3);
//				if (foundAt == null) {
//					foundAt = findAndSelectQuart(-1, player->cards);
//					if (foundAt == null) {
//						foundAt = findDoubleSequence(-1, player->cards, 4);
//					}
//				}
//			}
//			if (foundAt == null) {
//				for (size_t i = 0; i < remainCard; i++) {
//					if (remainCard == 2) {// neu con 2 la va co 1 con heo
//										  // trong 2 la thi chon con heo
//						for (size_t j = 0; j < remainCard; j++) {
//							if (player->cards.get(j).id > currentHighest && player->cards.get(j).id / 4 == 12) {
//								// System.out.println("// neu con 2 la va co
//								// 1 con heo trong 2 la thi chon con heo"
//								// + ((Card) player->cards.get(j)).id);
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// = true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//					}
//
//					if (player->cards.get(i).id > currentHighest
//						&& (player->cards.get(i).id % 4 == currentHighest % 4)) {// ko
//																				// phai
//																				// con
//																				// heo
//																				// xu ly truong hop sap bai xong xem con quan bai
//																				// nao nho hon ko
//						for (size_t j = i + 1; j < remainCard; j++) {
//							if (player->cards.get(j).id < player->cards.get(i).id
//								&& player->cards.get(j).id > currentHighest
//								&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// = true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);// =
//										  // true;
//						}
//						return new byte[]{ (byte)i };
//
//					}
//					if (player->cards.get(i).id > currentHighest && // con
//																   // heo
//						(player->cards.get(i).id / 4 == 12)) {
//						// xu ly truong hop sap bai xong xem con quan bai
//						// nao nho hon ko
//						for (size_t j = i + 1; j < remainCard; j++) {
//							if (player->cards.get(j).id < player->cards.get(i).id
//								&& player->cards.get(j).id > currentHighest
//								&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// =
//												  // true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);// =
//										  // true;
//						}
//						return new byte[]{ (byte)i };
//					}
//				}
//				// Neu khong chọn duoc con nao va currentHighest card lon
//				// hon
//				// current Card thi
//				if (currentHighest > currentCards[currentCards.length - 1]) {// xet
//																			 // truong
//																			 // hop
//																			 // doi
//																			 // vitri
//																			 // rui
//																			 // lap
//					currentHighest = currentCards[currentCards.length - 1];
//					for (size_t i = 0; i < remainCard; i++) {
//						if (player->cards.get(i).id > currentHighest
//							&& (player->cards.get(i).id % 4 == currentHighest % 4)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//																					   // true;
//									}
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//											  // true;
//							}
//							return new byte[]{ (byte)i };
//						}
//						if (player->cards.get(i).id > currentHighest && // con
//																	   // heo
//							(player->cards.get(i).id / 4 == 12)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//																					   // true;
//									}
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//											  // true;
//							}
//							return new byte[]{ (byte)i };
//						}
//					}
//				}
//			}
//		}
//	}
//	else if (currentCards.length == 2) {
//		// Nếu là 2 quân bài chỉ có trường hợp // đôi
//		// Tìm xem có đôi nào lớn hơn đôi hiện tại không
//		foundAt = findAndSelectTwin(currentHighest, player->cards);
//		if (foundAt == null) {
//			if (currentCards[0] >= 48 && gameType != GameType.GAMEID_XAM) {
//				foundAt = findAndSelectQuart(-1, player->cards);
//				if (foundAt == null) {
//					foundAt = findDoubleSequence(-1, player->cards, 4);
//				}
//			}
//		}
//
//	}
//	else if (currentCards.length == 3) {
//		if (is3la(currentCards)) {
//			if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//				|| gameType == GameType.GAMEID_XAM) {
//				foundAt = findAndSelectTriple(currentHighest, player->cards);
//			}
//			else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//				foundAt = findAndSelectTripleMB(currentCards, player->cards);
//			}
//			if (foundAt == null) {
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					// foundAt = findAndSelectTriple(currentHighest,
//					// player->cards);
//					if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//						|| gameType == GameType.GAMEID_XAM) {
//						foundAt = findAndSelectTriple(currentHighest, player->cards);
//					}
//					else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//						foundAt = findAndSelectTripleMB(currentCards, player->cards);
//					}
//				}
//			}
//		}
//		else {
//			// return findSeries(currentHighest, player,hostCards.length);
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//	}
//	else if (currentCards.length == 4) {
//		if (isTuQuy(currentCards)) {
//			foundAt = findAndSelectQuart(currentHighest, player->cards);
//			if (foundAt == null && gameType != GameType.GAMEID_XAM) {
//				foundAt = findDoubleSequence(-1, player->cards, 4);
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					foundAt = findAndSelectQuart(currentHighest, player->cards);
//				}
//			}
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//
//	}
//	else if (currentCards.length == 5 || currentCards.length == 7 || currentCards.length == 9
//		|| currentCards.length == 11) {
//		foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//		//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//		// if (foundAt == null) {
//		// if (currentHighest > currentCards[currentCards.length - 1]) {
//		// currentHighest = currentCards[currentCards.length - 1];
//		// foundAt = findAndSelectSeries(currentHighest, player->cards,
//		// (byte) currentCards.length);
//		//
//		// }
//		//
//		// }
//
//	}
//	else if (currentCards.length == 6) {
//		if (is3DoiThong(currentCards) && gameType != GameType.GAMEID_XAM) {
//			foundAt = findDoubleSequence(currentHighest, player->cards, 3);
//			if (foundAt == null) {
//				foundAt = findAndSelectQuart(-1, player->cards);
//			}
//			if (foundAt == null) {
//				foundAt = findDoubleSequence(-1, player->cards, 4);
//			}
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//
//	}
//	else if (currentCards.length == 8 && gameType != GameType.GAMEID_XAM) {
//		if (is4DoiThong(currentCards)) {
//			foundAt = findDoubleSequence(currentCards[currentCards.length - 1], player->cards, 4);
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// } else {
//			// return false;
//			// }
//			// } else {
//			// return true;
//			// }
//		}
//
//	}
//	else if (currentCards.length == 10 && gameType != GameType.GAMEID_XAM) {
//		foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//		//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//		// if (foundAt == null) {
//		// if (currentHighest > currentCards[currentCards.length - 1]) {
//		// currentHighest = currentCards[currentCards.length - 1];
//		// foundAt = findAndSelectSeries(currentHighest, player->cards,
//		// (byte) currentCards.length);
//		// }
//		// }
//		// } else {
//		// return true;
//		// }
//		// if (is5DoiThong(hostCards)) {
//		// return (findDoubleSequence(hostCards[hostCards.length - 1],
//		// player, 5) != null);
//		// } else {
//		//
//		// }
//	}
//	if (foundAt != null) {
//		boolean haveSelectedCard = false;
//		for (size_t j = 0; j < player->cards.size; j++) {
//			if (player->cards.get(j).isSelected) {
//				haveSelectedCard = true;
//				break;
//			}
//		}
//		for (size_t i = 0; i < foundAt.length; i++) {
//			if (popCardUp && !haveSelectedCard) {
//				setSelectWithAnimation((player->cards.get(foundAt[i])), !player->cards.get(foundAt[i]).isSelected,
//					defaultY);// isSelected
//							  // = true;
//			}
//		}
//	}
//
//	/*
//	* if (gameType == GameType.GAMEID_XAM) { Array<Card> temp=new
//	* Array<Card>(); if (foundAt.length>0) { for (size_t i = 0; i <
//	* foundAt.length; i++) { temp.add(player->cards.get(foundAt[i])); } if
//	* (isHeoEnd(player->cards, temp)) { foundAt=null; } } }
//	*/
//	return foundAt;
//}

/**
* Hàm này để xác định user có bài phù hợp với bài trên bàn hay k?
* Khác với hàm autoSelect bên trên là: nếu như user đang chọn bài thì hàm bên trên
* sẽ so sánh với những con bài đang select, còn hàm này lấy tất cả bài của user
* so sánh với bài của user trc đánh trên bàn.
* */
//public static byte[] autoSelectAllCard(byte[] currentCards, TienLenPlayer player, final float defaultY,
//	boolean popCardUp) {
//	int remainCard = player->cards.size;// getNoOfRemainCard();
//	byte currentHighest = -1; // = -1 neu khong chua chon
//
//	if (currentCards != null && currentCards.length > 0) {
//		currentHighest = currentCards[currentCards.length - 1];
//	}
//
//	GameType gameType = GameType.typeOf(GameController.gameId);
//	if (popCardUp) {
//		player.unSelectAllCards(defaultY);
//	}
//	if (currentCards == null || currentCards.length == 0) { // Neu khong co
//															// quan cai nao
//															// hien tai thi
//															// chon dai 1
//															// con
//		if (player->cards.size == 0) {
//			return null;
//		}
//		// Chon tu do bai de danh
//		Array<Card> temp = new Array<Card>();
//		for (size_t i = 0; i < remainCard; i++) {
//			if (remainCard == 2 && gameType == GameType.GAMEID_TIENLEN_MB) {// con
//																			// 2
//																			// con
//																			// thi
//																			// chon
//																			// heo
//				for (size_t j = 0; j < remainCard; j++) {
//					if (player->cards.get(j).id / 4 == 12) {
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//								defaultY);
//						}
//						return new byte[]{ (byte)j };
//					}
//				}
//			}
//			if (player->cards.get(i).id > currentHighest) {
//				if (gameType == GameType.GAMEID_XAM) {
//					temp.add(player->cards.get(i));
//					if (!isHeoEnd(player->cards, temp)) {
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);
//						}
//						return new byte[]{ (byte)i };
//					}
//				}
//				else {
//					if (popCardUp) {
//						setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected, defaultY);
//					}
//					return new byte[]{ (byte)i };
//				}
//
//			}
//		}
//		// Neu nhu khong chon duoc con nao thì chon con dau tien
//		if (gameType != GameType.GAMEID_XAM) {
//			if (popCardUp) {
//				setSelectWithAnimation((player->cards.get(0)), !player->cards.get(0).isSelected, defaultY);
//			}
//			return new byte[]{ (byte)0 };
//		}
//	}
//	byte[] foundAt = null;
//	// currentCards = 1 lá
//	if (currentCards.length == 1) {
//		if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//			|| gameType == GameType.GAMEID_XAM) {
//			// currentCards= Heo
//			if (currentCards[0] == 48 || currentCards[0] == 49 || currentCards[0] == 50 || currentCards[0] == 51) {
//				// tìm 3 đôi thông
//				if (gameType != GameType.GAMEID_XAM) {
//					foundAt = findDoubleSequence(-1, player->cards, 3);
//				}
//				if (foundAt == null) {
//					// tìm tứ quý
//					foundAt = findAndSelectQuart(-1, player->cards);
//					if (foundAt == null && gameType != GameType.GAMEID_XAM) {
//						// tìm 4 đôi thông
//						foundAt = findDoubleSequence(-1, player->cards, 4);
//					}
//				}
//			}
//			if (foundAt == null) {
//				if (gameType == GameType.GAMEID_XAM) {
//					for (size_t i = 0; i < remainCard; i++) {
//						Array<Card> temp = new Array<Card>();
//						temp.add(player->cards.get(i));
//						if (player->cards.get(i).id > currentHighest
//							&& (int)(player->cards.get(i).id / 4) > (int)(currentHighest / 4)
//							&& !isHeoEnd(player->cards, temp)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai
//							// nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								temp.clear();
//								temp.add(player->cards.get(j));
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (int)(player->cards.get(i).id / 4) >(int) (currentHighest / 4)
//									&& !isHeoEnd(player->cards, temp)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//									}
//									// true;
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//							}
//							// true;
//							return new byte[]{ (byte)i };
//						}
//					}
//
//				}
//				else {
//					for (size_t i = 0; i < remainCard; i++) {
//
//						if (player->cards.get(i).id > currentHighest) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai
//							// nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//									}
//									// true;
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// = true;
//							}
//
//							return new byte[]{ (byte)i };
//						}
//					}
//				}
//				// Neu khong chọn duoc con nao va currentHighest card lon
//				// hon
//				// current Card thi
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					for (size_t i = 0; i < remainCard; i++) {
//						Array<Card> temp = new Array<Card>();
//						temp.add(player->cards.get(i));
//						if (player->cards.get(i).id > currentHighest
//							&& (int)(player->cards.get(i).id / 4) > (int)(currentHighest / 4)
//							&& !isHeoEnd(player->cards, temp)) { // hostCards[0]
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//							}
//							// true;
//							return new byte[]{ (byte)i };
//						}
//					}
//				}
//			}
//		}
//		else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//			// Neu khong chon duoc thi tim xem co doi thong khong
//			if (currentCards[0] / 4 == 12) {
//				foundAt = findDoubleSequence(-1, player->cards, 3);
//				if (foundAt == null) {
//					foundAt = findAndSelectQuart(-1, player->cards);
//					if (foundAt == null) {
//						foundAt = findDoubleSequence(-1, player->cards, 4);
//					}
//				}
//			}
//			if (foundAt == null) {
//				for (size_t i = 0; i < remainCard; i++) {
//					if (remainCard == 2) {// neu con 2 la va co 1 con heo
//										  // trong 2 la thi chon con heo
//						for (size_t j = 0; j < remainCard; j++) {
//							if (player->cards.get(j).id > currentHighest && player->cards.get(j).id / 4 == 12) {
//								// System.out.println("// neu con 2 la va co
//								// 1 con heo trong 2 la thi chon con heo"
//								// + ((Card) player->cards.get(j)).id);
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// = true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//					}
//
//					if (player->cards.get(i).id > currentHighest
//						&& (player->cards.get(i).id % 4 == currentHighest % 4)) {// ko
//																				// phai
//																				// con
//																				// heo
//																				// xu ly truong hop sap bai xong xem con quan bai
//																				// nao nho hon ko
//						for (size_t j = i + 1; j < remainCard; j++) {
//							if (player->cards.get(j).id < player->cards.get(i).id
//								&& player->cards.get(j).id > currentHighest
//								&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// = true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);// =
//										  // true;
//						}
//						return new byte[]{ (byte)i };
//
//					}
//					if (player->cards.get(i).id > currentHighest && // con
//																   // heo
//						(player->cards.get(i).id / 4 == 12)) {
//						// xu ly truong hop sap bai xong xem con quan bai
//						// nao nho hon ko
//						for (size_t j = i + 1; j < remainCard; j++) {
//							if (player->cards.get(j).id < player->cards.get(i).id
//								&& player->cards.get(j).id > currentHighest
//								&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//								if (popCardUp) {
//									setSelectWithAnimation((player->cards.get(j)), !player->cards.get(j).isSelected,
//										defaultY);// =
//												  // true;
//								}
//								return new byte[]{ (byte)j };
//							}
//						}
//						if (popCardUp) {
//							setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//								defaultY);// =
//										  // true;
//						}
//						return new byte[]{ (byte)i };
//					}
//				}
//				// Neu khong chọn duoc con nao va currentHighest card lon
//				// hon
//				// current Card thi
//				if (currentHighest > currentCards[currentCards.length - 1]) {// xet
//																			 // truong
//																			 // hop
//																			 // doi
//																			 // vitri
//																			 // rui
//																			 // lap
//					currentHighest = currentCards[currentCards.length - 1];
//					for (size_t i = 0; i < remainCard; i++) {
//						if (player->cards.get(i).id > currentHighest
//							&& (player->cards.get(i).id % 4 == currentHighest % 4)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//																					   // true;
//									}
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//											  // true;
//							}
//							return new byte[]{ (byte)i };
//						}
//						if (player->cards.get(i).id > currentHighest && // con
//																	   // heo
//							(player->cards.get(i).id / 4 == 12)) {
//							// xu ly truong hop sap bai xong xem con quan
//							// bai nao nho hon ko
//							for (size_t j = i + 1; j < remainCard; j++) {
//								if (player->cards.get(j).id < player->cards.get(i).id
//									&& player->cards.get(j).id > currentHighest
//									&& (player->cards.get(j).id % 4 == currentHighest % 4)) {
//									if (popCardUp) {
//										setSelectWithAnimation((player->cards.get(j)),
//											!player->cards.get(j).isSelected, defaultY);// =
//																					   // true;
//									}
//									return new byte[]{ (byte)j };
//								}
//							}
//							if (popCardUp) {
//								setSelectWithAnimation((player->cards.get(i)), !player->cards.get(i).isSelected,
//									defaultY);// =
//											  // true;
//							}
//							return new byte[]{ (byte)i };
//						}
//					}
//				}
//			}
//		}
//	}
//	else if (currentCards.length == 2) {
//		// Nếu là 2 quân bài chỉ có trường hợp // đôi
//		// Tìm xem có đôi nào lớn hơn đôi hiện tại không
//		foundAt = findAndSelectTwin(currentHighest, player->cards);
//		if (foundAt == null) {
//			if (currentCards[0] >= 48 && gameType != GameType.GAMEID_XAM) {
//				foundAt = findAndSelectQuart(-1, player->cards);
//				if (foundAt == null) {
//					foundAt = findDoubleSequence(-1, player->cards, 4);
//				}
//			}
//		}
//
//	}
//	else if (currentCards.length == 3) {
//		if (is3la(currentCards)) {
//			if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//				|| gameType == GameType.GAMEID_XAM) {
//				foundAt = findAndSelectTriple(currentHighest, player->cards);
//			}
//			else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//				foundAt = findAndSelectTripleMB(currentCards, player->cards);
//			}
//			if (foundAt == null) {
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					// foundAt = findAndSelectTriple(currentHighest,
//					// player->cards);
//					if (gameType == GameType.GAMEID_TIENLEN || gameType == GameType.GAMEID_TIENLEN_SOLO
//						|| gameType == GameType.GAMEID_XAM) {
//						foundAt = findAndSelectTriple(currentHighest, player->cards);
//					}
//					else if (gameType == GameType.GAMEID_TIENLEN_MB) {
//						foundAt = findAndSelectTripleMB(currentCards, player->cards);
//					}
//				}
//			}
//		}
//		else {
//			// return findSeries(currentHighest, player,hostCards.length);
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//	}
//	else if (currentCards.length == 4) {
//		if (isTuQuy(currentCards)) {
//			foundAt = findAndSelectQuart(currentHighest, player->cards);
//			if (foundAt == null && gameType != GameType.GAMEID_XAM) {
//				foundAt = findDoubleSequence(-1, player->cards, 4);
//				if (currentHighest > currentCards[currentCards.length - 1]) {
//					currentHighest = currentCards[currentCards.length - 1];
//					foundAt = findAndSelectQuart(currentHighest, player->cards);
//				}
//			}
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//
//	}
//	else if (currentCards.length == 5 || currentCards.length == 7 || currentCards.length == 9
//		|| currentCards.length == 11) {
//		foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//		//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//		// if (foundAt == null) {
//		// if (currentHighest > currentCards[currentCards.length - 1]) {
//		// currentHighest = currentCards[currentCards.length - 1];
//		// foundAt = findAndSelectSeries(currentHighest, player->cards,
//		// (byte) currentCards.length);
//		//
//		// }
//		//
//		// }
//
//	}
//	else if (currentCards.length == 6) {
//		if (is3DoiThong(currentCards) && gameType != GameType.GAMEID_XAM) {
//			foundAt = findDoubleSequence(currentHighest, player->cards, 3);
//			if (foundAt == null) {
//				foundAt = findAndSelectQuart(-1, player->cards);
//			}
//			if (foundAt == null) {
//				foundAt = findDoubleSequence(-1, player->cards, 4);
//			}
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// }
//			// }
//		}
//
//	}
//	else if (currentCards.length == 8 && gameType != GameType.GAMEID_XAM) {
//		if (is4DoiThong(currentCards)) {
//			foundAt = findDoubleSequence(currentCards[currentCards.length - 1], player->cards, 4);
//		}
//		else {
//			foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//			//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//			// if (foundAt == null) {
//			// if (currentHighest > currentCards[currentCards.length - 1]) {
//			// currentHighest = currentCards[currentCards.length - 1];
//			// foundAt = findAndSelectSeries(currentHighest, player->cards,
//			// (byte) currentCards.length);
//			// } else {
//			// return false;
//			// }
//			// } else {
//			// return true;
//			// }
//		}
//
//	}
//	else if (currentCards.length == 10 && gameType != GameType.GAMEID_XAM) {
//		foundAt = findAndSelectSeries(currentCards, currentHighest, player->cards, (byte)currentCards.length);
//		//foundAt = findAndSelectSeries(currentHighest, player->cards, (byte) currentCards.length);
//
//		// if (foundAt == null) {
//		// if (currentHighest > currentCards[currentCards.length - 1]) {
//		// currentHighest = currentCards[currentCards.length - 1];
//		// foundAt = findAndSelectSeries(currentHighest, player->cards,
//		// (byte) currentCards.length);
//		// }
//		// }
//		// } else {
//		// return true;
//		// }
//		// if (is5DoiThong(hostCards)) {
//		// return (findDoubleSequence(hostCards[hostCards.length - 1],
//		// player, 5) != null);
//		// } else {
//		//
//		// }
//	}
//	if (foundAt != null) {
//		boolean haveSelectedCard = false;
//		for (size_t j = 0; j < player->cards.size; j++) {
//			if (player->cards.get(j).isSelected) {
//				haveSelectedCard = true;
//				break;
//			}
//		}
//		for (size_t i = 0; i < foundAt.length; i++) {
//			if (popCardUp && !haveSelectedCard) {
//				setSelectWithAnimation((player->cards.get(foundAt[i])), !player->cards.get(foundAt[i]).isSelected,
//					defaultY);// isSelected
//							  // = true;
//			}
//		}
//	}
//
//	/*
//	* if (gameType == GameType.GAMEID_XAM) { Array<Card> temp=new
//	* Array<Card>(); if (foundAt.length>0) { for (size_t i = 0; i <
//	* foundAt.length; i++) { temp.add(player->cards.get(foundAt[i])); } if
//	* (isHeoEnd(player->cards, temp)) { foundAt=null; } } }
//	*/
//	return foundAt;
//}

bool CardUtils::isHeoEnd(std::vector<Card*> cards, std::vector<Card*> selectedCards) {
	std::vector<Card*> temp;
	bool flag = false;
	for (auto i : cards)
	{
		flag = false;
		for (auto j : selectedCards)
		{
			if (i->getID() == j->getID())
			{
				flag = true;
				break;
			}
		}
		if (!flag) temp.push_back(i);
	}

	if (temp.size() <= 0) {
		return false;
	}
	else {
		for (size_t i = 0; i < temp.size(); i++) {
			if (temp.at(i)->getID() / 4 != 12) {
				return false;
			}
		}
	}
	return true;
}

/**
* Tim doi thong
*
* @param highestCard
*            : La bai cao nhat ( byte ) can vuot qua , -1 : nếu muốn tìm
*            tùy ý
* @param player
* @return Trả về vị trí được tìm thấy, null neu khong tim thay
*/
std::vector<unsigned char> CardUtils::findDoubleSequence(int highestCard, std::vector<Card*> cards, int noPair) {
	std::vector<unsigned char> foundAt(noPair << 1);

	int length = cards.size();
	if (length < noPair << 1)
	{
		foundAt.clear();
		return foundAt;
	}

	GameType gameType = GameController::getCurrentGameType();

	if (gameType == GameType::GAMEID_TIENLEN 
		|| gameType == GameType::GAMEID_TIENLEN_SOLO) 
	{
		//foundAt.clear();



		for (int i = length - 1; i >= 0; i --) 
		{
			if (cards.at(i)->getID() > highestCard && 
				cards.at(i)->getID() < 48) 
			{
				// if (isUseSameColor
				// && highestCard != -1
				// && (cards[i] % 4 < 2 && highestCard % 4 > 1 || cards[i] %
				// 4 > 1
				// && highestCard % 4 < 2)) {
				// continue;
				// }
				int count = 0; // Biến đếm xem được mấy con rồi
							   // foundAt = new byte[(noPair << 1)];
							   // Tim cac quan bai nho hon 1,2,... n
				for (int lessAmount = 0; lessAmount < noPair; lessAmount++) 
				{
					// Tìm quân bài cần thiết, nhỏ hơn quân bài lớn nhất
					// pairIndex đơn vị
					// Mỗi bậc phải có 2 quân bài
					// Duyệt từ quân bài thứ i trở ngược lại
					ubyte lessCount = 0; // Bien dem xem co đúng 2 quân bài ở
										// cấp
										// này không
					for (int k = length - 1; k >= 0; k --) 
					{
						if (cards.at(k)->GetCardValue() == cards.at(i)->GetCardValue() - lessAmount) {
							// if (isUseSameColor
							// && (cards[k] % 4 < 2 && cards[i] % 4 > 1 ||
							// cards[k] % 4 > 1
							// && cards[i] % 4 < 2)) {
							// continue;
							// }
							lessCount++;
							foundAt[count++] = k;
							if (lessCount == 2) // Nếu đã tìm đủ 2 quân bài
												// thì
												// không cần đếm tiếp
							{
								break;
							}
						}
					}
					if (lessCount != 2) // Khong tim thay du 2 con thi thoi
					{
						break;
					}
				}
				if (count == (noPair << 1)) 
				{
					return foundAt;
				}
			}
		}
	}
	else if (gameType == GameType::GAMEID_TIENLEN_MB) 
	{
		// for (size_t i = 0; i < length - 1; i++) {
		// for (size_t j = i + 1; j < length; j++) {
		// if (((Card) cards.get(i)).id > ((Card) cards.get(j)).id) {
		// byte temp = ((Card) cards.get(i)).id;
		// ((Card) cards.get(i)).id = ((Card) cards.get(j)).id;
		// ((Card) cards.get(j)).id = temp;
		// }
		// }
		// }
		for (ubyte i = 0; i < length; i++) {
			if (cards.at(i)->getID() > highestCard && cards.at(i)->getID() < 48) {
				int count = 0; // Biến đếm xem được mấy con rồi
							   // foundAt = new byte[(noPair << 1)];
							   // Tim cac quan bai nho hon 1,2,... n
				for (int lessAmount = 0; lessAmount < noPair; lessAmount++) {
					// Tìm quân bài cần thiết, nhỏ hơn quân bài lớn nhất
					// pairIndex đơn vị
					// Mỗi bậc phải có 2 quân bài
					// Duyệt từ quân bài thứ i trở ngược lại
					ubyte lessCount = 0; // Bien dem xem co đúng 2 quân bài ở
										// cấp
										// này không
					for (ubyte k = 0; k < length; k++) {
						if (cards.at(k)->GetCardValue() == cards.at(i)->GetCardValue() - lessAmount) {
							int temp = k - 1;
							if (temp >= 0) {
								if ((cards.at(k)->GetCardValue() % 2 == 1)
									&& (cards.at(k)->getID() % 4 - 1 == cards.at(temp)->getID() % 4)
									&& (cards.at(k)->getID() % 4 == cards.at(i)->getID() % 4)) {
									lessCount++;
									foundAt[count++] = k;
									if (lessCount == 2) // Nếu đã tìm đủ 2
														// quân bài thì
														// không cần đếm tiếp
									{
										break;
									}
								}
							}
							if ((cards.at(k)->getID() % 2 == 0 && cards.at(k)->getID() % 4 == cards.at(i)->getID() % 4 - 1)) {
								lessCount++;
								foundAt[count++] = k;
								if (lessCount == 2) // Nếu đã tìm đủ 2 quân
													// bài thì
													// không cần đếm tiếp
								{
									break;
								}
							}
						}
					}
					if (lessCount != 2) // Khong tim thay du 2 con thi thoi
					{
						break;
					}
				}
				if (count == (noPair << 1)) {
					return foundAt;
				}
			}
		}
	}
	foundAt.clear();
	return foundAt;
}

std::vector<unsigned char> CardUtils::findAndSelectQuart(int highestCards, std::vector<Card*> cards)
{
	std::vector<unsigned char> foundAt;
	GameType gameType = GameController::getCurrentGameType();
	int noCards = cards.size();
	if (noCards < 4)
	{
		return foundAt;
	}

	//sort the list card as name
	auto list_card = GetNumberCardSameName(cards);
	for (auto c : list_card)
	{
		if (c.second.size() < 4)
			continue;

		for (auto t : c.second)
		{
			int cur_id = ConvertToCardID(c.first, t.first, gameType);
			if (cur_id > highestCards)
			{
				for (auto t1 : c.second)
				{
					foundAt.push_back(t1.second);
				}
				return foundAt;
			}
		}
	}
	return foundAt;

	int count = 0;
	std::vector<Card*> temp;
	for (ubyte i = noCards; i >= 0; i--)
	{
		if (cards.at(i)->getID() > highestCards && cards.at(i)->getID() < 48) 
		{
			count = 0;
			//foundAt.reserve(4);
			foundAt[count++] = i;
			for (ubyte j = noCards; --j >= 0;) 
			{
				if ((i != j) && cards.at(i)->GetCardValue() == cards.at(j)->GetCardValue()) 
				{
					foundAt[count++] = j;
					temp.push_back(cards.at(i));
					if (count == 4) 
					{
						if (gameType == GameType::GAMEID_XAM) 
						{
							if (!isHeoEnd(cards, temp)) 
							{
								return foundAt;
							}
						}
						else 
						{
							return foundAt;
						}

					}
				}
			}
		}
	}
	return foundAt;
}

std::vector<unsigned char> CardUtils::findAndSelectQuart2(std::vector<Card*> cards)
{
	std::vector<unsigned char> foundAt;
	int noCards = cards.size();
	if (noCards < 4) 
	{
		return foundAt;
	}

	//sort the list card as name
	auto list_card = GetNumberCardSameName(cards);
	for (auto c : list_card)
	{
		if (c.second.size() < 4 && c.first != NAME_CARD::CARD_2)
			continue;

		for (auto t : c.second)
		{
			foundAt.push_back(t.second);
			return foundAt;
		}
	}

	return foundAt;
}

std::vector<unsigned char> CardUtils::findAndSelectTwin(int highestCard, std::vector<Card*> cards)
{
	std::vector<unsigned char> foundAt;
	int noCards = cards.size();

	if (cards.size() < 2)
	{
		return foundAt;
	}

	GameType gameType = GameController::getCurrentGameType();
	if (gameType == GameType::GAMEID_XAM) {
		for (int i = 0; i < noCards; i++) {
			if (cards.at(i)->getID() > highestCard 
				&& (cards.at(i)->getID() / 4) > highestCard / 4) 
			{
				int temp = i;
				// xu ly truong hop sap bai xong xem con quan bai nao nho
				// hon ko
				for (int j = i + 1; j < noCards; j++) {
					if (cards.at(j)->getID() < cards.at(i)->getID() && cards.at(j)->getID() > highestCard
						&& cards.at(i)->getID() / 4 > highestCard / 4) {
						temp = j;
					}
				}
				// Tim quân bài cùng số với quân bài hiện tại
				std::vector<Card*> tempC;
				for (int k = 0; k < noCards; k++) {
					if ((temp != k) && cards.at(temp)->GetCardValue() == cards.at(k)->GetCardValue()) {
						tempC.push_back(cards.at(temp));
						tempC.push_back(cards.at(k));
						if (!isHeoEnd(cards, tempC)) {
							std::vector<unsigned char> rs = { (unsigned char)temp, (unsigned char)k };
							return rs;
						}
					}
				}
			}
		}
	}
	else if (gameType == GameType::GAMEID_TIENLEN 
		  || gameType == GameType::GAMEID_TIENLEN_SOLO
		) 
	{
		//
		auto list_card = GetNumberCardSameName(cards);
		for (auto c : list_card)
		{
			if (c.second.size() < 2 )
				continue;

			for (auto t : c.second)
			{
				int cur_id = ConvertToCardID(c.first, t.first, gameType);
				if (cur_id > highestCard )
				{
					for (auto it = c.second.rbegin(); it != c.second.rend() ; it ++)
					{
						foundAt.push_back(it->second);
						if (foundAt.size() == 2)
						{
							return foundAt;
						}
					}
				}
			}
		}
		return foundAt;
	}
	else if (gameType == GameType::GAMEID_TIENLEN_MB) {// tien
													  // len
													  // mien
													  // baccccccccccccccccc
		for (ubyte i = 0; i < noCards; i++) 
		{

			if (noCards == 4) {// neu con 3 la va co 2 con heo trong 3 la
							   // thi chon 2 con heo truoc
				int temp = numHeoinCards(cards);
				if (temp == 2) {// co 2 con heo trong bai
					for (int j = 0; j < noCards; j++) {
						int first = -1;
						if (cards.at(j)->getID() > highestCard && cards.at(j)->getID() / 4 == 12) {
							first = j;
						}
						// Tim quân bài cùng số với quân bài hiện tại
						if (first != -1) {
							for (int k = 0; k < noCards; k++) {
								if ((first != k) && (cards.at(first)->getID() / 4 == cards.at(k)->getID() / 4)) {
									// Chon 2 quan bai nay
									// System.out.println("moassssssssssssssssssssss:
									// "
									// + ((Card) cards.get(first)).id +
									// " - " + ((Card) cards.get(k)).id);
									std::vector<unsigned char> rs = {(unsigned char)first, (unsigned char)k};
									return rs;
								}
							}
						}
					}
				}
			}

			if (cards.at(i)->getID() > highestCard && cards.at(i)->getID() % 4 == highestCard % 4) 
			{
				int temp = i;
				// xu ly truong hop sap bai xong xem con quan bai nao nho
				// hon ko
				for (int j = i + 1; j < noCards; j++) {
					if (cards.at(j)->getID() < cards.at(i)->getID() && cards.at(j)->getID() > highestCard
						&& cards.at(j)->getID() % 4 == highestCard % 4
						&& cards.at(j)->getID() % 4 == cards.at(i)->getID() % 4) {

						temp = j;
					}
				}

				// Tim quân bài cùng số với quân bài hiện tại
				for (int k = 0; k < noCards; k++) {
					if ((temp != k) && (cards.at(temp)->getID() % 4 == cards.at(k)->getID() % 4 + 1)
						&& (cards.at(temp)->GetCardValue() == cards.at(k)->GetCardValue())) {
						// Chon 2 quan bai nay
						// player.selectedCards[i] = player.selectedCards[k]
						// = true;
						std::vector<unsigned char> rs = {(unsigned char)temp, (unsigned char)k};
						return rs;
					}
				}
			}
			// truong 2 co 2 doi 2 ko dong chat
			if (cards.at(i)->getID() > highestCard && cards.at(i)->getID() / 4 == 12) {
				for (int k = 0; k < noCards; k++) {
					if ((i != k) && (cards.at(i)->getID() / 4 == cards.at(k)->getID() / 4)) {
						std::vector<unsigned char> rs = {(unsigned char)i, (unsigned char)k};
						return rs;
					}
				}
			}
		}
	}
	return std::vector<unsigned char>();
}

std::vector<unsigned char> CardUtils::findAndSelectTriple(int highestCard, std::vector<Card*> cards)
{
	std::vector<unsigned char> foundAt;
	int noCards = cards.size();

	if (cards.size() < 3)
	{
		return foundAt;
	}
	GameType gameType = GameController::getCurrentGameType();
	//
	auto list_card = GetNumberCardSameName(cards);
	for (auto c : list_card)
	{
		if (c.second.size() < 3)
			continue;

		for (auto t : c.second)
		{
			int cur_id = ConvertToCardID(c.first, t.first, gameType);
			if (cur_id > highestCard)
			{
				for (auto it = c.second.rbegin(); it != c.second.rend(); it++)
				{
					foundAt.push_back(it->second);
					if (foundAt.size() == 3)
					{
						return foundAt;
					}
				}
			}
		}
	}
	return foundAt;
}

std::vector<unsigned char> CardUtils::findAndSelectTripleMB(std::vector<unsigned char> highestCard, std::vector<Card*> cards)
{
	int noCards = cards.size();// player.getNoOfRemainCard();
	for (int i = 0; i < noCards; i++) {
		if (cards.at(i)->getID() > highestCard[highestCard.size() - 1]
			&& cards.at(i)->getID() % 4 == highestCard[highestCard.size() - 1] % 4) {
			int posIndex = 0;
			ubyte posCard[2]; // Cờ báo cho biết đã chọn xong 2
										  // con
										  // Tim 2 quân bài cùng số với quân bài hiện tại
			for (int k = 0; k < noCards; k++) {
				if ((k != i) && (cards.at(i)->GetCardValue() == cards.at(k)->GetCardValue())
					&& (cards.at(k)->getID() % 4 == highestCard[highestCard.size() - 2] % 4
						|| cards.at(k)->getID() % 4 == highestCard[highestCard.size() - 3] % 4)) {
					// Chon 2 quan bai nay
					posCard[posIndex++] = k;
					if (posIndex == 2) {
						std::vector<unsigned char> rs = {(unsigned char)posCard[0], (unsigned char)posCard[1], (unsigned char)i };
						return rs;
					}

				}
			}
		}
	}
	return std::vector<unsigned char>();
}

std::vector<unsigned char> CardUtils::findAndSelectSeries(std::vector<unsigned char> currentCard, int highestCard, std::vector<Card*> cards, int length)
{
	std::vector<unsigned char> foundAt;
	int noRemainCard = cards.size();
	if (noRemainCard < length) 
	{
		return foundAt;
	}
	GameType gameType = GameController::getCurrentGameType();

	if (gameType == GameType::GAMEID_XAM) 
	{

		//sort lại mảng bài trên bàn
		std::vector<unsigned char> cardsTemp;
		for (size_t i = 0; i < currentCard.size(); i++) {
			cardsTemp.push_back(currentCard[i]);
		}

		std::sort(cardsTemp.begin(), cardsTemp.end(), ascending_func);
		highestCard = cardsTemp.at(cardsTemp.size() - 1);

		if (!(highestCard / 4 == 12)) {
			for (int i = 0; i < noRemainCard; i++) {
				//sảnh trên bàn k có con 2
				if (cards.at(i)->getID() > highestCard && (cards.at(i)->GetCardValue() != 12)
					&& cards.at(i)->getID() / 4 > highestCard / 4) {
					std::vector<unsigned char> posCard;
					posCard.reserve(length);
					posCard[0] = i;
					std::vector<Card*> temp;
					temp.push_back(cards.at(posCard[0]));
					for (int posIndex = 1; posIndex < length; posIndex++) {
						bool found = false;
						for (int k = 0; k < noRemainCard; k++) {
							if ((cards.at(k)->GetCardValue() + posIndex == cards.at(i)->GetCardValue())) {
								found = true;
								posCard[posIndex] = k;
								temp.push_back(cards.at(posCard[posIndex]));
								if (posIndex == length - 1 && !isHeoEnd(cards, temp)) {
									return posCard;
								}
								break;
							}

						}
						if (!found) {
							break;
						}
					}
					// }
				}
			}

		}
		else {
			//sảnh trên bàn có con 2
			//sảnh trên bàn có con A
			if (cardsTemp.at(cardsTemp.size() - 2) / 4 == 11) {
				for (ubyte i = 0; i < noRemainCard; i++) {
					if (cards.at(i)->getID() > cardsTemp.at(cardsTemp.size() - 3)
						&& cards.at(i)->getID() / 4 > cardsTemp.at(cardsTemp.size() - 3) / 4) {
						std::vector<unsigned char> posCard;
						posCard.reserve(length);
						posCard[0] = i;
						std::vector<Card*> temp;
						temp.push_back(cards.at(posCard[0]));
						for (int posIndex = 1; posIndex < length; posIndex++) {
							bool found = false;
							for (int k = 0; k < noRemainCard; k++) {
								if ((cards.at(k)->GetCardValue() + posIndex == cards.at(i)->GetCardValue())) {
									found = true;
									posCard[posIndex] = k;
									temp.push_back(cards.at(posCard[posIndex]));
									//nếu con thứ 2 của sảnh là con 3, thì kiếm cho nó con 2 để 
									//return sảnh
									if (posIndex == length - 2 && cards.at(posCard[posIndex])->getID() / 4 == 0) {
										for (int j = 0; j < noRemainCard; j++) {
											if (cards.at(j)->GetCardValue() == 12) {
												posCard[posIndex + 1] = j;
												temp.push_back(cards.at(posCard[posIndex + 1]));
												if (!isHeoEnd(cards, temp)) {
													return posCard;
												}
											}
										}
									}
									if (posIndex == length - 1 && !isHeoEnd(cards, temp)) {
										return posCard;
									}
									break;
								}

							}
							if (!found) {
								break;
							}
						}
					}
				}
			}
			else {
				//sảnh trên bàn k có con A
				for (ubyte i = 0; i < noRemainCard; i++) {
					if (cards.at(i)->getID() > cardsTemp.at(cardsTemp.size() - 2)
						&& cards.at(i)->getID() / 4 > cardsTemp.at(cardsTemp.size() - 2) / 4
						&& cards.at(i)->getID() / 4 != 12) {
						std::vector<unsigned char> posCard;
						posCard.reserve(length);
						posCard[0] = i;
						std::vector<Card*> temp;
						temp.push_back(cards.at(posCard[0]));
						for (int posIndex = 1; posIndex < length; posIndex++) {
							bool found = false;
							for (int k = 0; k < noRemainCard; k++) {
								if ((cards.at(k)->GetCardValue() + posIndex == cards.at(i)->GetCardValue())) {
									found = true;
									posCard[posIndex] = k;
									temp.push_back(cards.at(posCard[posIndex]));
									if (posIndex == length - 1 && !isHeoEnd(cards, temp)) {
										return posCard;
									}
									break;
								}

							}
							if (!found) {
								break;
							}
						}
					}
				}
			}
		}
	}
	else if (gameType == GameType::GAMEID_TIENLEN 
		  || gameType == GameType::GAMEID_TIENLEN_SOLO) 
	{
		//foundAt = FindSeries(cards, currentCard.size(), highestCard, gameType, 0);

		//return foundAt;

		//foundAt.reserve()
		for (int i = 0; i < noRemainCard; i++) 
		{
			if (cards.at(i)->getID() > highestCard && (cards.at(i)->GetCardValue() != 12)) 
			{
				std::vector<unsigned char> posCard(length,0);
				//posCard.reserve(length);
				posCard[0] = i;
				for (int posIndex = 1; posIndex < length; posIndex++) 
				{
					bool found = false;
					for (int k = 0; k < noRemainCard; k++) 
					{
						if ((cards.at(k)->GetCardValue() + posIndex == cards.at(i)->GetCardValue())) 
						{
							found = true;
							posCard[posIndex] = k;
							if (posIndex == length - 1) 
							{
								return posCard;
							}
							break;
						}

					}
					if (!found) {
						break;
					}
				}
				// }
			}
		}
		return foundAt;
	}
	else if (gameType == GameType::GAMEID_TIENLEN_MB) 
	{
		for (int i = 0; i < noRemainCard; i++) {
			if (cards.at(i)->getID() > highestCard && cards.at(i)->getID() % 4 == highestCard % 4
				&& (cards.at(i)->GetCardValue() != 12)) {
				std::vector<unsigned char> posCard;
				posCard.reserve(length);
				posCard[0] = i;
				for (int posIndex = 1; posIndex < length; posIndex++) {
					bool found = false;
					for (int k = 0; k < noRemainCard; k++) {
						if ((cards.at(k)->GetCardValue() + posIndex == cards.at(i)->GetCardValue())
							&& cards.at(k)->getID() % 4 == cards.at(i)->getID() % 4) {
							found = true;
							posCard[posIndex] = k;
							if (posIndex == length - 1) {
								return posCard;
							}
							break;
						}

					}
					if (!found) {
						break;
					}
				}
				// }
			}
		}
	}
	return foundAt;
}


ubyte CardUtils::numHeoinCards(std::vector<Card*> cards) {
	ubyte count = 0;
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards.at(i)->getID() / 4 == 12) {
			count++;
		}
	}
	return count;
}

VecByte CardUtils::convertCardsToIds(std::vector<Card*> cards) {
	VecByte ids;
	for (size_t i = 0; i < ids.size(); i++) {
		ids.push_back(cards.at(i)->getID());
	}
	return ids;
}

//
//public static void setSelect(Card c, boolean val) {
//	if (c.isSelected != val && c.getTouchable() == Touchable.enabled) {
//		GameController.playSound(iWinSound.CHIABAI);
//		if (c.isSelected == val)
//			return;
//		c.isSelected = val;
//
//		Array<Action> actions = c.getActions();
//		for (Action action : actions) {
//			if (action instanceof MoveToAction) {
//				c.setY(((MoveToAction)action).getY());
//			}
//		}
//
//		c.clearActions();
//
//		c.addAction(
//			Actions.moveTo(c.getX(), c.isSelected ? (c.getY() + OFFSET_UP) : (c.getY() - OFFSET_UP), 0.15f));
//	}
//}

/**
*
* @param c
* @param val
* @param defaultY
*            : y mac dinh (de tranh truong hop bai bi di len hoac xuong qua
*            nhieu)
*/
//public static void setSelectWithAnimation(Card c, boolean val, final float defaultY) {
//	if (c == null)
//		return;
//	if (c.isSelected != val && c.getTouchable() == Touchable.enabled) {
//		GameController.playSound(iWinSound.CHON_BAI);
//		if (c.isSelected == val)
//			return;
//		c.isSelected = val;
//		if (c.isSelected) {
//			AnimationActor animationActor = new AnimationActor(
//				CommonAsset.gamescreen_tienlen_screen_effect_exploding_circles, 0.025f, new IActionListener(){
//
//				@Override
//				public void actionPerformed(Object actor) {
//				((AnimationActor)actor).remove();
//			}
//			});
//			animationActor.start();
//			animationActor.setScaleType(ScaleType.STRETCH);
//			animationActor.setSize(c.getWidth() * 1.2f, c.getWidth() * 1.3f);
//			animationActor.setPosition(c.getX() + (c.getWidth() - animationActor.getWidth()) / 2,
//				c.getY() + c.getHeight() - animationActor.getHeight() / 2 + OFFSET_UP);
//			if (c.getParent() != null)
//				c.getParent().addActor(animationActor);
//		}
//		AnimationUtils.selectCard(c, OFFSET_UP, defaultY);
//	}
//}
bool doCompareValue_CRChB1( byte o1, byte o2) {
	if (o1 == o2)
		return false;
	else {
		byte v1 = (byte)(o1 / 4), v2 = (byte)(o2 / 4);
		byte t1 = (byte)(o1 % 4), t2 = (byte)(o2 % 4);
		if (v1 < v2) {
			return true;
		}
		else if (v1 > v2)
			return false;
		else {
			return (t1 < t2);
		}
	
	}
}

bool doCompareValue_CRChB2(byte o1, byte o2) {
	if (o1 == o2)
		return false;
	else {
		byte v1 = (byte)(o1 / 4), v2 = (byte)(o2 / 4);
		byte t1 = (byte)(o1 % 4), t2 = (byte)(o2 % 4);
		if (v1 < v2) {
			return false;
		}
		else if (v1 > v2)
			return true;
		else {
			return (t1 < t2);
		}
	}
}


/**
* sắp xếp theo thứ tự nút, với chất đi từ trái sang phải : cơ rô chuồn bích
*
* @param asc
* @param cards
*/
void CardUtils::sortByValue_CRChB(bool asc, VecByte& cards) {
	if (!cards.empty()) {
		std::sort(cards.begin(), cards.end(), asc ? doCompareValue_CRChB1 : doCompareValue_CRChB2);
	}
}

bool doCompareValue_CRChB_Card1(Card* c1, Card* c2)
{
	if (c1 != nullptr && c2 != nullptr)
		return doCompareValue_CRChB1(c1->getID(), c2->getID());
	else {
		if (c1 == nullptr && c2 == nullptr)
			return true;
		else {
			//if (!asc) {
				if (c1 == nullptr)
					return true;
				else
					return false;
			//}
			//else {
			//	if (c2 == nullptr)
			//		return 1;
			//	else
			//		return -1;
			//}
		}
	}
}

bool doCompareValue_CRChB_Card2(Card* c1, Card* c2)
{
	if (c1 != nullptr && c2 != nullptr)
		return doCompareValue_CRChB2(c1->getID(), c2->getID());
	else {
		if (c1 == nullptr && c2 == nullptr)
			return true;
		else {
			if (c2 == nullptr)
				return true;
			else
				return false;
		}
	}
}

void CardUtils::sortByValue_CRChB_Cards(bool asc, std::vector<Card*>& cards) {
	if (!cards.empty()) {
		std::sort(cards.begin(), cards.end(), asc ? doCompareValue_CRChB_Card1 : doCompareValue_CRChB_Card2);
	}
}

std::vector<cocos2d::Vec2> CardUtils::AligneCardPosition(float leftX, float y, float desX,  int number_cards, Vec2 size_card, bool aligneX)
{
	bool move_to_center = true;

	std::vector<cocos2d::Vec2> return_value;

	if (number_cards > 0)
	{
		float delta_x = abs(leftX - desX);
		float w_list_card = size_card.x * number_cards;
		if (delta_x > w_list_card)
		{
			float w_center = (delta_x / 2.f);
			float w_delta_aligne = ((w_center > (w_list_card / 2)) ? (w_center - (w_list_card / 2)) : 0);

			for (int idx = 0 ; idx < number_cards ; idx ++)
			{
				if (aligneX)
				{
					return_value.push_back(Vec2(leftX + (size_card.x * idx) + w_delta_aligne, y));
				}
				else
				{
					return_value.push_back(Vec2( y , leftX + (size_card.x * idx) + w_delta_aligne));
				}
			}

		}
		else
		{
			float sub_delta_x = (w_list_card - delta_x) / number_cards;
			for (int idx = 0; idx < number_cards; idx++)
			{
				if (aligneX)
				{
					return_value.push_back(Vec2(leftX + (size_card.x * idx) - (sub_delta_x * idx), y));
				}
				else
				{
					return_value.push_back(Vec2(y, leftX + (size_card.x * idx) - (sub_delta_x * idx)));
				}
			}
		}
	}
	
	return return_value;
}


std::vector<cocos2d::Vec2> CardUtils::GetCardPosition(float leftX, float y, float desX,float desY, int sizeList)
{
	std::vector<cocos2d::Vec2> litPoint;
	for (size_t i = 0; i < sizeList; i++)
	{
		litPoint.push_back(Vec2(leftX, y));
		leftX += desX;
		y += desY;
	}
	return litPoint;
}

Card* CardUtils::getCardId(std::vector<Card*> cards, char id)
{
	for (Card* c :cards)
	{
		if (c->getID() == id)
		{
			return c;
		}
	}
	return nullptr;
}

std::vector<Card*> CardUtils::getCardId(std::vector<Card*> cards, std::vector<unsigned char> lsid)
{
	std::vector<Card*> lsCard;
	for (Card* c : cards)
	{
		for (char id : lsid)
		{
			if (c->getID() == (char)id)
			{
				lsCard.push_back(c) ;
				continue;
			}
		}
	}
	return lsCard;
}

int CardUtils::compareWithStraight(std::vector<Card*> cards1, std::vector<Card*> cards2)
{
	for (size_t i = 4; i >= 0; i--) {
		if (cards1[i]->getID() > cards2[i]->getID())
			return -1;
		else if (cards1[i]->getID() < cards2[i]->getID())
			return 1;

	}
	return 0;
}

void CardUtils::layoutListCardFromLeft(std::vector<Card*> list_cards, int offset_x, Vec2 pos)
{
	for (size_t i = 0; i < list_cards.size(); i++ )
	{
		list_cards[i]->SetPosition(Vec2(pos.x + i * offset_x, pos.y));
	}
}

void CardUtils::doAlignLeftListCard(std::vector<Card*> list_cards, int offset_x, Vec2 pos, int start_id, int end_id)
{
	for (int i = 0; i < end_id - start_id; i++)
	{
		list_cards[i + start_id]->SetPosition(Vec2(pos.x + i * offset_x, pos.y));
	}
}

Vec2 CardUtils::calculateCardPosCenter(Vec2 card_size, int num_card, Vec2 center, float percent_overlap)
{
	int offset = card_size.x * (1 - percent_overlap);
	int list_card_w = offset * (num_card - 1) + card_size.x;
	int startX = center.x - list_card_w / 2;
	return Vec2(startX + offset * num_card - 1, center.y);
}

std::vector<Vec2> CardUtils::calculateCardListPosCenter(Vec2 card_size, int num_card, Vec2 center, float percent_overlap)
{
	std::vector<Vec2> list_pos;
	int offset = card_size.x * (1 - percent_overlap);
	int list_card_w = offset * (num_card - 1) + card_size.x;
	int startX = center.x - list_card_w / 2;
	for (int i = 0; i < num_card; i++)
		list_pos.push_back(Vec2(startX + offset * i, center.y));
		
	return list_pos;
}

void CardUtils::layoutListCardCenter(std::vector<Card*> list_cards, Vec2 center, float percent_overlap)
{
	if (list_cards.empty()) return;
	Vec2 card_size = list_cards[0]->GetSize();
	int offset = card_size.x * (1 - percent_overlap);
	int list_card_w = offset * (list_cards.size() - 1) + card_size.x;
	int startX = center.x - list_card_w / 2;
	for(int i = 0; i < list_cards.size(); i++){
		list_cards[i]->SetPosition(Vec2(startX + offset * i, center.y));
	}
}


void CardUtils::layoutListCardCenter_Vertical(std::vector<Card*> list_cards, Vec2 center, float percent_overlap)
{
	if (list_cards.empty()) return;
	Vec2 card_size = list_cards[0]->GetSize();
	int offset = card_size.y * (1 - percent_overlap);
	int list_card_h= offset * (list_cards.size() - 1) + card_size.y;
	int startY = center.y + list_card_h / 2 - card_size.y;
	for(int i = 0; i < list_cards.size(); i++){
		list_cards[i]->SetPosition(Vec2(center.x, startY - offset * i));
	}
}

//////////////////////// just for test @PDT
std::vector<unsigned char> CardUtils::_test_cards;

void CardUtils::initCardTest()
{
	_test_cards.clear();
	for (size_t i = 0; i < num_card_test; i++)
	{
		_test_cards.push_back(i);
	}
}

ubyte CardUtils::getCardTestRandom()
{
	int rand_idx = CCRANDOM_0_1() * _test_cards.size();
	int value = _test_cards.at(rand_idx);
	_test_cards.erase(_test_cards.begin() + rand_idx);
	return value;
}

ubyte CardUtils::getCardTest(int number, int type)
{
	int value = number * 4 + type;
	for (size_t i = 0; i < _test_cards.size(); i++)
	{
		if (_test_cards[i] == value)
		{
			_test_cards.erase(_test_cards.begin() + i);
			return value;
		}
	}
	return 0;
}

ubyte CardUtils::getCardTest(int value)
{
	for (size_t i = 0; i < _test_cards.size(); i++)
	{
		if (_test_cards[i] == value)
		{
			_test_cards.erase(_test_cards.begin() + i);
			return value;
		}
	}
	return -1;
}
////////////////////////
std::vector<unsigned char> CardUtils::autoSelecWhenTouchLTMN(TienLenPlayer* player, std::vector<unsigned char> cardSelect, std::vector<unsigned char> curentCards)
{
	std::vector<unsigned char> tmpReturn;
	if (cardSelect.size()<=0 || cardSelect.size() > 2 && cardSelect.size() == 0)
	{
		return tmpReturn;
	}

	vector<Card*> listCards = player->cards;

	CardCombo typecard = getType(curentCards);

	int hightCard = cardSelect[0];
	if (curentCards.size() >0)
	{
		hightCard = curentCards[curentCards.size() - 1];
	}

	vector<unsigned char> forAtResult;
	if ((curentCards.size() > 0 && curentCards.size() <= 2 && curentCards[0] >= 48) 
			|| typecard == TYPE_3DOITHONG 
			|| typecard == TYPE_4DOITHONG 
			|| typecard == TYPE_TUQUY)
	{
		TienLenPlayer p;
		p.cards = listCards;
		forAtResult = autoSelect(curentCards,&p);
		if (forAtResult.size() >0)
			{
				for (size_t i = 0; i < cardSelect.size(); i++)
				{
					bool isExits = false;
					for (size_t j = 0; j < forAtResult.size(); j++)
					{
						if (cardSelect[i] == listCards[forAtResult[j]]->getID())
						{
							isExits = true;
						}
					}
					if (!isExits)
					{
						tmpReturn.clear();
						return tmpReturn;
					}
				}
				tmpReturn.clear();
				for (size_t j = 0; j < forAtResult.size(); j++)
				{
					tmpReturn.push_back(listCards[forAtResult[j]]->getID());
				}
				return tmpReturn;
			}
	}

	else if (cardSelect.size() == 1 && typecard != TYPE_SANH)
	{		
			if (curentCards.size() > 0)
			{
				int index = hightCard;
				if (typecard == TYPE_4DOITHONG)
				{
					forAtResult = findDoubleSequence(hightCard, listCards, 4);
				}
				else if (typecard == TYPE_TUQUY)
				{
					forAtResult = findAndSelectQuart(hightCard, listCards);
				}
				else if (typecard == TYPE_3DOITHONG)
				{
					forAtResult = findDoubleSequence(hightCard, listCards, 3);
				}
				else if (typecard == TYPE_3)
				{
					forAtResult = findAndSelectTriple(hightCard, listCards);
				}
				else if (typecard == TYPE_2)
				{
					forAtResult = findAndSelectTwin(hightCard, listCards);
				}
				while (forAtResult.size() > 0 && index <= listCards[listCards.size() - 1]->getID())
				{
					for (size_t i = 0; i < cardSelect.size(); i++)
					{
						bool isExits = false;
						for (size_t j = 0; j < forAtResult.size(); j++)
						{
							if (cardSelect[i] == listCards[forAtResult[j]]->getID())
							{
								isExits = true;
							}
						}
						if (!isExits)
						{
							if (typecard == TYPE_4DOITHONG)
							{
								forAtResult = findDoubleSequence(index, listCards, 4);
							}
							else if (typecard == TYPE_TUQUY)
							{
								forAtResult = findAndSelectQuart(index, listCards);
							}
							else if (typecard == TYPE_3DOITHONG)
							{
								forAtResult = findDoubleSequence(index, listCards, 3);
							}
							else if (typecard == TYPE_3)
							{
								forAtResult = findAndSelectTriple(index, listCards);
							}
							else if (typecard == TYPE_2)
							{
								forAtResult = findAndSelectTwin(index, listCards);
							}
						}
						else
						{
							goto FoundAuto;
						}

					}
					index++;
				}

			}

		FoundAuto:

			if (forAtResult.size()<=0 && curentCards.size() == 0 && cardSelect.size() > 1)
			{
				vector<unsigned char> listR;
				int i = 0;
				for(Card* c : listCards)
				{
					if ((int)(c->getID() / 4) == ((int)cardSelect[0] / 4))
					{
						listR.push_back(i);
					}
					i++;
				}
				if (listR.size() > 0)
				{
					forAtResult = listR;
				}
			}

			if (forAtResult.size() >0)
			{

				for (size_t i = 0; i < cardSelect.size(); i++)
				{
					bool isExits = false;
					for (size_t j = 0; j < forAtResult.size(); j++)
					{
						if (cardSelect[i] == listCards[forAtResult[j]]->getID())
						{
							//lsResult.Add(listCards[forAtResult[j]].id);
							isExits = true;
						}
					}
					if (!isExits)
					{
						return vector<unsigned char>();
					}
				}
				vector<unsigned char> lsResult ;
				for (size_t j = 0; j < forAtResult.size(); j++)
				{
					lsResult.push_back(listCards[forAtResult[j]]->getID());
				}
				return lsResult;
			}
	}
	else// chon sanh
	{
		if (cardSelect.size() == 1 && curentCards.size() >= 3)
		{
				int index = hightCard;
				forAtResult = findAndSelectSeries(curentCards,index, listCards, curentCards.size());
				while (forAtResult.size()>0 && index <= listCards[listCards.size() - 1]->getID())
				{
					for (size_t i = 0; i < cardSelect.size(); i++)
					{
						bool isExits = false;
						for (size_t j = 0; j < forAtResult.size(); j++)
						{
							if (cardSelect[i] == listCards[forAtResult[j]]->getID())
							{
								isExits = true;
							}
						}
						if (!isExits)
						{
							forAtResult = findAndSelectSeries(curentCards,index, listCards, curentCards.size());
						}
						else
						{
							goto Found;
						}

					}
					index++;
				}
			Found:
				if (forAtResult.size()>0)
				{
					vector<unsigned char> lsRs ;
					for (size_t j = 0; j < forAtResult.size(); j++)
					{
						lsRs.push_back(listCards[forAtResult[j]]->getID());
					}
					return lsRs;

				}
				else
				{
					return std::vector<unsigned char>();
				}
		}
		else if (cardSelect.size() == 2 && (int)(cardSelect[0] / 4) == ((int)cardSelect[1] / 4) - 1 && curentCards.size() <= 0)
		{
				std::vector<unsigned char> listR ;
				for (char t : cardSelect)
				{
					listR.push_back(t);
				}
				for (size_t i = 0; i < listCards.size(); i++)
				{
					if (listCards[i]->getID() > listR[listR.size() - 1] &&
						((int)(listCards[i]->getID() / 4) - 1) == (int)(listR[listR.size() - 1] / 4) &&
						listCards[i]->getID() < 48)
					{
						bool isExits = false;
						for (size_t j = 0; j < listR.size(); j++)
						{
							if ((int)(listCards[i]->getID() / 4) == (int)(listR[j] / 4))
							{
								isExits = true;
							}
						}
						if (!isExits)
						{
							listR.push_back(listCards[i]->getID());
						}

					}
				}
				if (listR.size() != 2)
				{
					return listR;
				}
				else
				{
					return vector<unsigned char>();
				}
		}
	}
	return vector<unsigned char>();
}

std::vector<unsigned char> CardUtils::autoSelect(std::vector<unsigned char> currentCards, TienLenPlayer* player)
{
	if (!player || player->cards.size()<=0)
	{
		return std::vector<unsigned char>();
	}
	vector<unsigned char> tmpReturn;
	int remainCard = player->cards.size();// getNoOfRemainCard();
	char currentHighest = -1; // = -1 neu khong chua chon
	vector<unsigned char> currentSelectedCards = player->getSelectedCardsValue();

	if (currentCards.size() >0)
	{
		currentHighest = currentCards[currentCards.size() - 1];
	}
	if (currentSelectedCards.size()> 0)
	{
		if (currentSelectedCards[currentSelectedCards.size() - 1] > currentHighest)
		{
			currentHighest = currentSelectedCards[currentSelectedCards.size() - 1];
		}
	}

	//dont let 2 action as in the function
	//player->unSelectAllCards();

	if ( currentCards.size() <= 0)
	{ // Neu khong co
	  // quan cai nao
	  // hien tai thi
	  // chon dai 1
	  // con
		if (player->cards.size() == 0)
		{
			return vector<unsigned char>();
		}
		// Chon tu do bai de danh
		for (int i = 0; i < remainCard; i++)
		{
			if (remainCard == 2
				&& GameController::gameId == GameType::GAMEID_TIENLEN_MB)
			{// con
			 // 2
			 // con
			 // thi
			 // chon
			 // heo
				for (int j = 0; j < remainCard; j++)
				{
					if ((player->cards.at(j))->getID() / 4 == 12)
					{
						setSelect(player->cards.at(j),
							!player->cards.at(j)->isSelected());
						vector<unsigned char> rt;
						rt.push_back(j);
						return rt;
					}
				}
			}
			if (player->cards[i]->getID() > currentHighest)
			{
				setSelect(player->cards.at(i),
					!player->cards.at(i)->isSelected());
				vector<unsigned char> rt;
				rt.push_back(i);
				return rt;
			}
		}
		// Neu nhu khong chon duoc con nao thì chon con dau tien
		setSelect(player->cards.at(0),
			!player->cards.at(0)->isSelected());
		vector<unsigned char> rt;
		rt.push_back(0);
		return rt;
	}

	vector<unsigned char> foundAt;
	if (currentCards.size() == 1)
	{
		
		if (GameController::gameId == GameType::GAMEID_TIENLEN
			|| GameController::gameId == GameType::GAMEID_TIENLEN_SOLO
			|| GameController::gameId == GameType::GAMEID_XAM)
		{

			// Neu khong chon duoc thi tim xem co doi thong khong

			if ((currentCards[0] == 48 || currentCards[0] == 49
				|| currentCards[0] == 50 || currentCards[0] == 51))
			{
				if (GameController::gameId != GameType::GAMEID_XAM)
				{
					foundAt = findDoubleSequence(-1, player->cards, 4);

					if (foundAt.size()<=0)
					{
						foundAt = findAndSelectQuart(-1, player->cards);
						if (foundAt.size()<= 0)
						{
							foundAt = findDoubleSequence(-1, player->cards, 3);
						}
					}
				}
				else if (GameController::gameId == GameType::GAMEID_XAM)
				{
					foundAt = findAndSelectQuart(-1, player->cards);
				}
			}

			if (foundAt.size() <=0)
			{
				if (GameController::gameId == GameType::GAMEID_XAM)
				{
					for (int i = 0; i < remainCard; i++)
					{

						if (player->cards.at(i)->getID() > currentHighest
							&& player->cards.at(i)->getID() / 4 > currentHighest / 4)
						{
							// xu ly truong hop sap bai xong xem con quan
							// bai
							// nao nho hon ko
							for (int j = i + 1; j < remainCard; j++)
							{
								if (player->cards.at(j)->getID() < player->cards.at(i)->getID()
									&& player->cards.at(j)->getID() > currentHighest
									&& player->cards.at(j)->getID() / 4 > currentHighest / 4)
								{
									setSelect(
										player->cards.at(j),
										!player->cards.at(j)->isSelected());// =
																			 // true;
									tmpReturn.clear();
									tmpReturn.push_back(j);
									return tmpReturn;
								}
							}
							setSelect(
								player->cards.at(i),
								!player->cards.at(i)->isSelected());// =
							tmpReturn.clear();
							tmpReturn.push_back(i);							 // true;
							return tmpReturn;
						}
					}

				}
				else
				{
					for (int i = 0; i < remainCard; i++)
					{
						if (player->cards.at(i)->getID() > currentHighest)
						{

							// xu ly truong hop sap bai xong xem con quan bai
							// nao nho hon ko
							for (int j = i + 1; j < remainCard; j++)
							{
								if (player->cards.at(j)->getID() < player->cards[i]->getID()
									&& player->cards.at(j)->getID() > currentHighest)
								{
									setSelect(
										player->cards.at(j),
										!player->cards.at(j)->isSelected());// =
												
																			// true;
									tmpReturn.clear();
									tmpReturn.push_back(j);
									return tmpReturn;
								}
							}
							setSelect(player->cards.at(i),
								!player->cards.at(i)->isSelected());// =
							tmpReturn.clear();
							tmpReturn.push_back(i);										 // true;
							return tmpReturn;
						}
					}
				}
				// Neu khong chọn duoc con nao va currentHighest card lon
				// hon
				// current Card thi
				if (currentHighest > currentCards[currentCards.size() - 1])
				{
					currentHighest = currentCards[currentCards.size() - 1];
					for (int i = 0; i < remainCard; i++)
					{
						if (player->cards.at(i)->getID() > currentHighest)
						{ // hostCards[0]
							setSelect(
								player->cards.at(i),
								!player->cards.at(i)->isSelected());// =
							tmpReturn.clear();
							tmpReturn.push_back(i);										 // true;
							return tmpReturn;
						}
					}
				}
			}
		}
		else if (GameController::gameId == GameType::GAMEID_TIENLEN_MB)
		{
			// Neu khong chon duoc thi tim xem co doi thong khong
			if (currentCards[0] / 4 == 12)
			{
				foundAt = findDoubleSequence(-1, player->cards, 3);
				if (foundAt.size() <=0)
				{
					foundAt = findAndSelectQuart(-1, player->cards);
					if (foundAt.size()<=0)
					{
						foundAt = findDoubleSequence(-1, player->cards, 4);
					}
				}
			}
			if (foundAt.size()<=0)
			{
				for (int i = 0; i < remainCard; i++)
				{
					if (remainCard == 2)
					{// neu con 2 la va co 1 con heo
					 // trong 2 la thi chon con heo
						for (int j = 0; j < remainCard; j++)
						{
							if (player->cards.at(j)->getID() > currentHighest
								&& player->cards.at(j)->getID() / 4 == 12)
							{
								// System.out.println("// neu con 2 la va co 1 con heo trong 2 la thi chon con heo"
								// + ((Card) player->cards.get(j)).id);
								setSelect(
									player->cards.at(j),
									!player->cards.at(j)->isSelected());// =
								tmpReturn.clear();
								tmpReturn.push_back(j);
								return tmpReturn;
							}
						}
					}

					if (player->cards.at(i)->getID() > currentHighest
						&& (player->cards.at(i)->getID() % 4 == currentHighest % 4))
					{// ko
					 // phai
					 // con
					 // heo
					 // xu ly truong hop sap bai xong xem con quan bai
					 // nao nho hon ko
						for (int j = i + 1; j < remainCard; j++)
						{
							if (player->cards.at(j)->getID() < player->cards.at(i)->getID()
								&& player->cards.at(j)->getID() > currentHighest
								&& (player->cards.at(j)->getID() % 4 == currentHighest % 4))
							{
								setSelect(
									player->cards.at(j),
									!player->cards.at(j)->isSelected());// =
								tmpReturn.clear();
								tmpReturn.push_back(j);
								return tmpReturn;
							}
						}
						setSelect(player->cards.at(i),
							!player->cards.at(i)->isSelected());// =
																 // true;
						tmpReturn.clear();
						tmpReturn.push_back(i);
						return tmpReturn;

					}
					if (player->cards.at(i)->getID() > currentHighest && // con
																	   // heo
						(player->cards.at(i)->getID() / 4 == 12))
					{
						// xu ly truong hop sap bai xong xem con quan bai
						// nao nho hon ko
						for (int j = i + 1; j < remainCard; j++)
						{
							if (player->cards.at(j)->getID() < player->cards[i]->getID()
								&& player->cards.at(j)->getID() > currentHighest
								&& (player->cards.at(j)->getID() % 4 == currentHighest % 4))
							{
								setSelect(
									player->cards.at(j),
									!player->cards.at(j)->isSelected());// =
								
								tmpReturn.clear();
								tmpReturn.push_back(j);
								return tmpReturn;
							}
						}
						setSelect(player->cards.at(i),
							!player->cards.at(i)->isSelected());// =
																 // true;
						tmpReturn.clear();
						tmpReturn.push_back(i);
						return tmpReturn;
					}
				}
				// Neu khong chọn duoc con nao va currentHighest card lon
				// hon
				// current Card thi
				if (currentHighest > currentCards[currentCards.size() - 1])
				{// xet
				 // truong
				 // hop
				 // doi
				 // vitri
				 // rui
				 // lap
					currentHighest = currentCards[currentCards.size() - 1];
					for (int i = 0; i < remainCard; i++)
					{
						if (player->cards.at(i)->getID() > currentHighest
							&& (player->cards.at(i)->getID() % 4 == currentHighest % 4))
						{
							// xu ly truong hop sap bai xong xem con quan
							// bai nao nho hon ko
							for (int j = i + 1; j < remainCard; j++)
							{
								if (player->cards.at(j)->getID() < player->cards[i]->getID()
									&& player->cards.at(j)->getID() > currentHighest
									&& (player->cards.at(j)->getID() % 4 == currentHighest % 4))
								{
									setSelect(
										player->cards.at(j),
										!player->cards.at(j)->isSelected());// =
									tmpReturn.clear();
									tmpReturn.push_back(j);										 // true;
									return tmpReturn;
								}
							}

							setSelect(
								player->cards.at(i),
								!player->cards.at(i)->isSelected());// =
							tmpReturn.clear();
							tmpReturn.push_back(i);
							return tmpReturn;
						}
						if (player->cards.at(i)->getID() > currentHighest
							&& // con heo
							(player->cards.at(i)->getID() / 4 == 12))
						{
							// xu ly truong hop sap bai xong xem con quan
							// bai nao nho hon ko
							for (int j = i + 1; j < remainCard; j++)
							{
								if (player->cards.at(j)->getID() < player->cards[i]->getID()
									&& player->cards.at(j)->getID() > currentHighest
									&& (player->cards.at(j)->getID() % 4 == currentHighest % 4))
								{
									setSelect(
										player->cards.at(j),
										!player->cards.at(j)->isSelected());// =
									tmpReturn.clear();
									tmpReturn.push_back(j);
									return tmpReturn;										 // true;
								}
							}

							setSelect(
								player->cards.at(i),
								!player->cards.at(i)->isSelected());// =
							tmpReturn.clear();
							tmpReturn.push_back(i);										 // true;
							return tmpReturn;
						}
					}
				}
			}
		}
	}
	else if (currentCards.size() == 2)
	{
		// Nếu là 2 quân bài chỉ có trường hợp // đôi
		// Tìm xem có đôi nào lớn hơn đôi hiện tại không
		foundAt = findAndSelectTwin(currentHighest, player->cards);
		if (foundAt.size()<=0 && GameController::gameId != GameType::GAMEID_XAM)
		{
			if (currentCards[0] >= 48)
			{
				foundAt = findAndSelectQuart(-1, player->cards);
				if (foundAt.size()<=0)
				{
					foundAt = findDoubleSequence(-1, player->cards, 4);
				}
			}
		}

	}
	else if (currentCards.size() == 3)
	{
		if (is3la(currentCards))
		{
			if (GameController::gameId == GameType::GAMEID_TIENLEN
				|| GameController::gameId == GameType::GAMEID_TIENLEN_SOLO
				|| GameController::gameId == GameType::GAMEID_XAM)
			{
				foundAt = findAndSelectTriple(currentHighest, player->cards);
			}
			else if (GameController::gameId == GameType::GAMEID_TIENLEN_MB)
			{
				foundAt = findAndSelectTripleMB(currentCards, player->cards);
			}

			if (foundAt.size()<=0)
			{
				if (currentHighest > currentCards[currentCards.size() - 1])
				{
					currentHighest = currentCards[currentCards.size() - 1];
					// foundAt = FindAndSelectTriple(currentHighest,
					// player->cards);
					if (GameController::gameId == GameType::GAMEID_TIENLEN
						|| GameController::gameId == GameType::GAMEID_TIENLEN_SOLO
						|| GameController::gameId == GameType::GAMEID_XAM)
					{
						foundAt = findAndSelectTriple(currentHighest,
							player->cards);
					}
				}
			}
		}
		else
		{
			// return findSeries(currentHighest, player,hostCards.Length);
			foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,
				currentCards.size());
		}
	}
	else if (currentCards.size() == 4)
	{
		if (isTuQuy(currentCards))
		{
			foundAt = findAndSelectQuart(currentHighest, player->cards);
			if (foundAt.size()<=0)
			{
				foundAt = findDoubleSequence(-1, player->cards, 4);
				if (currentHighest > currentCards[currentCards.size() - 1])
				{
					currentHighest = currentCards[currentCards.size() - 1];
					foundAt = findAndSelectQuart(currentHighest,
						player->cards);
				}
			}
		}
		else
		{
			foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,currentCards.size());
		
		}

	}
	else if (currentCards.size() == 5 || currentCards.size() == 7
		|| currentCards.size() == 9 || currentCards.size() == 11)
	{
		foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,currentCards.size() );
	}
	else if (currentCards.size() == 6)
	{
		if (is3DoiThong(currentCards))
		{
			foundAt = findDoubleSequence(currentHighest, player->cards, 3);
			if (foundAt.size()<=0)
			{
				foundAt = findAndSelectQuart(-1, player->cards);
			}
			if (foundAt.size() <=0)
			{
				foundAt = findDoubleSequence(-1, player->cards, 4);
			}
		}
		else
		{
			foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,currentCards.size() );
		}

	}
	else if (currentCards.size() == 8)
	{
		if (is4DoiThong(currentCards))
		{
			foundAt = findDoubleSequence(currentCards[currentCards.size() - 1], player->cards, 4);
		}
		else
		{
			foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,currentCards.size() );
		
		}

	}
	else if (currentCards.size() == 10)
	{
		foundAt = findAndSelectSeries(currentCards,currentHighest, player->cards,currentCards.size());
	
	}
	
	//dont let 2 action as in the function
	//if (foundAt.size() >0)
	//{
	//	for (size_t i = 0; i < foundAt.size(); i++)
	//	{
	//		setSelect((player->cards[foundAt[i]]),
	//			!(player->cards[foundAt[i]])->isSelected());// isSelected
	//														  // = true;
	//	}

	//}

	return foundAt;
}

bool CardUtils::checkCardTouch(Card* card, Vec2 touch)
{
	Vec2 pos = card->GetPosition();
	Vec2 size = card->GetSize();

	Vec2 anchor = card->GetResource()->getAnchorPoint();
	Vec2 pos_bot_left = Vec2(
		pos.x - (anchor.x * (size.x / 2)),
		pos.y - (anchor.y * (size.y / 2))
	);
	Vec2 pos_world = pos_bot_left;//Node::convertToWorldSpace(pos_bot_left);
	Vec2 size_world = NODE(card)->getParent()->convertToWorldSpace(size);
	cocos2d::Rect rec = cocos2d::Rect(pos_world.x, pos_world.y, size_world.x, size_world.y);
	return rec.containsPoint(touch);
}