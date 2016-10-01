

#ifndef __ADMOB_IOS_WRAPPER_H__
#define __ADMOB_IOS_WRAPPER_H__

#include "DisplayView.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
#if defined USE_SOCIAL_ADS
#import "Admob_ISO_Helper.h"
#import "../../AdColony/iOS/AdColony_iOS_Controller.h"

USING_NS_CC;
using namespace RKUtils;

@interface Admob_IOS_Wrapper : NSObject
{
    Admob_IOS_Helper * _banner_view;
    AdColonyHelper * _videos_view;
    
    UIButton * close_ads_btn;
    UIButton * close_video_ads_btn;
}

+(Admob_IOS_Wrapper*) defaultHelper;

-(void) init:(NSString *) id_admob_app  withCloseBtnImg:(NSString*)name_image;
-(void) init:(NSString *) id_app        andZoneId:(NSString *) id_zone_app;
-(void) showBanner;
-(bool) showVideo;
-(int)  GetStateVideo;
-(void) ResetVideoState;

-(void) HideBanner;
-(void) btn_ads_tab;
-(void) setVisibleBtnCloseAds:(BOOL)visible;

-(void) btn_video_ads_tab;
-(void) setVisibleBtnCloseVideoAds:(BOOL)visible;
-(BOOL) is_video_current_play;

@end
#else
USING_NS_CC;
using namespace RKUtils;
#endif
namespace Utility
{
    namespace Social
    {
        namespace Ads
        {
            void InitBanner(RKString admob_id ,RKString name_image_close_btn);
            void InitVideo(RKString app_id,RKString app_zone_id );
            
            void ShowBanner();
            
            bool ShowVideo();
            
            void HideBanner();
            
            void SetVisibleButonAds(bool visible);
            
            int GetVideoState();
            
            void ResetVideoState();
            
        }

    }
}



#endif //__ADMOB_IOS_WRAPPER_H__
