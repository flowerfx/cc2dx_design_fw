#include "User.h"



/**
* Lay so diem danh gia theo su nhap thong tin cua user. Max la 100;
*/
int User::scoreUserInfo(User u) {
	int score = 0;
	if (u.province > -1) {
		score += 10;
	}

	if (!u.phoneNo.empty()) {
		score += 50;
	}
	if (!u.idnumber.empty()) {
		score += 20;
	}
	if (!u.address.empty()) {
		score += 10;
	}

	if (u.yearOfBirth != 0) {
		score += 10;
	}
	return score;
}