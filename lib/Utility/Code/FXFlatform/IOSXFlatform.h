//
//  IOSXFlatform.h
//  JumpingBunny_IOS
//
//  Created by qchien on 5/2/15.
//
//

#ifndef _IOS_IOSXFlatform_h
#define _IOS_IOSXFlatform_h
#include "wchar.h"
#include "IAP/IAPManager.h"

namespace Utility
{
    namespace XFlatform
    {
        bool IOS_IsAtLogoState();
        bool IOS_IsHaveInternetConnection();
        void IOS_OnRateTheGame(RKString id_game);
        
        void IOS_FB_GetProfile();
    void IOS_FB_OpenLoginPage();
    int IOS_FB_SharePageString(std::wstring str, RKString url);
    const char*  IOS_FB_GetUserScore();
    void IOS_FB_RequestUserScore();
    void IOS_FB_UpdateUserScore(unsigned int i);
    const wchar_t* IOS_FB_GetUserProfile(std::string key);
    const wchar_t* IOS_FB_GetProfileImgURLFromID(std::wstring id);
    bool IOS_FB_IsSharePanelAppear();
    void IOS_FB_CloseSharePanel();
    
    void IOS_PopUp_SetClose(bool b);
    bool IOS_PopUp_GetClose();
#if USE_IAP
    IAP::STATE_IAP_LOAD IOS_IAP_GetStatePurchaseItem();
    IAP::STATE_IAP_LOAD IOS_IAP_GetStateLoadItem();
    void IOS_IAP_SetStateLoadItem(IAP::STATE_IAP_LOAD s);
    void IOS_IAP_SetStatePurchaseItem(IAP::STATE_IAP_LOAD s);

    void IOS_IAP_RequestListItem();
    void IOS_IAP_PurchaseItemWithID(std::string id);
    void IOS_IAP_DoFulfillmentItemId(std::string id);
    void IOS_IAP_InitStoreItem(std::string id, std::string key);
    void IOS_IAP_CheckRestoreIAP();
    bool IOS_IAP_IsAdsRemove();
    const wchar_t* IOS_GetPriceValue();
#endif
        
    void iOS_Ads_InitBanner();
    void iOS_Ads_InitVideo();
    bool iOS_Ads_ShowVideo();
    void IOS_Ads_ShowBanner();
    void IOS_Ads_HideBanner();
        
    int  IOS_Ads_GetVideoState();
    void IOS_Ads_ResetVideoState();
        
	std::string IOS_GetUDID();
        
    void iOS_SendEmail(std::string email_to,std::string title, std::string content);
    }
}
#endif
