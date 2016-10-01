#import "CasualAppDelegate.h"

//#include "Game.h"
#include "Platform.h"
#include "RKNSLog.h"
#include "SoundManager.h"
#include "Music.h"

#if CC_USE_ADSSERVER == 1
#include "RewardReceiver/ADServerManager.h"
#endif

#if CCONLINE_USE_ADSSERVER ==1
#include "../lib/CasualCoreOnline/Include/ADServerManager.h"
#import "AdViewController.h"
#endif

// For glot
#if CC_USE_DEBUG_HOST==1
#  include "DebugConnection/DebugHost.h"
#endif

#include "RKTexture.h"
#import "CasualViewController.h"
#import "CasualMoviePlayer.h"
#import "ActivityIndicator.h"
#import "KeyboardView.h"
#import "DatePickerView.h"
#import "NetworkState.h"
#import <MediaPlayer/MediaPlayer.h>
#include "Platform/Hardware.h"

#import "YouTubeView.h"

#if CC_USE_GLLIVE==1
  #import "GLiveViewController.h"
#endif

#include "ClientSNSInterface.h"

#include "OnlineService.h"
#include "InAppPurchase.h"

#import "Facebook/iphone/FacebookiOSGLSocialLib.h"

#include <sys/sysctl.h>

#if CC_USE_IN_GAME_BROWSER
#include "InGameBrowser.h"
#endif // CC_USE_IN_GAME_BROWSER

#include "RKDevice.h"

@interface MyViewController : UIViewController<UIWebViewDelegate>
@end
@implementation MyViewController

// MyViewController orientation information.
-(NSUInteger)supportedInterfaceOrientations
{
  return UIInterfaceOrientationMaskLandscape | UIInterfaceOrientationMaskPortrait;
}

-(BOOL)shouldAutorotate
{
  return YES;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}

-(void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation) fromInterfaceOrientation
{
    /*if (CasualCore::AdServerManager::IsValid())
     CasualCore::AdServerManager::GetInstance()->UpdateBannerOrientation();
     
     CasualCore::ServiceManager *service_manager = CasualCore::Game::GetInstance()->GetServiceManager();
     if (service_manager != NULL)
     {
     int rot = 0;
     if (fromInterfaceOrientation == UIInterfaceOrientationLandscapeRight)
     rot = 180;
     service_manager->DeviceOrientationChanged(rot);
     }*/
}

-(void) loadView
{
    self.view = [[UIView alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    self.view.backgroundColor = [UIColor colorWithWhite:1.f alpha:1.f];
}

-(BOOL) webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    if (navigationType == UIWebViewNavigationTypeReload)
    {
        //    NSLog(@"YouTube: WE HAVE FINISHED YAY!!!");
        CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
        [appdel closeYouTube];
    }
    return true;
}

@end

#import <CoreTelephony/CTCall.h>
#import <CoreTelephony/CTCallCenter.h>

namespace CasualCore {
  extern const char* GetGameDebugName();
}
extern CasualCore::Platform::DialogCallback g_dialogcallback;

@interface CasualAppDelegate()
{
  bool _first_launch;
}
@end

@implementation CasualAppDelegate

@synthesize window = _window;
@synthesize viewController = _viewController;
@synthesize networkState = _networkState;
@synthesize moviePlayer = _moviePlayer;
@synthesize keyboardView = _keyboardView;
@synthesize datePickerView = _datePickerView;
@synthesize motionManager = _motionManager;

/*
 Praveen - Print a programmer readable call stack
 */
void uncaughtExceptionHandler(NSException *exception)
{
    RKNSLOG(@"CRASH: %@", exception);
    RKNSLOG(@"Stack Trace: %@", [exception callStackSymbols]);
}

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
  //[chris, 2014-06-03] - crash bug 6542456 - : added this function.
  // This is needed for 'small' screen devices (eg iPod4, iPhone4s, 960x640) when view to display fit only in Portrait mode
  // eg: GameCenter login, see GameCenterGLSocialLib.mm : .. [s_gameViewController presentViewController:viewController .. ]
  // Warning: this may be bad for other views ... --> eg: Theater view .. will allow user to rotate to Portrait .. 

  // GLAds requires the app to support all orientations. Instead changing the info.plist, here we enable support all orientations
  // In game view controller, the only supported orientation, so the game only displays in landsacpe mode
  return UIInterfaceOrientationMaskAll;
}



