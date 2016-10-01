#ifndef __USER_H__
#define __USER_H__

#include "Common/Common.h"

class User
{
	
public:

	User(User * user)
	{
		id = user->id;
		nickName = user->nickName;
		avatarID = user->avatarID;
		money = user->money;
		fullname = user->fullname;
		yearOfBirth = user->yearOfBirth;
		dayOfBirth = user->dayOfBirth;
		birthdayStatus = user->birthdayStatus;
		monthOfBirth = user->monthOfBirth;
		gender = user->gender;
		genderStatus = user->genderStatus;
		idnumber = user->idnumber;
		idnumberStatus = user->idnumberStatus;
		address = user->address;
		status = user->status;
		isChangePass = user->isChangePass;
		oldPass = user->oldPass;
		newPass = user->newPass;
		// Array<?> assets;
		phoneNo = user->phoneNo;
		province = user->province;
		percent = user->percent;
		playingTime = user->playingTime;
		email = user->email;

		isSentGift = user->isSentGift;
		benefitElo = user->benefitElo;
		bestElo = user->bestElo;
		eloT = user->eloT;
		avaElo = user->avaElo;

		ruby = user->ruby;
		isEmailValidate = user->isEmailValidate;
		isPhoneValidate = user->isPhoneValidate;
	}

	User()
	{
		id = -1;
		nickName = "";
		avatarID = -1;
		money = 0;
		ruby = 0;
		fullname = "";
		yearOfBirth = 1920;
		dayOfBirth = 1;
		birthdayStatus = true;
		monthOfBirth = 1;
		gender = 0;
		genderStatus = true;
		idnumber = "";
		idnumberStatus = true;
		address = "";
		status = "";
		isChangePass = false;
		oldPass = "";
		newPass = "";
		// Array<?> assets;
		phoneNo = "";
		province = 0;
		percent = 0;
		playingTime = "";
		email = "";

		isSentGift = true;
		benefitElo = "";
		bestElo = "";
		eloT = "";
		avaElo = -1;

		isEmailValidate = false;
		isPhoneValidate = false;
	}

	virtual ~User()
	{
		id = -1;
		nickName = "";
		avatarID = -1;
		money = 0;
		fullname = "";
		yearOfBirth = 1920;
		dayOfBirth = 1;
		birthdayStatus = true;
		monthOfBirth = 1;
		gender = 0;
		genderStatus = true;
		idnumber = "";
		idnumberStatus = true;
		address = "";
		status = "";
		isChangePass = false;
		oldPass = "";
		newPass = "";
		// Array<?> assets;
		phoneNo = "";
		province = 0;
		percent = 0;
		playingTime = "";
		email = "";

		isSentGift = true;
		benefitElo = "";
		bestElo = "";
		eloT = "";
		avaElo = -1;
	}
public:

	int id;
	std::string nickName;
	int avatarID;
	u64 money;
	u64 ruby;
	std::string fullname;
	s16 yearOfBirth;
	ubyte dayOfBirth;
	bool birthdayStatus ;
	ubyte monthOfBirth;
	ubyte gender;
	bool genderStatus;
	std::string idnumber;
	bool idnumberStatus ;
	std::string address ;
	std::string status  ;
	bool isChangePass	;
	std::string oldPass ;
	std::string newPass ;
	// Array<?> assets;
	std::string phoneNo ;
	ubyte province;
	int percent;
	std::string playingTime;
	std::string email ;
	/**
	* Bien nay dung de xac dinh user da duoc tang qua hay chua. Neu roi thi k
	* show thanh slider...
	*/
	bool isSentGift ;
	std::string benefitElo;
	std::string bestElo, eloT;
	int avaElo;

	/**
	* 2 biến dùng để biết được trạng thái verify email va sdt của user
	*/
	bool isEmailValidate;
	bool isPhoneValidate;
public:
	static int scoreUserInfo(User u);
};

#endif //__USER_H__
