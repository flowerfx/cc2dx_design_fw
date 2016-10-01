#include "AdServerManager.h"
#if CC_USE_ADSSERVER==1

#include "Game.h"
//#include "SBJSON/JSON.h"
#include "RKList.h"
#include "RKString.h"
#include "Tools/Encryption.h"
#include "RKNSLog.h"
// Ad Server including files end

#import "AdServerManagerIOS.h"
#import "CasualAppDelegate.h"

#import "AdViewController.h"

//#define TEST_TAPJOY_REWARD_LOACALLY_DEBUG 1 // For test retrieve tapjoy item offline

// Tapjoy callback request
//#define TAPJOY_REWARD_LINK_BETA @"http://gllive-beta.gameloft.com/callback/tapjoy/?action=registertransaction&game=YOURGAMEGGI&type=credits"
//#define TAPJOY_REWARD_LINK_GOLD @"http://gllive.gameloft.com/callback/tapjoy/?action=registertransaction&game=YOURGAMEGGI&type=credits"
#define TAPJOY_REWARD_LINK_BETA @"http://gllive-beta.gameloft.com/ope/tapjoy.php?action=ACTIONTYPE&game=GAMEGGI&user=USER";
#define TAPJOY_REWARD_LINK_GOLD @"http://gllive.gameloft.com/ope/tapjoy.php?action=ACTIONTYPE&game=GAMEGGI&user=USER";
#define ONE_MEG 104857

using namespace CasualCore;


/*** CasualADserverIOS ********************************************************************/
@implementation CasualADserverIOS

@synthesize adViewCtrler;
@synthesize deviceOrientation;
@synthesize bannerX;
@synthesize bannerY;

static UIDeviceOrientation getDeviceOrientation(int orientation)
{
    switch (orientation) {
        case 1:
            return UIDeviceOrientationPortrait;
        case 2:
            return UIDeviceOrientationPortraitUpsideDown;
        case 3:
            return UIDeviceOrientationLandscapeLeft;
        case 4:
            return UIDeviceOrientationLandscapeRight;
        case 5:
            return UIDeviceOrientationFaceUp;
        case 6:
            return UIDeviceOrientationFaceDown;
        default:
            break;
    }
    return UIDeviceOrientationUnknown;
}

- (id)init
{
  self = [super initWithNibName:nil bundle:nil];
  if (self)
  {  
  }
  return self;
}

- (void) initializeAdServer
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON
  
  // If ... I said if... by any reason the function is NOT running in main thread, we need to create a function initalizeViews()
  // and call the function in following way to make sure the Ad Server initialization running in the main thread
  // due to the GUI things have to stay in main thread in iOS. :0
  // [gAdAppViewDelegate performSelectorOnMainThread:@selector(initalizeViews) withObject:nil waitUntilDone:YES];
  
  // Language
  int languageIndex = static_cast<int>(CasualCore::Game::GetInstance()->GetLanguage());
  // Game version
  NSString* gameversion = [NSString stringWithUTF8String:(CasualCore::Game::GetInstance()->GetGameVersion())];
  // Game code
  const char* igpGameCode = CasualCore::Game::GetInstance()->GetGameCode();
  NSString* gamecode = [[NSString alloc] initWithUTF8String:igpGameCode];
  [[AdViewController sharedAdViewController] initParteners:languageIndex :gameversion :gamecode];
  
  [[NSNotificationCenter defaultCenter] addObserver:(id) (self) selector:@selector(didAdServerBannerRotate:) name:@"UIDeviceOrientationDidChangeNotification" object:nil ];
#endif
}

-(void) setBannerParameters:(int)x y:(int)y deviceOrientation:(int) orientation
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation 
    self.bannerX = x;
    self.bannerY = y;
    deviceOrientation = getDeviceOrientation(orientation);
#endif // End of Ad Server implementation
}

- (BOOL)isBannerVisible
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation 
    if(nil != [AdViewController sharedAdViewController])
    {
        return [[AdViewController sharedAdViewController] bannerIsVisible];
    }
#endif
    return NO;
}

