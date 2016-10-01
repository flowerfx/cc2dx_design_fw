#include "FacebookMgr.h"
#include "MainGame.h"
#include "Platform/WindowsPhone/WPPlatform.h"
#ifdef SDKBOX_ENABLED
using namespace sdkbox;
#endif
FacebookMgr* FacebookMgr::s_instance = nullptr;

FacebookMgr* FacebookMgr::getInstance()
{
	if (!s_instance)
	{
		s_instance = new FacebookMgr();
	}
	return s_instance;
}

void FacebookMgr::init()
{
#ifdef SDKBOX_ENABLED
	sdkbox::PluginFacebook::init();
	PluginFacebook::setListener(this);
#endif
}


void FacebookMgr::login()
{
#ifdef SDKBOX_ENABLED
	PluginFacebook::login();
#elif defined WP8 || defined OS_W8 || defined OS_W10
	WPPlatform::WPHelper::loginFacebook([](void*, std::string) {
			
		});
#endif
}

bool FacebookMgr::isLoggedIn()
{
#ifdef SDKBOX_ENABLED
	return PluginFacebook::isLoggedIn();
#else
	return false;
#endif
}


std::string FacebookMgr::getAccessToken()
{
#ifdef SDKBOX_ENABLED
	return PluginFacebook::getAccessToken();
#else
	return "EAAEfQJMIOe4BABW71ohyQvn9ABh4NMYhNjNycwuZABedfZBpyZAMtBUziGZASNpdQDTjoRI5ZB1HJunWWxe749rbT1w8pWQ3xbUZAQ3IJLnNZBdnCnm8eKsQaWU6crGiadu8vdLOS8Gjf68RNPFDrkFsgN9pUe8yrsZD";
#endif
}


#ifdef SDKBOX_ENABLED
void FacebookMgr::onLogin(bool isLogin, const std::string& msg)
{
	if (isLogin)
	{
		GameMgr->Network_loginFacebook(PluginFacebook::getAccessToken());
	}
	else
	{
		CCLOG("Login failed:%s", msg.c_str());
	}
}

void FacebookMgr::onSharedSuccess(const std::string& message)
{

}

void FacebookMgr::onSharedFailed(const std::string& message)
{

}

void FacebookMgr::onSharedCancel()
{

}

void FacebookMgr::onAPI(const std::string& key, const std::string& jsonData)
{

}

void FacebookMgr::onPermission(bool isLogin, const std::string& msg)
{

}

void FacebookMgr::onFetchFriends(bool ok, const std::string& msg)
{

}

void FacebookMgr::onRequestInvitableFriends(const sdkbox::FBInvitableFriendsInfo& friends)
{

}

void FacebookMgr::onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg)
{

}

void FacebookMgr::onInviteFriendsResult(bool result, const std::string& msg)
{

}

void FacebookMgr::onGetUserInfo(const sdkbox::FBGraphUser& userInfo)
{

}

#endif