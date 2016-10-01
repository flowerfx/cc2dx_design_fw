#ifndef _XFLATFORM_H_
#define _XFLATFORM_H_
#include "RKString_Code/RKString.h"
#ifdef USE_IAP
#include "IAP/IAPManager.h"
#endif
using namespace RKUtils;
namespace Utility
{
	namespace XFlatform
	{
		bool IsAtLogoState();
		bool IsHaveInternetConnection();
		void OnRateTheGame(RKString id_game);

        void FB_GetProfile();
		void FB_OpenLoginPage();
		int FB_SharePageString(std::wstring str, RKString url );
		std::string FB_GetUserScore();
		void FB_RequestUserScore();
		void FB_UpdateUserScore(unsigned int i);
		std::wstring FB_GetUserProfile(std::string key);
		std::wstring FB_GetProfileImgURLFromID(std::wstring id);
        
		bool FB_IsSharePanelAppear();
		void FB_CloseSharePanel();

		void PopUp_SetClose(bool b);
		bool PopUp_GetClose();
#if USE_IAP
		IAP::STATE_IAP_LOAD IAP_GetStatePurchaseItem();
		IAP::STATE_IAP_LOAD IAP_GetStateLoadItem();
		void IAP_SetStateLoadItem(IAP::STATE_IAP_LOAD s);
		void IAP_SetStatePurchaseItem(IAP::STATE_IAP_LOAD s);

		void IAP_RequestListItem();
		void IAP_PurchaseItemWithID(std::string id);
		void IAP_DoFulfillmentItemId(std::string id);
		void IAP_InitStoreItem(std::string id, std::string key);
		std::string IAP_CheckFulfillmentIfNotFulfill(std::string keygain);
		bool IAP_IsAdsRemove();
		std::wstring GetPriceValue();
 #endif       
        void Ads_InitBanner();
		void Ads_ShowBanner();
		void Ads_HideBanner();
        
        void Ads_InitVideo();
        bool Ads_ShowVideo();
        int  Ads_GetVideoState();
        void Ads_ResetVideoState();
		
		std::string GetUDID();
        
        void SendEmail(std::string email_to,std::string title, std::string content);
	}
}

#endif //_XFLATFORM_H_