-(void) showBanner
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation 
    if(nil != [AdViewController sharedAdViewController])
    {
        //CasualAppDelegate * delegate = [(CasualAppDelegate*)[UIApplication sharedApplication]delegate];
        //[delegate.viewController.view addSubview:adViewCtrler.view];
        
        // Initialize the game code
        const char* gamecode = CasualCore::Game::GetInstance()->GetGameCode();
        //GAME_CODE = [NSString stringWithUTF8String:(gamecode)];
        GAME_CODE = [[NSString alloc] initWithUTF8String:gamecode];
        // RKNSLOG(@"--- game code : %@",GAME_CODE);
        
        // Attach the Ad view to the main UI view
        //CasualAppDelegate * delegate = [(CasualAppDelegate*)[UIApplication sharedApplication]delegate];
        //[delegate.viewController.view addSubview:adViewCtrler.view];
        UIWindow*  uiwindow = [[[UIApplication sharedApplication] delegate] window];
        [uiwindow addSubview:[[AdViewController sharedAdViewController] view]];
        //[uiwindow bringSubviewToFront:adViewCtrler.view];
        int languageIndex = (int)(CasualCore::Game::GetInstance()->GetLanguage());
      
        //TURKEY HACK
        if (languageIndex >= 10)
          languageIndex = 0;
      
        /*UIDeviceOrientationUnknown,
         UIDeviceOrientationPortrait,            // Device oriented vertically, home button on the bottom
         UIDeviceOrientationPortraitUpsideDown,  // Device oriented vertically, home button on the top
         UIDeviceOrientationLandscapeLeft,       // Device oriented horizontally, home button on the right
         UIDeviceOrientationLandscapeRight,      // Device oriented horizontally, home button on the left
         UIDeviceOrientationFaceUp,              // Device oriented flat, face up
         UIDeviceOrientationFaceDown             // Device oriented flat, face down*/
     
        if(![[AdViewController sharedAdViewController] showBanner:languageIndex :GAME_CODE :self.bannerX :self.bannerY :AD_BOTTOM_CENTER :deviceOrientation])
        {
            RKNSLOG(@"Cannot display AD banner in the AdViewController.");
            return;
        }
        return;
    }
    RKNSLOG(@"Cannot display banner, AdViewController is not initialized. PlatformIOS::ShowBanner()");
#endif // End of Ad Server implementation
}

- (void) removeBanner
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation
    [[AdViewController sharedAdViewController] hideBanner];
    [[AdViewController sharedAdViewController].view removeFromSuperview];
#endif
}

-(void)getBannerSize: (int* )width : (int*)height
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation
  [[AdViewController sharedAdViewController] getBannerSize:width : height];
#endif
}

- (void) showFreeCash
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation
  int languageIdx = (int)(CasualCore::Game::GetInstance()->GetLanguage());
  //TURKEY HACK
  if (languageIdx >= 10)
    languageIdx = 0;
  
  NSString * game_code = [[NSString alloc] initWithUTF8String: CasualCore::Game::GetInstance()->GetGameCode()];

  int orientation = static_cast<int>(CasualCore::Game::GetInstance()->GetPlatform()->GetScreenOrientation());
  deviceOrientation = getDeviceOrientation(orientation);
  if(![[AdViewController sharedAdViewController] showFreeCash :languageIdx :game_code :deviceOrientation])
    RKNSLOG(@"AdServerManagerIOS::showFreeCash Failed to show free cash, no connection.");
  [game_code release];
#endif // Ad server implementation
}

- (BOOL) isInShowFreeCash
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation
  if(nil != [AdViewController sharedAdViewController])
  {
      return [[AdViewController sharedAdViewController] isInShowFreeCash ];
  }
#endif
    return NO;
}

- (void) didAdServerBannerRotate:(NSNotification *)notification
{
    if([AdViewController sharedAdViewController])
    {
        UIDeviceOrientation currentOrientation = [[UIDevice currentDevice] orientation];
        
        if(//(currentOrientation == UIDeviceOrientationPortrait)||
           //(currentOrientation == UIDeviceOrientationPortraitUpsideDown)||
           (currentOrientation == UIDeviceOrientationLandscapeLeft)||
           (currentOrientation == UIDeviceOrientationLandscapeRight))
        {
            [[AdViewController sharedAdViewController] setBannerOrientation:currentOrientation];
        }
    }
}


- (void) showIGPView
{
  int language = (int)(CasualCore::Game::GetInstance()->GetLanguage());
  //TURKEY HACK
  if (language >= 10)
    language = 0;
  
  CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
  
  [[FreemiumIGP sharedFreemiumIGP]
    showOnView:appDelegate.window
    withLanguageIndex:language
    andGameVersion:[NSString stringWithUTF8String:(CasualCore::Game::GetInstance()->GetGameVersion())]];
}
- (void) CheckFreeAdsEnable
{
#ifdef GAMELOFT_AD_SERVER_IOS_ON  // Ad server implementation
#endif // Ad server implementation
}
- (void)dealloc {
    [super dealloc];
}
@end // end of CasualADserverIOS


/*************************************************************************************************************
 *                              C++ part, implementation of the AdServerManager
 *************************************************************************************************************/
static CasualADserverIOS* ptrAdServer = nil;
namespace CasualCore {
  
  AdServerManager::AdServerManager()
  : _adsImpl(NULL), m_pfnCallback(NULL)
  {
    ptrAdServer = nil;
  }
  
  AdServerManager::~AdServerManager()
  {
    [ptrAdServer release];
    ptrAdServer = nil;
    
    delete _adsImpl;
  }
  
  void AdServerManager::InitalizeAdServer()
  {
    //CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    if(ptrAdServer == nil)
    {
      ptrAdServer = [[CasualADserverIOS alloc] init];
      [ptrAdServer initializeAdServer];
    }
  }
  
  bool AdServerManager::IsInShowFreeCash()
  {
    return [ptrAdServer isInShowFreeCash];
  }
  
  bool AdServerManager::IsBannerVisible()
  {
    return [ptrAdServer isBannerVisible]? true : false;
  }
  