- (BOOL)application:(UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*)launchOptions
{
   NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);
  [application setStatusBarHidden: YES withAnimation: UIStatusBarAnimationNone];
  RKHeap_InitModule();
  CasualCore::Hardware::Create();
  
#if CC_USE_DEBUG_HOST==1
  dbg::DebugHost::Create(CasualCore::GetGameDebugName(), CasualCore::Hardware::GetInstance()->GetDeviceType().GetString(), CC_DEBUG_HOST_META_IP, CC_DEBUG_HOST_META_PORT, CC_DEBUG_HOST_LISTENING_PORT);
#endif
  

  CasualCore::Game::GetInstance()->UpdateClientId();

  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  [self.window release];
    _viewController = [[CasualViewController alloc] init];
  
  CasualCoreOnline::CCOnlineService::Get()->OnAppLaunch(self.window, _viewController.view);
  
  _networkState = [[NetworkState alloc] init];
  _moviePlayer = [[CasualMoviePlayer alloc] initWithViewController:_viewController];
  _activityIndicator = [[ActivityIndicator alloc] initWithViewController:_viewController];
  _keyboardView = [[KeyboardView alloc] initWithViewController:_viewController];
  _datePickerView = [[DatePickerView alloc] initWithViewController:_viewController];
  _motionManager = [[CMMotionManager alloc] init];
  _gamePaused = false;
  _checkgameActiveAfterForeground = false;
  
  
  self.window.rootViewController = self.viewController;
  self.window.multipleTouchEnabled = YES;
  [self.window makeKeyAndVisible];
  
  
  if (launchOptions != NULL)
	{
		NSDictionary* dict = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
		if (dict != NULL)
		{
			NSString* launchType = [dict objectForKey:@"type"];
      NSString* body = [[[dict objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"body"];
			
      if (launchType != NULL && [launchType isEqualToString:@"igpcode"]) //Promotion notification
      {
        NSString* igpCode = [dict objectForKey:@"igpcode"];
        NSString* buttonRight = [dict objectForKey:@"button_ignore"];
        NSString* buttonLeft = [[[dict objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"action-loc-key"];
        
        if (!buttonLeft || [buttonLeft isKindOfClass:[NSNull null]] || buttonLeft == (NSString*)[NSNull null])
        {
          buttonLeft = @"MANUAL_LOCALIZE_STR_GUI_OK";
        }
        
        if (!buttonRight || [buttonRight isKindOfClass:[NSNull null]] || buttonRight == (NSString*)[NSNull null])
        {
          buttonRight = @"MANUAL_LOCALIZE_STR_UI_IGNORE";
        }

        CasualCore::Game::GetInstance()->SetPNStart(52229/*TRACKING_PN_TYPE_IGP_NOTIFICATIONS*/);
        CasualCore::Game::GetInstance()->OnServerPushNotification([igpCode UTF8String], [buttonLeft UTF8String], [buttonRight UTF8String], [body UTF8String]);
      }
      else if (body != nil)
      {
        NSString* buttonRight = [[[dict objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"action-loc-key"];
        if (!buttonRight || [buttonRight isKindOfClass:[NSNull null]] || buttonRight == (NSString*)[NSNull null])
        {
          buttonRight = @"MANUAL_LOCALIZE_STR_GUI_OK";
        }

        CasualCore::Game::GetInstance()->SetPNStart(52230/*TRACKING_PN_TYPE_NOTICE_NOTIFICATIONS*/);
        CasualCore::Game::GetInstance()->OnServerPushNotification(NULL,NULL,[buttonRight UTF8String], [body UTF8String]);
      }
    }
    
    //Pull Tracking ID for Local Notification
    Class cls = NSClassFromString(@"UILocalNotification");
    if (cls)
    {
      UILocalNotification *notification = [launchOptions objectForKey:UIApplicationLaunchOptionsLocalNotificationKey];
      if (notification != nil && notification && NULL!= notification.userInfo)
      {
        NSNumber* iTrackingID = [notification.userInfo objectForKey:@"trackingid"];
        if(NULL!= iTrackingID)
        {
          NSInteger iTrackingValue = [iTrackingID integerValue];
          CasualCore::Game::GetInstance()->SetPNStart(iTrackingValue);
        }
      }
    }
  }

#if CC_USE_GLLIVE==1
  [[NSNotificationCenter defaultCenter] addObserver:(id)([GLiveViewController sharedSingleton]) selector:@selector(didRotate:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:(id)([GLiveViewController sharedSingleton]) selector:@selector(keyboardWillShow:) name:@"UIKeyboardWillChangeFrameNotification" object:nil];
#endif

  // User music notifications
  CasualCore::Game::GetInstance()->GetMusic()->EnableUserMusicNotifications(true);
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userMusicStateChanged:) name:MPMusicPlayerControllerPlaybackStateDidChangeNotification object:nil];
  
  return YES;
}

// returns true if "string" starts with "prefix"
static bool startsWith(const char* string, const char* prefix)
{
  return strncmp(string, prefix, strlen(prefix)) == 0;
}

- (void) userMusicStateChanged:(NSNotification *)notification
{
  MPMusicPlayerController* iPodPlayer = [notification object];
  
  if (iPodPlayer)
    CasualCore::Game::GetInstance()->GetMusic()->OnUserMusicStateChanged([iPodPlayer playbackState] == MPMusicPlaybackStatePlaying);
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
  // e.g. fe2ee39be1e93d2d8edb3c07e8c0ae4ba36a395142dc2d1c6a79840ab6e1512a
  NSString* deviceTokenString = [[[deviceToken description]
                                  stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]]
                                 stringByReplacingOccurrencesOfString:@" " withString:@""];
  
  CasualCore::Game::GetInstance()->SetDeviceToken([deviceTokenString UTF8String]);
  CasualCoreOnline::CCOnlineService::Get()->SetDeviceToken([deviceTokenString UTF8String]);
  // Do not change this to RKNSLOG. Testers need to see this log to figure out the deviceTokenString for sending push notifications
  //Testers can retrieve this using the Gameloft Fed Debug tool, removing from retail builds
	RKNSLOG(@"@application:didRegisterForRemoteNotificationsWithDeviceToken: %@", deviceTokenString);
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
  // NOTE(Diego): if it is failing, please check:
  //              1. The Bundle ID is correct in Info.plist.
  //              2. The provisioning profile is correct.
	RKNSLOG(@"@application:didFailToRegisterForRemoteNotificationsWithError: %@", error);
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
	RKNSLOG(@"@application:didReceiveRemoteNotification: %@", userInfo);
  
  //Safety Checks
  if ([userInfo objectForKey:@"aps"] == nil || [[userInfo objectForKey:@"aps"] objectForKey:@"alert"] == nil)
    return;
  
  NSString* notificationType = [userInfo objectForKey:@"type"];
  NSString* body = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"body"];
  
  CasualCore::Game* game = CasualCore::Game::GetInstance();
  assert(game);
  
  bool processedPN = false;
  if (notificationType != nil)
  {
    if ([notificationType isEqualToString:@"play"])
    {
      //Safety Checks
      if ([userInfo objectForKey:@"fromGGI"] == nil)
        return;

      int ggi = [[userInfo objectForKey:@"fromGGI"] intValue];
      NSString* userName = [userInfo objectForKey:@"username"];
      NSString* body = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"body"];

      game->OnServerPushNotification(NULL,NULL,NULL,[body UTF8String]);

      processedPN = true;
      CasualCore::Game::GetInstance()->SetPNResume(52230/*TRACKING_PN_TYPE_NOTICE_NOTIFICATIONS*/);
    }
    else if ([notificationType isEqualToString:@"igpcode"]) //Promotion notification
    {
      NSString* igpCode = [userInfo objectForKey:@"igpcode"];
      NSString* buttonRight = [userInfo objectForKey:@"button_ignore"];
      NSString* buttonLeft = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"action-loc-key"];
      NSString* bodyString = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"body"];

      if (!buttonLeft || [buttonLeft isKindOfClass:[NSNull null]] || buttonLeft == (NSString*)[NSNull null])
      {
        buttonLeft = @"MANUAL_LOCALIZE_STR_GUI_OK";
      }

      if (!buttonRight || [buttonRight isKindOfClass:[NSNull null]] || buttonRight == (NSString*)[NSNull null])
      {
        buttonRight = @"MANUAL_LOCALIZE_STR_UI_IGNORE";
      }

      
      game->OnServerPushNotification(
                                            [igpCode UTF8String],
                                            [buttonLeft UTF8String],
                                            [buttonRight UTF8String],
                                            [bodyString UTF8String]);
      processedPN = true;
      CasualCore::Game::GetInstance()->SetPNResume(52229/*TRACKING_PN_TYPE_IGP_NOTIFICATIONS*/);
    }
	}
	
  if (body != nil && !processedPN)
	{
    NSString* buttonRight = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"action-loc-key"];
    NSString* bodyString = [[[userInfo objectForKey:@"aps"] objectForKey:@"alert"] objectForKey:@"body"];

    if (buttonRight == nil || [buttonRight isKindOfClass:[NSNull null]] || buttonRight == (NSString*)[NSNull null])
    {
      buttonRight = @"MANUAL_LOCALIZE_STR_GUI_OK";
    }

    // Check is the application is active before sending the PN, to avoid sending the PN twice(In backround and foreground)
    UIApplicationState currentAppState = [application applicationState];
    if(currentAppState == UIApplicationStateActive)
    {
      game->OnServerPushNotification(NULL,NULL, [buttonRight UTF8String],[bodyString UTF8String]);
    }
    
    CasualCore::Game::GetInstance()->SetPNResume(52230/*TRACKING_PN_TYPE_NOTICE_NOTIFICATIONS*/);
  }
  
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
	RKNSLOG(@"@application:didReceiveLocalNotification:");
  
  UIApplicationState state = [application applicationState];
  if (state == UIApplicationStateInactive)
  {
    NSNumber* iTrackingID = [notification.userInfo objectForKey:@"trackingid"];
    if(nil != iTrackingID)
    {
      NSInteger iTrackingValue = [iTrackingID integerValue];
      CasualCore::Game::GetInstance()->SetPNResume(iTrackingValue);
    }
  }
}

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
    //register to receive notifications
    [application registerForRemoteNotifications];
}

