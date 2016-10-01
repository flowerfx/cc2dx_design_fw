

#ifndef _ADMOB_IOS_HELPER_H_
#define _ADMOB_IOS_HELPER_H_

#if defined USE_SOCIAL_ADS

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <GoogleMobileAds/GoogleMobileAds.h>

#include "RKString_Code/RKString.h"
using namespace RKUtils;
@interface Admob_IOS_Helper : UIViewController<GADBannerViewDelegate>{
    GADBannerView * _bannerView;
    UIViewController *_contentController;
    BOOL _bannerLoaded;
}

- (instancetype)initWithContentViewController:(UIViewController *)contentController andAppId:(NSString *)id_app;
- (void) hideBanner;
- (void) showBanner;
@end
#endif
#endif // _ADMOB_IOS_HELPER_H_

