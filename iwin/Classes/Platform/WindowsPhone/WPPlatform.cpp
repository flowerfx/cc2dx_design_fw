#include "WPPlatform.h"
#include "RKString_Code\RKString.h"
#define URL_FACEBOOK_LOGIN "https://www.facebook.com/dialog/oauth"
#define URL_FACEBOOK_DERECT "https://www.facebook.com/connect/login_success.html"
using namespace RKUtils;
namespace WPPlatform
{

	std::function<void(std::string,std::function<void(void*,std::string)>)> WPHelper::actionShowWebView = nullptr;

	void WPHelper::sendSMS(std::string content, std::string toNumber)
	{
#if defined WP8
		Windows::ApplicationModel::Chat::ChatMessage^ sms = ref  new Windows::ApplicationModel::Chat::ChatMessage();
		sms->Body = ref new Platform::String(stows(content).c_str());
		sms->Recipients->Append(ref new Platform::String(stows(toNumber).c_str()));
		Windows::ApplicationModel::Chat::ChatMessageManager::ShowComposeSmsMessageAsync(sms);
#endif
	}
	void WPHelper::makeCall(std::string _displayName, std::string _phoneNumber)
	{
#if defined WP8
		Platform::String^ displayName = ref new  Platform::String(stows(_displayName).c_str());
		Platform::String^ phoneNumber = ref new Platform::String(stows(_phoneNumber).c_str());
		Windows::ApplicationModel::Calls::PhoneCallManager::ShowPhoneCallUI(phoneNumber, displayName);
#endif
	}
	void WPHelper::vibrate(int milisecond)
	{

	}

	std::wstring WPHelper::stows(std::string s)
	{
		std::wstring ws;
		ws.assign(s.begin(), s.end());
		return ws;
	}
	void WPHelper::showWebView(std::string url,std::function<void(void*,std::string)> callback)
	{
		if (actionShowWebView)
		{
			actionShowWebView(url, callback);
		}
	}

	void WPHelper::loginFacebook(std::function<void(void*, std::string)> callback)
	{
		std::string urlLogin = StringUtils::format("%s?client_id=%s&redirect_uri=%s&response_type=token&display=popup", URL_FACEBOOK_LOGIN,"315837181868526", URL_FACEBOOK_DERECT );
		showWebView(urlLogin, [](void* sender, std::string url) {
			auto str = RKString(url);
			bool isClose = str.StartsWith(RKString(URL_FACEBOOK_DERECT));
			if (isClose)
			{
				std::string accessToken;
				std::string expiresIn;
				getAssesToken(url, accessToken,  expiresIn);
				string sss = "";
			}
		});
	}

	void WPHelper::getAssesToken(std::string webAuthResultResponseData, std::string& accessToken, std::string& expiresIn)
	{
		RKString strData = RKString(webAuthResultResponseData);
		int index = webAuthResultResponseData.find("access_token", 0);
		RKString responseData = strData.Substring(index, strData.Length() - index);
		RKList<RKString> keyValPairs = responseData.Split("&");
		accessToken = "";
		expiresIn = "";
		for (int i = 0; i < keyValPairs.Size(); i++)
		{
			RKList<RKString> splits = keyValPairs[i].Split("=");
			if(splits.GetAt(0) == "access_token")
			{
				accessToken = splits.GetAt(1).GetString();
			}
			else if (splits.GetAt(0)== "expires_in")
			{
				expiresIn = splits.GetAt(1).GetString();
			}
			
		}
	}

}