- (void)application:(UIApplication *)application handleActionWithIdentifier:(NSString *)identifier forRemoteNotification:(NSDictionary *)userInfo completionHandler:(void(^)())completionHandler
{
  //handle the actions
  if ([identifier isEqualToString:@"declineAction"])
  {
  }
  else if ([identifier isEqualToString:@"answerAction"])
  {
  }
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication *)application
{
  RKNSLOG(@"@application: Received memory warning, running lean.");
  [_viewController runLean: TRUE];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL*)url
{
  std::string urlStr = [[url absoluteString] UTF8String];
  
  CasualCoreOnline::CCOnlineService::Get()->HandleOpenURL(urlStr.c_str());
  
  return sociallib::CLIENT_SNS_INTERFACE->handleOpenUrl(sociallib::CLIENT_SNS_FACEBOOK, urlStr);
}

- (void) applicationWillTerminate: (UIApplication*) application
{
  CasualCoreOnline::CCOnlineService::Get()->SetIsAppExiting();
  RKNSLOG(@"[SAVGAM] CasualAppDelegate.mm: applicationWillTerminate()...");
  bool wantQuickExit = CasualCore::Game::DestroyInstance();

#if CC_USE_DEBUG_HOST==1
  //chris, to fix..  dbg::DebugHost::Destroy();
#endif
  
  self.window = nil;
  [_viewController release]; _viewController = nil;
  [_networkState release]; _networkState = nil;
  [_moviePlayer release]; _moviePlayer = nil;
  [_activityIndicator release]; _activityIndicator = nil;
  [_keyboardView release]; _keyboardView = nil;
  [_datePickerView release]; _datePickerView = nil;
  [_motionManager release]; _motionManager = nil;
  
  if (wantQuickExit)
  {
    RKNSLOG(@"[SAVGAM] applicationWillTerminate() All is OK: wantQuickExit: ~~~~~~~~~~~~~~~ THE_DOORS_SAYS_THIS_IS_THE_END ~~~~~~~~~~~~~");   //last log ...
    return;
  }
  
  CasualCore::Hardware::Destroy();
  
  //uncomment this when all leaks are fixed... :   RKHeap_DeinitModule();
  RKHeapError_EnableReport(false);  //better to not see this report...    true); //re-enable full memory dump if leaks
  //at least we are here, without crash !
  RKNSLOG(@"[SAVGAM] applicationWillTerminate() All is OK! ~~~~~~~~~~~~~~~ THE_DOORS_SAYS_THIS_IS_THE_END ~~~~~~~~~~~~~");   //last log ...
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
  RKNSLOG(@"[SAVGAM] CasualAppDelegate.mm: applicationDidEnterBackground()...");
  CasualCore::Game::GetInstance()->Minimise();

#if CC_USE_GLLIVE==1
  [[GLiveViewController sharedSingleton] reloadGlive];
#endif
  
  //_gamePaused = true;
  _checkgameActiveAfterForeground = true;
  [self.viewController pauseDisplay:YES];

  [self.viewController suspendGL];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
  RKNSLOG(@"[SAVGAM] CasualAppDelegate.mm: applicationWillEnterForeground()...");

#if CC_USE_GLLIVE==1
  [[GLiveViewController sharedSingleton] stopRefresh];
#endif

  [self.viewController pauseDisplay:NO];
  _gamePaused = false;
  
  if (_youTubeView != nil)
  {
    [_youTubeView close];
  }

  CasualCore::Game::GetInstance()->Maximise();
}

