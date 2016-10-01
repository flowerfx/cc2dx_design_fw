#include "BannerInvite.h"
#include "cocos2d.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/LayerEntity.h"
#include "UI/AdvanceUI/wScrollView.h"
#include "UI/AdvanceUI/wText.h"
#include "Network/Global/Globalservice.h"

using namespace Utility;
using namespace Utility::UI_Widget;
using namespace cocos2d::ui;

BannerInvite::BannerInvite()
{

}

BannerInvite::~BannerInvite()
{
}


void BannerInvite::init(WidgetEntity* entity)
{
	p_panel_invite = entity;
	_isShowing = false;
}

void BannerInvite::setData(int gameId, int roomId, int boardId, std::string password, std::string message, int level, int idInvite, std::string username, int idAvatar)
{
	static_cast<TextWidget*>(p_panel_invite->GetWidgetChildByName(".lbl_invite"))->SetText(message);
	_gameId = gameId;
	_roomId = roomId;
	_boardId = boardId;
	_password = password;
	_level = level;
	_idInvite = idInvite;
	_username = username;
	_idAvatar = idAvatar;
}

void BannerInvite::show()
{
	Vec2 game_size = GetGameSize();
	p_panel_invite->GetResource()->stopAllActions();
	p_panel_invite->GetResource()->setPosition(Vec2(.0f, game_size.y));
	Sequence* sequence = Sequence::create(MoveTo::create(.5f, Vec2(.0f, game_size.y - p_panel_invite->GetSize().y)), DelayTime::create(2.0f), CallFunc::create(CC_CALLBACK_0(BannerInvite::hide, this)), nullptr);
	p_panel_invite->GetResource()->runAction(sequence);
	_isShowing = true;
}

void BannerInvite::hide()
{
	Vec2 game_size = GetGameSize();
	p_panel_invite->GetResource()->stopAllActions();
	p_panel_invite->GetResource()->runAction(MoveTo::create(.5f, Vec2(.0f, game_size.y)));
	_isShowing = false;
}

void BannerInvite::handleAgreeBtn()
{
	GlobalService::joinBoard(_roomId, _boardId, _password);
}