#include "Player.h"
#include "Utils.h"


Player::Player()
{
	userProfile = new User();
	IDDB = -1;

	name = "";
	group = 0;
	showName = "";
	exp = 0;
	money = 0;
	ruby = 0;
	avatar = 0;
	isReady = false;
	giftID = 0;
	roomID = 0;
	boardID = 0;
	gameID = 0;
	hasBoardPassword = false;
	finishPosition = false;

	shortMoneyString = "0";
	longMoneyString = "0";
	onlineInfo = "";
	lv = 1;
	percent = 0;
	quotaTime = 0;
	elo = 0;
	isQuickPlay = false;
	isActived = false;
}

Player::Player(Player * p)
{
	CopyValue(p);
}

void Player::CopyValue(Player * p)
{
	userProfile = new User(p->userProfile);
	IDDB = p->IDDB;
	name = p->name;
	group = p->group;
	showName = p->showName;
	exp = p->exp;
	money = p->money;
	ruby = p->ruby;
	avatar = p->avatar;
	isReady = p->isReady;
	giftID = p->isReady;
	roomID = p->roomID;
	boardID = p->boardID;
	gameID = p->gameID;
	hasBoardPassword = p->hasBoardPassword;
	finishPosition = p->hasBoardPassword;

	shortMoneyString = p->shortMoneyString;
	longMoneyString = p->longMoneyString;
	onlineInfo = p->onlineInfo;
	lv = p->lv;
	percent = p->percent;
	quotaTime = p->quotaTime;
	elo = p->elo;
	isQuickPlay = p->isQuickPlay;
	isActived = p->isActived;
}

Player::~Player()
{
	if (userProfile)
	{
		delete userProfile;
		userProfile = nullptr;
	}

}

std::string Player::getLevelDetail() {
	if (percent > 0) {
		return cocos2d::StringUtils::format("%d+%d%", lv, percent);
	}
	else {
		return cocos2d::StringUtils::format("%d", lv);
	}
}

bool Player::equals(Player obj) {
	return IDDB == obj.IDDB;
}

std::string Player::toString() {
	return name;
}

/**
* Set money cho Player
*
* @param amount
*            : long - Tong so tien ma user co, do server tra ve
*/
void Player::setMoney(s64 amount) {
	//Gdx.app.log(TAG, "setMoney: " + amount + " for " + name);
	money = amount;
	if (money <= 0) {
		money = 0;
	}
	std::string moneyFormatted = cocos2d::StringUtils::format("%lld", money);
	//@PDT: TODO
	//shortMoneyString = moneyFormatted + L.gL(202);
	//longMoneyString = moneyFormatted + " " + L.gL(395);
	shortMoneyString = moneyFormatted;
	longMoneyString = moneyFormatted;
}

void Player::setRuby(s64 amount)
{
	ruby = amount;
	if (ruby <= 0)
	{
		ruby = 0;
	}
}

/**
* add money cho Player
*
* @param amount
*            : long - So tien dc cong them
*/
void Player::addMoney(s64 amount) 
{
	setMoney(money + amount);
}

//@PDT: TODO
//@Override
//public TextureRegion getIcon() {
//	return ImageService.getDefaultAvatar();
//}
//
//@Override
//public TextureRegion getSubIcon() {
//	return isReady ? CommonAsset.mainscreen_icon_playing.getRegion()
//		: CommonAsset.mainscreen_icon_wait.getRegion();
//}

std::string Player::getDescription() {
	return Utils::formatMoney_dot_win(money);
}

std::string Player::getSubDescription() {
	return "Level : " + getLevelDetail();
}

std::string Player::getContent() {
	return onlineInfo.empty() ? userProfile != nullptr ? userProfile->status
		: ""
		: onlineInfo;
}

/**
* Kiem tra xem thang user nay da login hay chua.
*
* @return
*/
bool Player::isLoggedIn() {
	return IDDB != -1 && name.length() > 0;
}

// public Label getLabelGift() {
// mLabelGift = (mLabelGift != null) ? mLabelGift : new Label("");
// return mLabelGift;
// }

// public String createOnlineInfo(final byte room, final byte board, final
// byte gameId) {
// String a = "";
// if (gameId != -1) {
// a = IwinGameController.getGameNameOf(gameId) + ", " + LangManager.gL(290)
// + " " + room + ", " + LangManager.gL(22) + " " + board;
// }
// return a;
// }

/**
* tinh toan level
*
* @param exp
* @return []{level,percent}
*/
int* calculateExp(int exp) 
{
	int lv = 1;
	int lastExpRemain = exp;
	while (true) {
		int upLevelRequire = lv * 100;
		lastExpRemain = exp;
		int expRemain = exp - upLevelRequire;
		if (expRemain >= 0) {
			lv++;
			exp = expRemain;
		}
		else {
			break;
		}
	}
	// expRemain = lastExpRemain;
	int* data = new int[2];
	data[0] = lv;
	int expRequire = lv * 100;
	data[1] = lastExpRemain * 100 / expRequire;
	return data;
}

/**
* Set cap do cho Player
*
* @param exp
*            : Int - Cap do
*/
void Player::setExp(int _exp) {
	exp = _exp;
	int* data = calculateExp(_exp);
	lv = data[0];
	percent = data[1];
}

int Player::getPercent() {
	return percent;
}

int Player::getLevel() {
	return lv;
}

long Player::getExp()
{
	return exp;
}

s64 Player::getMoney()
{
	return money;
}

void Player::setName(std::string new_name)
{
	name = new_name;
	if (name.size() >= 8)
	{
		showName = name.substr(0, 7);
		showName = name;
	}
}

void Player::reset()
{

}

void Player::setUserProfile(User* userData)
{
	if (userProfile)
		delete userProfile;
	userProfile = new User(userData);
}