  void AdServerManager::ShowBanner(const int x, const int y, const int orientation)
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    //restoring some old code here.  I guess we have these backwards somewhere.  If only it wasn't late at night when we are trying to gold, I might fix this elsewhere
    int orientation_hack = orientation;
    if(orientation == 4)
    {
      orientation_hack = 3;
    }else if (orientation == 3)
    {
      orientation_hack = 4;
    }
    if(nil != ptrAdServer)
    {
      [ptrAdServer setBannerParameters:x y:y deviceOrientation:orientation_hack];
      [ptrAdServer performSelectorOnMainThread:@selector(showBanner) withObject:nil waitUntilDone:YES];
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::RemoveBanner()
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    if(nil != ptrAdServer)
    {
      [ptrAdServer performSelectorOnMainThread:@selector(removeBanner) withObject:nil waitUntilDone:YES];
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::GetBannerSize(int& width, int& height)
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    if(nil != [AdViewController sharedAdViewController])
    {
      [[AdViewController sharedAdViewController]getBannerSize:&width :&height];
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::GetScreenSize(int& width, int& height)
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    if(nil != [AdViewController sharedAdViewController])
    {
      [[AdViewController sharedAdViewController]getScreenSize:&width :&height];
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::ShowFreeCash()
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    if(nil != ptrAdServer)
    {
      [ptrAdServer showFreeCash];
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::ShowIGPView(CallbackAdsBasic igpExitCallback, void* caller)
  {
    /*if(nil != ptrAdServer)
     {
     [ptrAdServer showIGPView];
     
     _adsImpl->igpExitCallback = igpExitCallback;
     _adsImpl->igpExitCallbackCaller = caller;
     }*/
    
    int language = (int)(CasualCore::Game::GetInstance()->GetLanguage());
    
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    
    [[FreemiumIGP sharedFreemiumIGP]
     showOnView:appDelegate.window
     withLanguageIndex:language
     andGameVersion:[NSString stringWithUTF8String:(CasualCore::Game::GetInstance()->GetGameVersion())]];
    
  }
  /**
   * Function to check is Free Ads is enabled in the Server
   */
  void AdServerManager::CheckFreeCashReady()
  {
#ifdef GAMELOFT_AD_SERVER_IOS_ON
    if(_adsImpl->reward[AP_FREECASH_AVAILABLE])
    {
      static_cast<AdsSingleQueryData*>(_adsImpl->reward[AP_FREECASH_AVAILABLE])->request->SendRequest();
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  
  void AdServerManager::ShowInterstitial()
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    if(_adsImpl)
    {
      if(_adsImpl->m_cbInterstitial)
      {
        CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
        NSString* strGameCode = [[NSString stringWithUTF8String:(const char*)Game::GetInstance()->GetGameCode()] retain];
        [[AdViewController sharedAdViewController]showInterstitialOnView:[appDelegate window] :Game::GetInstance()->GetLanguage() :strGameCode];
      }
      else
      {
        RKLOG("Interstitial callback function is not sed.");
      }
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  int AdServerManager::GetInterstitialState()
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    return static_cast<int>([[AdViewController sharedAdViewController] getInterstitialsState]);
#else
    return 0;
#endif //GAMELOFT_AD_SERVER_IOS_ON
  }
  
  /**
   * It is a bridge function, the interstitial state INTERSTITIAL_STATE is defined in objectiveC header. The C++ AdserverManager cannot
   * include the ObjectiveC header, this function glue C++ and objectiveC code together.
   */
  void AdServerInterstitialCallbackFunc(INTERSTITIAL_STATE newState)
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    if(ADSERVER)
    {
      ADSERVER->InvokeInterstitialCallback(newState);
    }
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::SetInterstitialCallback(const CallbackAdsInterstitialFunc a_pCallbackFunc)
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    if(_adsImpl && a_pCallbackFunc)
    {
      _adsImpl->m_cbInterstitial = a_pCallbackFunc;
      [[AdViewController sharedAdViewController]setInterstitialChangeStateCallback:(TIntestitialsChangeStateCallback)AdServerInterstitialCallbackFunc];
    }
    else
    {
      _adsImpl->m_cbInterstitial = NULL;
      [[AdViewController sharedAdViewController]setInterstitialChangeStateCallback:(TIntestitialsChangeStateCallback)nil];
    }
#endif //GAMELOFT_AD_SERVER_IOS_ON
  }
  
  void AdServerManager::BlockInterstitials()
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    [[AdViewController sharedAdViewController] setIsPAU:YES];
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
  
  bool AdServerManager::ShowCinema()
  {
#if defined (GAMELOFT_AD_SERVER_IOS_ON)
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    NSString* strGameCode = [[NSString stringWithUTF8String:(const char*)Game::GetInstance()->GetGameCode()] retain];
    return TRUE == [[AdViewController sharedAdViewController] showCinemaOnView:appDelegate.window :Game::GetInstance()->GetLanguage() :strGameCode];
#else
    return false;
#endif // GAMELOFT_AD_SERVER_IOS_ON
  }
} // CasualCore

#endif // #if CC_USE_ADSSERVER==1