- (void)applicationWillResignActive:(UIApplication *)application {
  /*
   Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
   Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
   */
  RKNSLOG(@"[SAVGAM] CasualAppDelegate.mm: applicationWillResignActive()...");
  _gamePaused = true;
  CasualCore::Game::GetInstance()->ResignActive();
  //[self.viewController pauseDisplay:YES];
  // OJW - 20120719 - Fix bug 5194481
  if (self.moviePlayer)
  {
    [self.moviePlayer pause];
  }
  if(CasualCore::Game::GetInstance()->GetSoundManager() && CasualCore::Game::GetInstance()->GetSoundManager()->IsInitialised())
  {
    CasualCore::Game::GetInstance()->GetMusic()->EnableUserMusicNotifications(false);
    CasualCore::Game::GetInstance()->GetSoundManager()->SuspendEngine();
  }
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
  /*
   Restart any tasks that were paused while the application was inactive
   */
  //if (_gamePaused) // won't be called for initial launch, only for resume from background
  {
    CasualCore::Game::GetInstance()->BecomeActive();
    
    //[self.viewController pauseDisplay:NO];
    
    // OJW - 20120719 - Fix bug 5191965
    if (self.moviePlayer)
    {
      [self.moviePlayer resume];
    }
    
    if(CasualCore::Game::GetInstance()->GetSoundManager() && CasualCore::Game::GetInstance()->GetSoundManager()->IsInitialised())
    {
      CasualCore::Game::GetInstance()->GetSoundManager()->ResumeEngine();
      CasualCore::Game::GetInstance()->GetMusic()->EnableUserMusicNotifications(true);
      CasualCore::Game::GetInstance()->GetMusic()->OnResumeGame();
    }
    
    _gamePaused = false;
  }
  //sociallib::CLIENT_SNS_INTERFACE->handleOnResume();
  if (_checkgameActiveAfterForeground)
  {
     CasualCore::Game::GetInstance()->BecomeActiveAfterComingForeground();
  }
  _checkgameActiveAfterForeground = false;

 }

