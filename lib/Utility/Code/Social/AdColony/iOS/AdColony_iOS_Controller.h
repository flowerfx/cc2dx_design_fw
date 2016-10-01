//
//  ViewController.h
//  AdColonyV4VC
//
//  Created by Owain Moss on 1/14/15.
//  Copyright (c) 2015 AdColony. All rights reserved.
//
#if defined USE_SOCIAL_ADS

#import <UIKit/UIKit.h>
#import <AdColony/AdColony.h>

#include "RKString_Code/RKString.h"
@interface AdColonyHelper : UIViewController <AdColonyAdDelegate ,AdColonyDelegate>{
    NSString * p_id_app;
    NSString * p_id_zone;
    
    BOOL       p_is_video_available;
    int        p_current_state_video; // 0 is none , 1 is playing, 2 is finish
    
    UIViewController *_contentController;
}
- (instancetype)initWithContentViewController:(UIViewController*) controller withIDApp:(NSString*) id_app andIDZone:(NSString*) id_zone;
- (bool) ShowVideo;
- (void) CloseVideo;
- (void) OnFinishVideo;
- (BOOL) IsVideosPlaying;
- (int)  GetStateVideo;
- (void) ResetStateVideo;
@end
#endif