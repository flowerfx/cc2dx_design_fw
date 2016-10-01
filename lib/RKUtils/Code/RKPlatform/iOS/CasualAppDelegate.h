#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>
#import <CoreMotion/CoreMotion.h>

@class AdViewController;
@class NetworkState;
@class CasualMoviePlayer;
@class CasualViewController;
@class ActivityIndicator;
@class KeyboardView;
@class DatePickerView;
@class YouTubeView;

@interface CasualAppDelegate : UIResponder<UIApplicationDelegate, MFMessageComposeViewControllerDelegate, UIAlertViewDelegate, UIWebViewDelegate>
{
  UIWindow* _window;
  CasualViewController* _viewController;
  NetworkState* _networkState;
  CasualMoviePlayer* _moviePlayer;
  UIView* indicatorView;
  UIActivityIndicatorView* indicator;
  KeyboardView* _keyboardView;
  DatePickerView* _datePickerView;
  CMMotionManager* _motionManager;
  YouTubeView* _youTubeView;
  bool _gamePaused;
  bool _checkgameActiveAfterForeground;
}

@property (retain, nonatomic)     UIWindow* window;
@property (nonatomic, readonly)   CasualViewController* viewController;
@property (nonatomic, readonly)   NetworkState* networkState;
@property (nonatomic, readonly)   CasualMoviePlayer* moviePlayer;
@property (nonatomic, readonly)   ActivityIndicator* activityIndicator;
@property (nonatomic, readonly)   KeyboardView* keyboardView;
@property (nonatomic, readonly)   DatePickerView* datePickerView;
@property (nonatomic, readonly)   CMMotionManager* motionManager;

- (void) showLocalNotification:(id)params;

- (void) sendSMS:(NSString *)body recipientList:(NSArray *)recipients;

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;

- (void) showActivityIndicator: (BOOL) lockrenderbuffer;
- (void) hideActivityIndicator: (BOOL) unlockrenderbuffer;

-(void) launchYouTube: (NSString*)urlString :(bool)inChina :(bool)disableCloseWhilePlaying;
-(void) closeYouTube;
-(bool) isGamePaused;

@end
