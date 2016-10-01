//
//  IAP_IOS_.m
//  CrystalNumberful
//
//  Created by qchien on 12/3/15.
//
//
#import "Admob_IOS_Wrapper.h"
using namespace RKUtils;
#if defined USE_SOCIAL_ADS
#include "../../../FileManager.h"
@implementation Admob_IOS_Wrapper

static Admob_IOS_Wrapper * m_instance_admob = nil;

+(Admob_IOS_Wrapper*) defaultHelper
{
    if(!m_instance_admob)
    {
        m_instance_admob = [Admob_IOS_Wrapper alloc];
    }
    return m_instance_admob;
}

-(void) init:(NSString *) id_app andZoneId:(NSString *) id_zone_app
{
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    _videos_view = [[AdColonyHelper alloc]initWithContentViewController:window.rootViewController withIDApp:id_app andIDZone:id_zone_app];
   // [window setRootViewController:_videos_view];
    
    close_video_ads_btn = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    float width_screen = [[UIScreen mainScreen]bounds].size.width;
    float height_screen = [[UIScreen mainScreen]bounds].size.height;
    
    [close_video_ads_btn setBackgroundImage:[UIImage imageNamed:@"close-button.png"] forState:UIControlStateNormal];
    CGVector size_btn;
    size_btn.dx = 15;
    size_btn.dy = 15;
    
    if(width_screen / height_screen > 0.68) //ipad
    {
        size_btn.dx *= 2;
        size_btn.dy *= 2;
        
        close_video_ads_btn.frame = CGRectMake((width_screen * 10 / 6) -  size_btn.dx, (height_screen / 2 )-  size_btn.dy, size_btn.dx,size_btn.dy);
    }
    else{ //iphone
        
        close_video_ads_btn.frame = CGRectMake((width_screen * 10 / 6) -  size_btn.dx, (height_screen / 2 )-  size_btn.dy, size_btn.dx,size_btn.dy);
    }
    [close_video_ads_btn sizeToFit];
    [close_video_ads_btn addTarget:self action:@selector(btn_video_ads_tab) forControlEvents:UIControlEventTouchUpInside];
    
    close_video_ads_btn.hidden = YES;
    [window addSubview:close_video_ads_btn];
}

-(void)init:(NSString *) id_admob_app withCloseBtnImg:(NSString*)name_image
{
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    _banner_view = [[Admob_IOS_Helper alloc] initWithContentViewController:window.rootViewController   andAppId:id_admob_app];
    
    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _banner_view.view];
    }
   else
    {
        // use this method on ios6
        [window setRootViewController:_banner_view];
    }
    
    close_ads_btn = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    
    float width_screen = [[UIScreen mainScreen]bounds].size.width;
    float height_screen = [[UIScreen mainScreen]bounds].size.height;
    
    [close_ads_btn setBackgroundImage:[UIImage imageNamed:name_image] forState:UIControlStateNormal];
    CGVector size_btn;
    size_btn.dx = 15;
    size_btn.dy = 15;
    
    if(width_screen / height_screen > 0.68) //ipad
    {
        size_btn.dx *= 2;
        size_btn.dy *= 2;
        
        close_ads_btn.frame = CGRectMake(width_screen - 150,height_screen - 100, size_btn.dx,size_btn.dy);
    }
    else{ //iphone
        
        close_ads_btn.frame = CGRectMake((width_screen * 5 / 6 ) - size_btn.dx * 2, (height_screen / 6 ) - size_btn.dx * 2, size_btn.dx,size_btn.dy);
    }
    [close_ads_btn sizeToFit];
    [close_ads_btn addTarget:self action:@selector(btn_ads_tab) forControlEvents:UIControlEventTouchUpInside];
    
    close_ads_btn.hidden = YES;
   [window addSubview:close_ads_btn];

}

-(void)showBanner
{
    [_banner_view showBanner];
}

-(bool)showVideo
{
    return [_videos_view ShowVideo];
}

-(void)HideBanner
{
    [_banner_view hideBanner];
    close_ads_btn.hidden = YES;
    //[close_ads_btn removeFromSuperview];
}

-(void)btn_ads_tab
{
    [self HideBanner];
}

-(void)setVisibleBtnCloseAds:(BOOL)visible
{
    close_ads_btn.hidden = !visible;
}

-(void) btn_video_ads_tab
{
    [_videos_view CloseVideo];
    close_video_ads_btn.hidden = YES;
}

-(void) setVisibleBtnCloseVideoAds:(BOOL)visible
{
    close_video_ads_btn.hidden = !visible;
}

-(int)  GetStateVideo
{
    return [_videos_view GetStateVideo];
}

-(void) ResetVideoState
{
    [_videos_view ResetStateVideo];
}

-(BOOL) is_video_current_play
{
    return [_videos_view IsVideosPlaying];
}
@end
#endif
namespace Utility
{
    namespace Social
    {
        namespace Ads
        {
            
            void InitBanner(RKString admob_id ,RKString name_image_close_btn)
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper] init : [NSString stringWithUTF8String:admob_id.GetString()]
                                         withCloseBtnImg: [NSString stringWithUTF8String:name_image_close_btn.GetString()]];
#endif
            }
            
            void InitVideo(RKString app_id,RKString app_zone_id )
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper] init : [NSString stringWithUTF8String:app_id.GetString()] andZoneId:[NSString stringWithUTF8String:app_zone_id.GetString()]];
#endif
            }
            
            void ShowBanner()
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper]showBanner];
#endif
            }
            
            bool ShowVideo()
            {
#if defined USE_SOCIAL_ADS
                return [[Admob_IOS_Wrapper defaultHelper]showVideo];
#endif
                return false;
            }
            
            void HideBanner()
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper]HideBanner];
#endif
            }
            
            void SetVisibleButonAds(bool visible)
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper]setVisibleBtnCloseAds:visible];
#endif
            }
            
            int GetVideoState()
            {
#if defined USE_SOCIAL_ADS
                return [[Admob_IOS_Wrapper defaultHelper]GetStateVideo];
#endif
                return 0;
            }
            
            void ResetVideoState()
            {
#if defined USE_SOCIAL_ADS
                [[Admob_IOS_Wrapper defaultHelper] ResetVideoState];
#endif
            }
        }
    }
}