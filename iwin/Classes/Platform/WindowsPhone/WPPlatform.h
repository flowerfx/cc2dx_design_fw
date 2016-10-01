#ifndef __WPPLATFORM_H__
#define __WPPLATFORM_H__

#include "cocos2d.h"
//using namespace Windows;
USING_NS_CC;
//
namespace WPPlatform
{
#if defined WP8 || defined OS_W8 || defined OS_W10
	class WPHelper
	{
	public:
		//static System::Action^ actionShowWebView;
		static std::function<void(std::string, std::function<void(void*,std::string)>)> actionShowWebView;
		
		static void sendSMS(std::string content, std::string toNumber);
		static void makeCall(std::string _displayName, std::string _phoneNumber);
		static void vibrate(int milisecond);

		static std::wstring stows(std::string s);
		static void showWebView(std::string url,  std::function<void(void*,std::string)> callback);
		static void loginFacebook(std::function<void(void*, std::string)> callback);
	private:
		static void getAssesToken(std::string webAuthResultResponseData, std::string& accessToken, std::string& expiresIn);
	};
#endif
}
#endif