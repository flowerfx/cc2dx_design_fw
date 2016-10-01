#include "../DisplayView.h"
#ifdef WP8
//#include "Direct3DInterop.h"
#endif
#include "XFlatform.h"
#ifdef GAME_IOS
#include "../IAP/IOS/IAP_IOS_Wrapper.h"
#include "IOSXFlatform.h"
#endif
#if defined WP8 
extern Platform::String ^stops(std::string s);
extern std::string pstos(Platform::String^ ps);
#endif


#if defined OS_W8 || defined OS_W10
//#include "../../XFlatformCorePackage/WCP/include/WinShared.h"
#include "../IAP/Win8.1/WinRT_IAP.h"
#endif
namespace Utility
{
	namespace XFlatform
	{

#if defined WIN32 && defined USE_IAP//hack iap
		IAP::STATE_IAP_LOAD state_iap = IAP::STATE_IAP_LOAD::IAP_SUCCEED;
		IAP::STATE_IAP_LOAD state_purchase_iap = IAP::STATE_IAP_LOAD::IAP_NONE;
#endif

		bool IsAtLogoState()
		{
#ifdef WP8
			//return cocos2d::Direct3DInterop::Get_LogoState();
			return false;
#else
			return false;
#endif
		}

		bool IsHaveInternetConnection()
		{
#if defined WP8
			return false;
#elif defined GAME_IOS
            return IOS_IsHaveInternetConnection();
#elif defined OS_W8 || defined OS_W10
			//WCPToolkit::Connectivity::InternetConnectionInfo connect_type = WCPToolkit::Connectivity::GetInternetConnectionInfo();
			//return  (connect_type.connectionType != WCPToolkit::Connectivity::NoConnection && connect_type.connectionType != WCPToolkit::Connectivity::UnknownConnection);
			return false;
#else
			return true;
#endif
		}

		void OnRateTheGame(RKString id_game)
		{
#if defined WP8
			//cocos2d::Direct3DInterop::OnRateTheGame();
#elif defined GAME_IOS
			IOS_OnRateTheGame(id_game);
#else
			return ;
#endif
		}

        
        void FB_GetProfile()
        {
#if defined WP8
         //   cocos2d::Direct3DInterop::openFacebookPage();
#elif defined GAME_IOS
            IOS_FB_GetProfile();
#else
            return;
#endif
        }

		void FB_OpenLoginPage()
		{
#if defined WP8
			//cocos2d::Direct3DInterop::openFacebookPage();
#elif defined GAME_IOS
            IOS_FB_OpenLoginPage();
#else
			return;
#endif
		}

		int FB_SharePageString(std::wstring str, RKString url )
		{
#if defined WP8
			//cocos2d::Direct3DInterop::Get_FB_ShareFB(ref new Platform::String(str.data()));
			return 0;
#elif defined GAME_IOS
            return IOS_FB_SharePageString(str , url);
#else
            return 0 ;
#endif
		}

		std::string FB_GetUserScore()
		{
#ifdef WP8
			//std::string score = std::string(CCUnicodeToUtf8(cocos2d::Direct3DInterop::Get_FB_GetScore()->Data()));
			return "0";
#else
			return "0";
#endif
		}

		void FB_RequestUserScore()
		{
#ifdef WP8
			//cocos2d::Direct3DInterop::Request_FB_Score();
#else
			return;
#endif
		}

		void FB_UpdateUserScore(unsigned int i)
		{
#ifdef WP8
			//cocos2d::Direct3DInterop::Get_FB_UpdateScore(i);
#else
			return;
#endif
		}