- (void)showLocalNotification:(id)params
{
  enum {eDefaultDelayInSeconds = 60};

  if (NSNumber* iTrackingID = [params objectForKey:@"trackingid"])
  {
    NSDictionary *userinfo = [NSDictionary dictionaryWithObjectsAndKeys:iTrackingID, @"trackingid", nil];
    id notification = [[UILocalNotification alloc] init];
    if (id delayInSeconds = [params objectForKey:@"delayInSeconds"])
    {
      [notification setFireDate:[NSDate dateWithTimeIntervalSinceNow:[delayInSeconds intValue]]];
    }
    else
    {
      [notification setFireDate:[NSDate dateWithTimeIntervalSinceNow:eDefaultDelayInSeconds]];
    }
    [notification setTimeZone:[NSTimeZone defaultTimeZone]];
    [notification setAlertBody:[params objectForKey:@"message"]];
    [notification setAlertAction:[params objectForKey:@"action"]];
    [notification setApplicationIconBadgeNumber:[[params objectForKey:@"showBadge"] intValue]];
    [notification setUserInfo:userinfo];
    [notification setSoundName:@"sound/sfx_push_notification.wav"];
    
    [[UIApplication sharedApplication] scheduleLocalNotification:notification];
    [notification release];
  }
}

// sendSMS and friends
-(void)sendSMS:(NSString *)body recipientList:(NSArray *)recipients
{
	MFMessageComposeViewController* controller = [[[MFMessageComposeViewController alloc] init] autorelease];
	if([MFMessageComposeViewController canSendText])
	{
		controller.body = body;
		controller.recipients = recipients;
		controller.messageComposeDelegate = self;
 		[self.viewController presentViewController:controller animated:YES completion:nil];	// Replacing the deprecated in iOS 6.0: [self.viewController presentModalViewController:controller animated:YES];
	}
}

