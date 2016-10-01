#import <UIKit/UIKit.h>
#include "CasualCoreConfig.h"

#if CC_USE_ADSSERVER == 1

#include "AdServerManager.h"
#import "FreemiumIGP.h"

#define IGP_SET_IN_LANDSCAPE_RIGHT_MODE		1
#define IGP_SET_IN_LANDSCAPE_LEFT_MODE		2
#define IGP_SET_IN_PORTRAIT_MODE			3

#define LANDSCAPE_Y 88

@class AdViewController;
/************************************************************************
 * CasualADserverIOS
 */
@interface CasualADserverIOS : UIViewController <UIApplicationDelegate, UIWebViewDelegate, UITextViewDelegate>
{
  int bannerX;
  int bannerY;
  UIDeviceOrientation deviceOrientation;
  
  NSString* GAME_CODE;
  NSString* _requestString;
  
  NSMutableData* _receivedData;
}

@property (nonatomic, retain) IBOutlet AdViewController* adViewCtrler;
@property int bannerX;
@property int bannerY;
@property UIDeviceOrientation deviceOrientation;

- (id)   init;
- (void) initializeAdServer;
- (int)  languageIndexAdServer;

- (void) setBannerParameters: (int)x y:(int)y deviceOrientation:(int) orientation;
- (BOOL) isBannerVisible;
- (void) showBanner;
- (void) removeBanner;
- (void) didAdServerBannerRotate:(NSNotification *)notification;
- (void)getBannerSize: (int* )width : (int*)height;
/**
 * Display the the player get free cash
 */
- (void) showFreeCash;
- (BOOL) isInShowFreeCash;

// IGP code
- (void) showIGPView;
- (void) CheckFreeAdsEnable;

@end

#endif // CC_USE_ADSSERVER
