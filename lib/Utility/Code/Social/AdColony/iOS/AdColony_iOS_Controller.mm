//
//  ViewController.m
//  AdColonyV4VC
//
//  Created by Owain Moss on 1/14/15.
//  Copyright (c) 2015 AdColony. All rights reserved.
//
#if defined USE_SOCIAL_ADS
#import "AdColony_iOS_Controller.h"
#include "../../Admob/iOS/Admob_IOS_Wrapper.h"
#import "Constants.h"

@interface AdColonyHelper ()

@end


@implementation AdColonyHelper

#pragma mark - UIViewController overrides

- (instancetype)initWithContentViewController:(UIViewController*) controller withIDApp:(NSString*) id_app andIDZone:(NSString*) id_zone;
{
    self = [super init];
    if (self != nil) {
        
        p_id_app = id_app;
        p_id_zone = id_zone;
        _contentController = controller;
        p_current_state_video = 0;
        
        [AdColony configureWithAppID:p_id_app zoneIDs:@[p_id_zone] delegate:self logging:YES];
        
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    

    [self zoneLoading];

}

- (UIStatusBarStyle)preferredStatusBarStyle {
    return UIStatusBarStyleLightContent;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskAll;
}

- (BOOL)shouldAutorotate {
    return YES;
}

#pragma mark - UI Updates

- (void)zoneLoading {
 
}

- (void)zoneReady {

}

- (void)zoneOff {

}

#pragma mark - AdColony
- (void)onAdColonyV4VCReward:(BOOL)success currencyName:(NSString*)currencyName currencyAmount:(int)amount inZone:(NSString*)zoneID {
    NSLog(@"AdColony zone %@ reward %@ %i %@", zoneID, success ? @"YES" : @"NO", amount, currencyName);
    
    if (success)
    {
        p_current_state_video = 2;
        [[Admob_IOS_Wrapper defaultHelper]setVisibleBtnCloseVideoAds:FALSE];
    }
}

#pragma mark - AdColony ad fill

//Callback triggered when the state of ad readiness changes
//When the AdColony SDK tells us our zone either has, or doesn't have, ads, we
// need to tell our view controller to update its UI elements appropriately
- (void)onAdColonyAdAvailabilityChange:(BOOL)available inZone:(NSString*) zoneID {
    if (available)
    {
        p_is_video_available = YES;
        NSLog(@"video available");
    }
    else
    {
        p_is_video_available = NO;
        NSLog(@"no available video , if the issue occur continuesly , check the plist info!");
    }
}

- (void) CloseVideo
{
    if(p_is_video_available == YES && [AdColony videoAdCurrentlyRunning])
    {
        [AdColony cancelAd];
        p_current_state_video = 2;
    }
}

- (void)ShowCloseBtn
{
    [[Admob_IOS_Wrapper defaultHelper]setVisibleBtnCloseVideoAds:TRUE];
}

- (bool) ShowVideo
{
    if(p_is_video_available == YES)
    {
        [AdColony playVideoAdForZone:p_id_zone withDelegate:self];
        
        [self performSelector:@selector(ShowCloseBtn) withObject:nil afterDelay:7];
        
        p_current_state_video = 1;
        return true;
    }
    else
    {
        NSLog(@"cannot play video : no video available right now!");
        return false;
    }
}

- (void)onAdColonyAdAttemptFinished:(BOOL)shown inZone:(NSString *)zoneID {
    
    ADCOLONY_ZONE_STATUS current_status = [AdColony zoneStatusForZone:zoneID];
    
    if (!shown && current_status != ADCOLONY_ZONE_STATUS_ACTIVE)
    {
        [self zoneLoading];
    }
    else if (!shown)
    {
        [self zoneReady];
    }
}

- (BOOL) IsVideosPlaying
{
    if([AdColony videoAdCurrentlyRunning])
    {
        return YES;
    }
    else{
        return NO;
    }
}

- (int)  GetStateVideo
{
    return p_current_state_video;
}

- (void) ResetStateVideo
{
    p_current_state_video = 0;
}

@end
#endif