- (void)messageComposeViewController:(MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result
{
	switch (result)
  {
		case MessageComposeResultCancelled:
			RKNSLOG(@"Cancelled");
			break;
		case MessageComposeResultFailed:
    {
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"MyApp" message:@"Unknown Error" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
			[alert show];
			[alert release];
			break;
		}
    case MessageComposeResultSent:
			break;
		default:
			break;
	}
  controller.messageComposeDelegate = nil;
    [self.viewController dismissViewControllerAnimated:YES completion:nil];		// Replacing the deprecated in iOS 6.0:   [self.viewController dismissModalViewControllerAnimated:YES];
};

-(void) showActivityIndicator: (BOOL) lockrenderbuffer
{
  UIViewController * root = [UIApplication sharedApplication].keyWindow.rootViewController;

  if(indicatorView == nil)
  {
    CGRect rect = [root.view bounds];
    indicatorView = [[UIView alloc] initWithFrame:rect];
    indicatorView.backgroundColor = [[UIColor grayColor] colorWithAlphaComponent:0.6f];

    //UIView* _topView = [[[[UIApplication sharedApplication] keyWindow] subviews] lastObject];
    [root.view addSubview:indicatorView];
  }
  if (indicator == nil)
  {
    indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    CGRect position = indicator.frame;
    position.origin.x = (indicatorView.frame.size.width - indicator.frame.size.width) / 2;
    position.origin.y = (indicatorView.frame.size.height - indicator.frame.size.height) / 2;
    indicator.frame = position;
    [indicatorView addSubview:indicator];
  }

  [indicator setHidden:NO];
  [indicator startAnimating];

  if (lockrenderbuffer)
  {
    [[self viewController] setRenderBufferLocked: YES];
  }
}

- (void)hideActivityIndicator: (BOOL) unlockrenderbuffer
{
  [indicator removeFromSuperview];
  [indicator release], indicator = nil;

  [indicatorView removeFromSuperview];
  [indicatorView release], indicatorView = nil;

  if (unlockrenderbuffer)
  {
    [[self viewController] setRenderBufferLocked: NO];
  }
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
  [alertView dismissWithClickedButtonIndex: buttonIndex animated: YES];
  if (g_dialogcallback)
  {
    g_dialogcallback(buttonIndex);
    g_dialogcallback = 0;
  }
}

- (void) launchYouTube:(NSString *)urlString :(bool)inChina :(bool)disableCloseWhilePlaying
{
  if (_youTubeView)
    return;
  int width = RKDevice_GetWidth() / self.window.rootViewController.view.contentScaleFactor;
  int height = RKDevice_GetHeight() / self.window.rootViewController.view.contentScaleFactor;;
  RKNSLOG(@"launchYouTube: %d,%d", width, height);
  CGRect window_frame = CGRectMake(0, 0, width, height);
  _youTubeView = [[YouTubeView alloc] initWithStringAsURL:urlString frame:window_frame isChinese:inChina :disableCloseWhilePlaying];
  [self.window.rootViewController.view addSubview:_youTubeView];
  _youTubeView.delegate = (MyViewController*)self.window.rootViewController;
  
  [self.viewController pauseDisplay:YES];
}

- (void) closeYouTube
{
  if (nil == _youTubeView)
    return;
  
  NSString* videoId = [[_youTubeView getVideoId] copy];
  [_youTubeView removeFromSuperview];
  [_youTubeView release];
  _youTubeView = nil;
  [self.viewController pauseDisplay:NO];
  [videoId release];
}

- (bool) isGamePaused
{
  return _gamePaused;
}

@end
