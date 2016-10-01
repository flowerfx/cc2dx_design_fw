

#ifndef _FB_IOS_HELPER_H_
#define _FB_IOS_HELPER_H_
#if defined USE_SOCIAL_ADS
#import <Foundation/Foundation.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>
#import <FBSDKMessengerShareKit/FBSDKMessengerShareKit.h>

#import <Bolts/Bolts.h>

#include "RKString_Code/RKString.h"
using namespace RKUtils;
@interface FB_IOS_Helper : UIViewController<FBSDKSharingDelegate , FBSDKLoginTooltipViewDelegate,FBSDKGraphRequestConnectionDelegate> {
    //UIViewController* authVC;
    
    RKString name_facebook_;
    RKString id_facebook_ ;
    RKString email_facebook_ ;
}

//@property (nonatomic, retain) UIViewController* authVC;
@property (assign, nonatomic) RKString name_facebook_;
@property (assign, nonatomic) RKString id_facebook_;
@property (assign, nonatomic) RKString email_facebook_;

+ (FB_IOS_Helper*)defaultHelper;

- (void)LoginFacebook;
- (void)GetFrofileFBWithAccessToken;
- (RKString) GetIDFacebook;
- (RKString) GetNameFacebook;
- (RKString) GetURLAvatarFBFromID:(NSString*) user_id;
- (int) PostMessageToFacebook:(NSString*)message withURL:(NSString*) url;
@end
#endif
#endif // _FB_IOS_HELPER_H_

