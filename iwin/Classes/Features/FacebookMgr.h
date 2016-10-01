#ifndef __FACEBOOK_MGR_H__
#define __FACEBOOK_MGR_H__

#include <string>


#ifdef SDKBOX_ENABLED
#include "PluginFacebook/PluginFacebook.h"


class FacebookMgr :public sdkbox::FacebookListener
#else
class FacebookMgr
#endif // 
{
private:
	static FacebookMgr*			s_instance;
public:
	static FacebookMgr* getInstance();
	void init();
	void login();
	bool isLoggedIn();
	std::string getAccessToken();
#ifdef SDKBOX_ENABLED
	void onLogin(bool isLogin, const std::string& msg);
	void onSharedSuccess(const std::string& message);
	void onSharedFailed(const std::string& message);
	void onSharedCancel();
	void onAPI(const std::string& key, const std::string& jsonData);
	void onPermission(bool isLogin, const std::string& msg);
	void onFetchFriends(bool ok, const std::string& msg);
	void onRequestInvitableFriends(const sdkbox::FBInvitableFriendsInfo& friends);
	void onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg);
	void onInviteFriendsResult(bool result, const std::string& msg);
	void onGetUserInfo(const sdkbox::FBGraphUser& userInfo);
#endif
};

#define GetFacebookMgr() FacebookMgr::getInstance()

#endif //__FACEBOOK_MGR_H__