		std::wstring FB_GetUserProfile(std::string key)
		{
			std::wstring val = L"";
			if (key == "name")
			{
#if defined WP8
				//val = cocos2d::Direct3DInterop::Get_FB_Name()->Data();
#elif defined GAME_IOS
                return IOS_FB_GetUserProfile(key);
#else
				val = L"";
#endif
			}
			else if (key == "id")
			{
#if defined WP8
				//val = cocos2d::Direct3DInterop::Get_FB_ID()->Data();
#elif defined GAME_IOS
                return IOS_FB_GetUserProfile(key);
#else
				val = L"";
#endif
			}
			else if (key == "token")
			{
#if defined WP8
				//val = cocos2d::Direct3DInterop::Get_FB_Token()->Data();
#elif defined GAME_IOS
                return IOS_FB_GetUserProfile(key);
#else
				val = L"";
#endif
			}
			else if (key == "imgUrl")
			{
#if defined WP8
				//val = cocos2d::Direct3DInterop::Get_FB_ProfileImgURL()->Data();
#elif defined GAME_IOS
                return IOS_FB_GetUserProfile(key);
#else
				val = L"";
#endif
			}
			return val;
		}

		std::wstring FB_GetProfileImgURLFromID(std::wstring id)
		{
			std::wstring val = L"";
#ifdef WP8
			//val = cocos2d::Direct3DInterop::Get_FB_ProfileImgURLFromID(ref new Platform::String(id.data()))->Data();
#else
			val = L"";
#endif
			return val;
		}

		bool FB_IsSharePanelAppear()
		{
#ifdef WP8
			//return cocos2d::Direct3DInterop::Get_FB_SharePanelAppear();
			return false;
#else
			return false;
#endif
		}

		void FB_CloseSharePanel()
		{
#ifdef WP8
			//return cocos2d::Direct3DInterop::FB_CloseSharePanel();
#else
			return ;
#endif
		}

		void PopUp_SetClose(bool b)
		{
#ifdef WP8
		//	cocos2d::Direct3DInterop::PopUp_SetClose(b);
#else
			return;
#endif
		}

		bool PopUp_GetClose()
		{
#ifdef WP8
		//	return cocos2d::Direct3DInterop::PopUp_GetClose();
			return false;
#else
			return true;
#endif
		}
		/*
	#ifdef WP8
	#else
	#endif
	*/
#if USE_IAP
		IAP::STATE_IAP_LOAD IAP_GetStatePurchaseItem()
		{
#if defined WP8
			int res = cocos2d::Direct3DInterop::IAP_GetStatePurchaseItem();
			return (STATE_IAP_LOAD)res;
#elif defined GAME_IOS
            return IOS_IAP_GetStatePurchaseItem();
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->GetStatePurchaseItem();
#elif defined WIN32
			return state_purchase_iap;
#else
			return IAP::STATE_IAP_LOAD::IAP_SUCCEED;
#endif
		}

		IAP::STATE_IAP_LOAD IAP_GetStateLoadItem()
		{
#if defined WP8
			int res = cocos2d::Direct3DInterop::IAP_GetStateLoadItem();
			return (STATE_IAP_LOAD)res;
#elif defined GAME_IOS
            return IOS_IAP_GetStateLoadItem();
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->GetStateLoadListItem();
#elif defined WIN32
			return state_iap;
#else
			return IAP::STATE_IAP_LOAD::IAP_SUCCEED;
#endif
		}

		void IAP_SetStateLoadItem(IAP::STATE_IAP_LOAD s)
		{
#if defined WP8
			cocos2d::Direct3DInterop::IAP_SetStateLoadItem((int)s);
#elif defined GAME_IOS
            IOS_IAP_SetStateLoadItem(s);
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->SetStateLoadListItem(s);
#elif defined WIN32
			state_iap = s;
#else
			return;
#endif
		}

		void IAP_SetStatePurchaseItem(IAP::STATE_IAP_LOAD s)
		{
#ifdef WP8
			cocos2d::Direct3DInterop::IAP_SetStatePurchaseItem((int)s);
#elif defined GAME_IOS
            IOS_IAP_SetStatePurchaseItem(s);
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->SetStatePurchaseItem(s);
#elif defined WIN32
			state_purchase_iap = s;
#else
			return;
#endif
		}
        
		void IAP_RequestListItem()
		{
#if defined WP8
			cocos2d::Direct3DInterop::IAP_RequestListItem();
#elif defined GAME_IOS
            IAP::IOS_LoadTheStore();
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->RequestListItem();
#else
			return;
#endif
		}

