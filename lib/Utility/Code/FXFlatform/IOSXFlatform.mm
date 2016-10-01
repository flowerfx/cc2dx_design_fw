//
//  IOSXFlatform.m
//  JumpingBunny_IOS
//
//  Created by qchien on 5/2/15.
//
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "../IAP/IOS/IAP_IOS_Wrapper.h"
#import "../Social/Facebook/IOS/FB_IOS_Wrapper.h"
#import "../Social/Admob/IOS/Admob_IOS_Wrapper.h"
#import "../Social/Email/iOS/Email_IOS_Helper.h"
#import "../Social/Reachability.h"

#include "IOSXFlatform.h"
#include "string.h"
namespace Utility
{
    namespace XFlatform
    {
        bool IOS_IsHaveInternetConnection()
        {
            if ([[Reachability reachabilityForInternetConnection]currentReachabilityStatus]==NotReachable)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        
        
		std::string IOS_GetUDID()
		{
			NSString* uniqueIdentifier = [[[UIDevice currentDevice] identifierForVendor] UUIDString];	
			std::string devide_udid = [uniqueIdentifier UTF8String];
            return devide_udid;
		}
#if USE_IAP
        IAP::STATE_IAP_LOAD IOS_IAP_GetStateLoadItem()
        {
            return IAP::GetStateLoadListItem();
        }
        
        void IOS_IAP_SetStateLoadItem(IAP::STATE_IAP_LOAD  state)
        {
            IAP::SetStateLoadListItem(state);
        }
        
        IAP::STATE_IAP_LOAD IOS_IAP_GetStatePurchaseItem()
        {
            return IAP::GetStatePurchaseItem();
        }
        
        void IOS_IAP_SetStatePurchaseItem(IAP::STATE_IAP_LOAD s)
        {
            IAP::SetStatePurchaseItem(s);
        }
        
         void IOS_IAP_CheckRestoreIAP()
        {
            IAP::RestorePurchase();
        }
#endif        
        void IOS_FB_OpenLoginPage()
        {
            Social::Facebook::LoginFacebook();
        }
        
        void IOS_FB_GetProfile()
        {
            Social::Facebook::GetFrofileFBWithAccessToken();
        }
        
        const wchar_t* IOS_FB_GetUserProfile(std::string key)
        {
            if(key == "id")
            {
                return Social::Facebook::GetIDFacebook().ToUnicode();
            }
            else if (key == "name")
            {
                return Social::Facebook::GetNameFacebook().ToUnicode();
            }
        }
        
        
        int IOS_FB_SharePageString(std::wstring str,RKString url)
        {
            return Social::Facebook::PostMessageToFacebook(RKString(str) , url);
        }
        
        void iOS_Ads_InitBanner()
        {
            //this is overwritten method, each project will use diffenrenc ...
            RKString id_admob = "ca-app-pub-3411299046701283/1127556256";
            RKString name_image = "close-button.png";
            Social::Ads::InitBanner(id_admob, name_image);
        }
        
        void iOS_Ads_InitVideo()
        {
            RKString id_app = "appe5d1ddee83f34954af";
            RKString id_zone = "vz908f23a0cc6a49e6b1";
            Social::Ads::InitVideo(id_app, id_zone);
        }
        bool iOS_Ads_ShowVideo()
        {
            return Social::Ads::ShowVideo();
        }
    
        void IOS_Ads_ShowBanner()
        {
            Social::Ads::ShowBanner();
        }
        
        void IOS_Ads_HideBanner()
        {
            Social::Ads::HideBanner();
        }
        
        int  IOS_Ads_GetVideoState()
        {
            return Social::Ads::GetVideoState();
        }
        
        void IOS_Ads_ResetVideoState()
        {
            Social::Ads::ResetVideoState();
        }
    
        void iOS_SendEmail(std::string email_to,std::string title, std::string content)
        {
            NSString * email_to_ios = [NSString stringWithUTF8String:email_to.c_str()];
            NSString * title_ios = [NSString stringWithUTF8String:title.c_str()];
            NSString * content_ios = [NSString stringWithUTF8String:content.c_str()];
            
            [[Email_iOS_Helper defaultHelper] ShowEmail: email_to_ios withTitle:title_ios andContent:content_ios];
        }
        
        void IOS_OnRateTheGame(RKString id_game)
        {
            RKString full_url = RKString("itms-apps://itunes.apple.com/app/id") + id_game;
            
            NSString * url_game = [NSString stringWithUTF8String:full_url.GetString()];
            
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithFormat:url_game]]];
        }
    }
}