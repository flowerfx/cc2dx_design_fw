//
//  Platform.cpp
//  iwin_ios
//
//  Hiện thực các tính năng call qua các platform.
//
//
//This class for Android platform
#include "Platform.h"
#include "Screens/ScreenManager.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
#include "WindowsPhone\WPPlatform.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID 
#include "hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.h"
#include "Platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "ios/Platform_ios.h"
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
using namespace Platform;
using namespace WPPlatform;
#endif

namespace Platform
{

	bool GooglePlayServices::isSignedIn()
	{
		return true;
	}
	void GooglePlayServices::signIn() {}
	void GooglePlayServices::signOut() {}
	void GooglePlayServices::submitScore(const char *leaderboardID, long score) {}
	void GooglePlayServices::unlockAchievement(const char *achievementID) {}
	void GooglePlayServices::incrementAchievement(const char *achievementID, int numSteps) {}
	void GooglePlayServices::showAchievements() {}
	void GooglePlayServices::showLeaderboards() {}
	void GooglePlayServices::showLeaderboard(const char *leaderboardID) {}


	void GameCenter::signIn() {}
	void GameCenter::showLeaderboard() {}
	void GameCenter::showAchievements() {}
	void GameCenter::submitScore(int scoreNumber, cocos2d::__String leaderboardID) {}
	void GameCenter::unlockAchievement(cocos2d::__String achievementID) {}
	void GameCenter::unlockAchievement(cocos2d::__String achievementID, float percent) {}
	void GameCenter::resetPlayerAchievements() {}


	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int repeatInterval, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int repeatInterval, int notificationTag) {}
	void Notifications::unscheduleAllLocalNotifications() {}
	void Notifications::unscheduleLocalNotification(int notificationTag) {}


	void getCurrentLocation(std::function<void(const double, const double)> _resultCallback)
	{
	}

	void makeCall(std::string phoneNumber, std::string showName)
	{
#if defined CC_WINDOWS_PHONE_8_1
		WPPlatform::WPHelper::makeCall(phoneNumber, showName);
#elif defined CC_PLATFORM_IOS

#endif
	}
	void showWeb(const std::string url) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID	
		nativeShowWeb(url, "iWinOnline");
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowWebView(url);
#endif
	}
	void showSMSComposer(std::string content, std::string toPhoneNumber)
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeShowSmsComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowSMSComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::sendSMS(content, toPhoneNumber);
#endif
	}

    void sendSMS(std::string toPhoneNumber, std::string content)
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeSendSMS(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowSMSComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::sendSMS(content, toPhoneNumber);
#endif
	}

	void makeCall(std::string phone_number)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeMakeCall(phone_number);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeMakeCall(phone_number);
#endif
	}

	void vibrate(int milis)
	{
		Device::vibrate(milis * 1.0f / 1000);
	}

	std::string getVersion()
	{
		return Application::getInstance()->getVersion();
	}

	std::string getPackageName()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		return getPackageNameJNI();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        return Platform::ios::nativeGetBundleId();
#else
		return "";
#endif

	}

	std::string getAppName()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		return nativeGetAppName();
#else
		return "";
#endif
	}

	void handleSMSSucceed(std::string phone_number, std::string sms_body)
	{
		ScrMgr->OnShowDialog("Thong bao", StringUtils::format("Gui Sms thanh cong %s, %s", phone_number.c_str(), sms_body.c_str()), TYPE_DIALOG::DIALOG_ONE_BUTTON);
	}

	void handleSMSFail()
	{
		ScrMgr->OnShowDialog("Thong bao", "Gui Sms that bai", TYPE_DIALOG::DIALOG_ONE_BUTTON);
	}

}