		void IAP_PurchaseItemWithID(std::string id)
		{
#if defined WP8
			Platform::String ^ itemId = stops(id);
			cocos2d::Direct3DInterop::IAP_PurchaseItemWithID(itemId);
#elif defined GAME_IOS
            IAP::IOS_PurchaseItem(id);
#elif defined OS_W8 || defined OS_W10
			return IAP::WinRT_IAP::DefaultHelper()->PurchaseItem(id);
#elif defined WIN32
			state_purchase_iap = IAP::STATE_IAP_LOAD::IAP_SUCCEED;
#else
            return;
#endif
		}

		void IAP_DoFulfillmentItemId(std::string id)
		{
#ifdef WP8
			Platform::String ^ keyId = stops(id);
			cocos2d::Direct3DInterop::IAP_DoFulfillmentItemId(keyId);
#else
			return;
#endif
		}

		void IAP_InitStoreItem(std::string id, std::string key)
		{
#ifdef WP8
			Platform::String ^ pid = stops(id);
			Platform::String ^ pkey = stops(key);
			cocos2d::Direct3DInterop::IAP_InitStoreItem(pid, pkey);
#else
			return;
#endif
		}

		std::string IAP_CheckFulfillmentIfNotFulfill(std::string keygain)
		{
			std::string returnkey = "";
#if defined WP8
			Platform::String^	keyGain = "";
			Platform::String^	keyGain1 = "";
			keyGain1 = cocos2d::Direct3DInterop::IAP_CheckFulfillmentIfNotFulfill(keyGain);
			returnkey = pstos(keyGain1);
#elif defined GAME_IOS
            IOS_IAP_CheckRestoreIAP();
#else
			returnkey = "";
#endif
			return returnkey;
		}
		bool IAP_IsAdsRemove()
		{
#if defined WP8
			bool b = cocos2d::Direct3DInterop::IAP_IsRemoveAds();
			return b;
#elif defined GAME_IOS
            return false;
#else
			return true;
#endif
		}
#endif
		std::wstring GetPriceValue()
		{
#ifdef WP8
			//std::wstring w_str = cocos2d::Direct3DInterop::IAP_GetPriceValue()->Data();
			return L"";
#else
			return L"";
#endif
		}

        void Ads_InitBanner()
        {
#if defined GAME_IOS
            iOS_Ads_InitBanner();
#else
            return;
#endif
        }
        
		void Ads_ShowBanner()
		{
#if defined WP8
			//cocos2d::Direct3DInterop::Ads_ShowBanner();
#elif defined GAME_IOS
            IOS_Ads_ShowBanner();
#else
			return;
#endif
		}

		void Ads_HideBanner()
		{
#if defined WP8
			//cocos2d::Direct3DInterop::Ads_HideBanner();
#elif defined GAME_IOS
            IOS_Ads_HideBanner();
#else
			return;
#endif
		}
        
        void Ads_InitVideo()
        {
#ifdef GAME_IOS
            iOS_Ads_InitVideo();
#endif
        }
        
        bool Ads_ShowVideo()
        {
#ifdef GAME_IOS
            return iOS_Ads_ShowVideo();
#endif
			return false;
        }
		
        int Ads_GetVideoState()
        {
#ifdef GAME_IOS
            return IOS_Ads_GetVideoState();
#endif
            return 0;
        }
        
        void Ads_ResetVideoState()
        {
#ifdef GAME_IOS
            IOS_Ads_ResetVideoState();
#endif
        }
        
		std::string GetUDID()
		{
#if defined WP8
			//auto str = std::string(CCUnicodeToUtf8(Direct3DInterop::Get_DeviceID()->Data()));
			return "";
#elif defined GAME_IOS
			return IOS_GetUDID();
#elif defined OS_W8 || defined OS_W10
			//std::string str = WCPToolkit::Identifiers::GetHardwareID();
			return "";
#else
			return "";
#endif		
		}
        
        void SendEmail(std::string email_to,std::string title, std::string content)
        {
#if defined GAME_IOS
            iOS_SendEmail(email_to, title, content);
#else
            return;
#endif
        }
		
	}

}