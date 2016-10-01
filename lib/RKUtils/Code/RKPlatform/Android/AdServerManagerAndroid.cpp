#include "AdServerManager.h"		// Declare the class AdServerAndroid
#include "../../Game.h"
#include "../../Platform/Platform.h"
#include "PlatformAndroid.h"
#include "../../RewardReceiver/AdServerManager.h"
#include "config_Android.h"
#include "../../../prj/android/GameSpecific/jni/jni_base.h"
#include <cassert>
//////////////////////////////////////////////////////////////////////////////////////////
//						AdServerManager on android
//////////////////////////////////////////////////////////////////////////////////////////
namespace CasualCore
{    
#if defined(GAME_ANDROID)
    //////////////////////////////////////////////////////////////////////////////////////////
    // Implementation of AdServer on Android platform
    bool AdServerManager::IsBannerVisible()
    {
        LOG("XXXXXX NOT implemented XXXXXX"); // Not implemented
        return false;
    }
    
    void AdServerManager::ShowBanner(const int x, const int y, const int orientation)
    {
#if USE_ADS_SERVER
		LOG("********* AdServerManagerAndroid.cpp AdServerManager::ShowBanner");
		int language = Game::GetInstance()->GetLanguage();
		nativeShowAds(language);
#endif // USE_ADS_SERVER
    }
        
    void AdServerManager::RemoveBanner()
    {
#if USE_ADS_SERVER
		nativeHideAds();
#endif // USE_ADS_SERVER
    }
        
    void AdServerManager::CheckFreeCashReady()
    {
#if USE_ADS_SERVER
		int languageindex = Game::GetInstance()->GetLanguage();
    nativeLoadFreeCash(languageindex);
    nativeCheckFreeCashReady();

#endif // USE_ADS_SERVER
    }

    void AdServerManager::ShowFreeCash()
    {
#if USE_ADS_SERVER
    nativeShowFreeCash();
#endif // USE_ADS_SERVER
    }
	
    void AdServerManager::LoadIGPView()
    {
      // Only used on iOS
    }
    
    extern "C" void nativelaunchIGP(int lang);

    void AdServerManager::ShowIGPView(CallbackAdsBasic igpExitCallback, void* caller)
    {
#if USE_IGP_ACTIVITY || USE_IGP_FREEMIUM
      int language = CasualCore::Game::GetInstance()->GetLanguage();
      nativelaunchIGP(language);
      LOG( "[IGP] show IGP on android AdServerManager::ShowIGPView()" );
#endif // USE_IGP_ACTIVITY || USE_IGP_FREEMIUM
  	}
#endif // GAME_ANDROID
};