#include "Platform.h"
#include "Game.h"
#include "Graphics/Scene.h"
#include "Platform/CasualFile.h"
#include "TranslationPack.h"
#include "Tools/Encryption.h"

#include "RKAssert.h"
#include "RKDevice.h"
#include "RKFile.h"
#include "RKHeap.h"
#include "RKString.h"
#include "RKInterlock.h"
#include "RKThread.h"

#import "CasualAppDelegate.h"
#import "CasualViewController.h"
#import "KeyboardView.h"
#import "DatePickerView.h"
#import "DateTime.h"
#import "ActivityIndicator.h"
#import "CasualMoviePlayer.h"
#include "Utils/IOS_Utils.h"
#import <StoreKit/StoreKit.h> // for IAP

#include <tinyxml.h>

#import "CoreFoundation/CFBundle.h"
#import <UIKit/UIKit.h>
#import "SplashScreenViewController.h"
#include "ServiceManagers/BaseServiceManager.h"
#include "Gaia.h"

#ifdef GAMELOFT_AD_SERVER_IOS_ON
#include "AdServerManager.h"
#import "AdViewController.h"
#endif

#include <sys/sysctl.h>
#include <glwebtools/internal/glwebtools_memory.h>
#import "NetworkState.h"
// MAC address
#include <net/if_dl.h>
#include <net/if.h>

#include <CommonCrypto/CommonDigest.h>

#import <CoreMotion/CoreMotion.h>

#if CC_USE_IN_GAME_BROWSER==1
#  include "InGameBrowser.h"
#endif

#include "../lib/CasualCoreOnline/Include/OnlineService.h"

static UIAlertView* s_pCurrMessage = NULL;

CasualCore::Platform::DialogCallback g_dialogcallback = 0;
CasualCore::ScreenOrientation defaultorientation = CasualCore::ORIENTATION_PORTRAIT;

// TODO(Diego): to be completed. And yes, there's a typo but some of the libraries rely on that (extern'ed).
char g_moudlePath[512] = "";
RKTexture* g_depthtexture = 0;

void autoStartGame(int suspend, const char* name = "")
{
}

RKString ValidateNSStringConvertion(const char* text)
{
  RKString textString = text;
  
  // nstring doesnt like having no break space in (\xa0) format..so converting it to supportted "\u00a0"
  RKString originalString;
  originalString.AppendChar(160); // no break space \xa0
  RKString replaceChar("\u00a0");
  
  textString.ReplaceAll(originalString, replaceChar);
  return textString;

}


namespace CasualCore
{
  const char* STR_AD_GAME_LANGUAGE[] = {"EN", "FR", "DE", "SP", "IT", "JP", "BR", "CN", "KO", "RU"};
  
  Platform::Platform()
  : isPushNotificationEnabled(true)
  , isHostReachableHeartbeat(false)

  , m_pThreadSubtitles(0)
  , m_RunningThread(0)
  , m_iCurrentSubtitle(0)

  , m_pVideoCallback(0)
  , m_pVideoCallingObject(0)
  , m_eVideoState(VIDEO_IDLE)
  
  , m_pKeyboardCallback(0)
  , m_pKeyboardCallingObject(0)
  , m_bKeyboardOpen(false)
  
  , m_pDatePickerCallback(0)
  , m_pDatePickerCallingObject(0)
    
  , m_fMoviePlaybackTime(0.0f)
  , depthtexture_(0)
  
  , m_subTitleFontName("Iceage")
  {
  }
  
  Platform::~Platform()
  {
    //ExternalLinksIOS::Release();
    //~Game is calling delete Platform .. delete Game::GetInstance();
    if (depthtexture_)
    {
      depthtexture_->ReleaseRef();
      RKDELETE(depthtexture_);
    }
  }
   
  ScreenOrientation Platform::GetScreenOrientation() const
  {
    ScreenOrientation result = ORIENTATION_LANDSCAPE_LEFT;
    
    switch ([UIApplication sharedApplication].statusBarOrientation) {
      case UIInterfaceOrientationPortrait:
        result = ORIENTATION_PORTRAIT;
        break;
        
      case UIInterfaceOrientationPortraitUpsideDown:
        result = ORIENTATION_PORTRAIT_UPSIDEDOWN;
        break;
        
      case UIInterfaceOrientationLandscapeLeft:
        result = ORIENTATION_LANDSCAPE_LEFT;
        break;
        
      case UIInterfaceOrientationLandscapeRight:
        result = ORIENTATION_LANDSCAPE_RIGHT;
        break;
        
      default:
        // Intentionally doing nothing
        break;
    }
    
    return result;
  }

  void Platform::SubscribeGesture(unsigned int gesture)
  {
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appDelegate.viewController subscribeGesture: gesture];
  }

  void Platform::UnsubscribeGesture(unsigned int gesture)
  {
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appDelegate.viewController unsubscribeGesture: gesture];
  }

  void Platform::UnsubscribeAllGestures()
  {
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appDelegate.viewController unsubscribeAllGestures];
  }

  void Platform::EnableMSAA(bool enabled)
  {
    CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appDelegate.viewController enableMSAA: enabled];
  }
  
	void Platform::OpenURL(const char* url)
	{
		NSString* path = [NSString stringWithUTF8String:url];
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:path]];
	}
  
	bool Platform::IsConnected(int networkType)
	{
    return CasualCoreOnline::CCOnlineService::IsConnected(networkType);
	}
  
  void Platform::LockOrientation(bool lock)
	{
		CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
		[[appdel viewController] setOrientationLocked:lock];
	}
  
	bool Platform::IsOrientationLocked() const
	{
		CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
		return [[appdel viewController] orientationLocked];
	}
  
  void Platform::TryCorrectOrientation()
	{
		CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
		[[appdel viewController] tryCorrectOrientation];
	}
  
	void Platform::Start(int argc, char** argv)
	{
    // Set up the module path for GLXPlayer
    if(0 == strlen(g_moudlePath))
    {
      RKFile_CreatePath(g_moudlePath, "");
      LOG(g_moudlePath);
    }
    
  }
  
  void Platform::SetDepthTexture(unsigned int texture)
  {
    if (!depthtexture_)
    {
      depthtexture_ = RKNEW(RKTexture);
      depthtexture_->GetRef();
    }
    depthtexture_->handle = texture;
    if (depthtexture_->width = RKDevice_GetWidth())
    {
      depthtexture_->texelSize[0] = 1.0 / RKDevice_GetWidth();
    }
    if (depthtexture_->height = RKDevice_GetHeight())
    {
      depthtexture_->texelSize[1] = 1.0 / RKDevice_GetHeight();
    }
    depthtexture_->pixelFormat = RKPF_DEPTH_COMPONENT;
    depthtexture_->clampMode = RK_CLAMP;
    depthtexture_->SetParam(RKTexture::HASMIPMAPS, false);
  }
  
  void Platform::InitializePushNotification()
  {
    if([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)])
    {
      UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:(UIRemoteNotificationTypeBadge|UIRemoteNotificationTypeSound|UIRemoteNotificationTypeAlert) categories:nil];
      [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
    }
    else
     
    {
      [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationType)(UIRemoteNotificationTypeBadge|UIRemoteNotificationTypeAlert|UIRemoteNotificationTypeSound)];
    }
  }

  void Platform::CancelLocalNotifications()
  {
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
  }
  
  void Platform::PostLocalNotification(const char* message, const char* action, int delayInSeconds, int a_iTrackingId, bool showBadge)
  {
    if (isPushNotificationEnabled)
    {
      RKString messageString = message;
      wchar_t* unicodeMessageString = messageString.ToUnicode();    // Fix for bug id 6633844, convert the unicode to a valid localised string

      // Fix for bug id 6633844, assigns the valid localised unicode string to NSString
      NSString* unicodeIOSText = [[NSString alloc] initWithBytes:unicodeMessageString length:wcslen(unicodeMessageString)*sizeof(wchar_t) encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
      NSString *strAction = [NSString stringWithUTF8String:action];
      
      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
      
      unicodeIOSText = unicodeIOSText == nil ? [NSString stringWithUTF8String:message] : unicodeIOSText;
      NSDictionary *params = [NSDictionary dictionaryWithObjectsAndKeys:
                              unicodeIOSText, @"message",
                              strAction, @"action",
                              [[NSNumber numberWithInt:delayInSeconds] stringValue], @"delayInSeconds",
                              [[NSNumber numberWithInt:showBadge ? 1 : 0] stringValue], @"showBadge",
                              [NSNumber numberWithInt:a_iTrackingId], @"trackingid",
                              nil];
      
      CasualAppDelegate* appDelegate = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
      [appDelegate showLocalNotification:params];
      
      RKDELETEARRAY(unicodeMessageString);
      unicodeMessageString = NULL;
      
      [pool release];
    }
  }
  
  void Platform::OpenKeyboard(KeyboardCallback pCallback, void* pCallingObject, const char* sInitialTextValue,  bool bMultiLine/*=false*/, bool bNumeric/*=false*/)
  {
    // open the on-screen keyboard
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    m_pKeyboardCallback = pCallback;
    m_pKeyboardCallingObject = pCallingObject;
    m_bKeyboardOpen = true;
    [appdel.keyboardView openKeyboard:sInitialTextValue withMultiLine:bMultiLine withNumber:bNumeric withMaxLength:1024];
  }
  
  void Platform::setKeyboardInputText(const char* a_sText)
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel.keyboardView KeyboardText:a_sText];
  }
  
  void Platform::CloseKeyboard()
  {
    // close the on-screen keyboard
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    m_bKeyboardOpen = false;
    [appdel.keyboardView closeKeyboard];
  }
    
  void Platform::OpenDatePicker(DatePickerCallback pCallback, void* pCallingObject, DATE_PICKER_MODE eMode)
  {
      // open the on-screen date picker
      CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
      m_pDatePickerCallback = pCallback;
      m_pDatePickerCallingObject = pCallingObject;
      [appdel.datePickerView openDatePicker:eMode];
  }

  void Platform::CloseDatePicker()
  {
    // close the on-screen date picker
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel.datePickerView closeDatePicker];
      
    // null values.
    m_pDatePickerCallback = 0;
    m_pDatePickerCallingObject = 0;
  }
    
  void Platform::OnDatePickerEvent(int datePickerStatus, DateTime dateInfo)
  {
      if (m_pDatePickerCallback)
          m_pDatePickerCallback(dateInfo, m_pDatePickerCallingObject);
  }
    
  void Platform::ShowActivityIndicator()
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel showActivityIndicator: YES];
  }
  
  void Platform::HideActivityIndicator()
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel hideActivityIndicator: YES];
  }
  
  void Platform::OnKeyboardEvent(int keyboardStatus, const char* sTextValue)
  {
    if (m_pKeyboardCallback)
    {
      m_pKeyboardCallback(keyboardStatus, sTextValue, m_pKeyboardCallingObject);
    }
  }
  
  void Platform::PlayVideo(const char* pFilename, const char *pSubTitles, VideoCallback pCallback, void* pCallingObject)
  {
    RKString fullPath;
    RKFile_CreatePath(fullPath, pFilename);
    
    m_pVideoCallback = pCallback;
    m_pVideoCallingObject = pCallingObject;
    
    LoadSubtitles(pSubTitles);//, a_iJapaneseLineLimit);
    
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel.moviePlayer playMovie:fullPath.GetString()];
    SetupSubtitlesUpdateThread(true);
  }
  
  void Platform::StopVideo()
  {
    if (m_RunningThread)
    {
      RKInterlock_Reset(m_RunningThread);
    }
    
    if (m_pThreadSubtitles)
    {
      RKThread_WaitForExit(m_pThreadSubtitles);
    }
    SetupSubtitlesUpdateThread(false);
    
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    [appdel.moviePlayer stopMovie];
  }
  
  void Platform::SetupSubtitlesUpdateThread(bool init)
  {
    if(m_pThreadSubtitles)
    {
      RKThread_Destroy(&m_pThreadSubtitles);
      m_pThreadSubtitles = 0;
    }
    
    if (m_RunningThread)
    {
      RKInterlock_Destroy(&m_RunningThread);
      m_RunningThread = 0;
    }
    
    if (init)
    {
      m_RunningThread = RKInterlock_Create("SubtitleInterlock");
      
      m_pThreadSubtitles = RKThread_Create("Platform::SubtitlesThread", Platform::TickUpdateSubtitles, this);
      assert(m_pThreadSubtitles);
      RKInterlock_Lock(m_RunningThread);
      RKThread_Start(m_pThreadSubtitles);
    }
  }
  
  unsigned int Platform::TickUpdateSubtitles(void* pThreadData)
  {
    RKTHREAD_ENTER
    Platform* ptrPlatform = (Platform*) pThreadData;
    
    while(RKInterlock_Query(ptrPlatform->m_RunningThread))
    {
      ptrPlatform->UpdateSubtitles();
      RKThread_Sleep(0, 50);
    }
    
    RKTHREAD_EXIT
    return 0;
  }
  
  void Platform::UpdateSubtitles()
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    
    [appdel.moviePlayer performSelectorOnMainThread:@selector(setMoviePlaybackTime) withObject:nil waitUntilDone:NO];
    
    if (m_iCurrentSubtitle < m_listSubtitles.Size())
    {
      if ( m_fMoviePlaybackTime >= m_listSubtitles[m_iCurrentSubtitle].fTimestamp)
      {
        if (!m_listSubtitles[m_iCurrentSubtitle].bDisplayed)
        {
          m_listSubtitles[m_iCurrentSubtitle].bDisplayed = true;
          CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
          RKString localisedAscii = GAMELOFT_TR(m_listSubtitles[m_iCurrentSubtitle].strText.GetString());
          const wchar_t* sLocalisedText = (wchar_t*)localisedAscii.ToUnicode();
          NSString* localizedIOSText = nil;
          NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
          int language_idx = CasualCore::Game::GetInstance()->GetTranslationPack()->GetDeviceLanguageID([language UTF8String]);
          
          if (language_idx == LANGUAGE::JAPANESE || language_idx == LANGUAGE::THAI)
          {
            std::wstring formattedText = CasualCore::Game::GetInstance()->GetTranslationPack()->FormatControlSymbols(sLocalisedText, [appdel.moviePlayer getSubTitlesCharacterWidth], [appdel.moviePlayer getSubTitlesLineWidth]);
            
            localizedIOSText = [[NSString alloc] initWithBytes:formattedText.c_str() length:formattedText.size()*sizeof(wchar_t)
                                                      encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
          }
          else
          {
            localizedIOSText = [[NSString alloc] initWithBytes:sLocalisedText length:wcslen(sLocalisedText)*sizeof(wchar_t)
                                                      encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
          }
          
          [appdel.moviePlayer performSelectorOnMainThread:@selector(setSubtitlesText:) withObject:localizedIOSText waitUntilDone:NO];
          [localizedIOSText release], localizedIOSText = nil;
          RKDELETEARRAY(sLocalisedText);
          sLocalisedText = NULL;
        }
        
        if ( m_fMoviePlaybackTime >= m_listSubtitles[m_iCurrentSubtitle].fTimestamp + m_listSubtitles[m_iCurrentSubtitle].fDuration)
        {
          m_iCurrentSubtitle++;
          [appdel.moviePlayer performSelectorOnMainThread:@selector(setSubtitlesText:) withObject:@"" waitUntilDone:NO ];
        }
      }
    }
  }
  
  bool Platform::LoadSubtitles(const char* a_kpcFilename)
  {
    TiXmlDocument fontsDoc;
    
    if (!fontsDoc.LoadFile("fontslist.xml"))
    {
      RKASSERT(0,"cannot find font list xml");
    }
    
    NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
    
    int language_idx = CasualCore::Game::GetInstance()->GetTranslationPack()->GetDeviceLanguageID([language UTF8String]);
    TiXmlElement* xmlFontElem = fontsDoc.RootElement();
    int i =0;

    // Loop through all the text fields.
    for (const TiXmlElement* pObjElem = xmlFontElem->FirstChildElement( "Font" ); pObjElem; pObjElem = pObjElem->NextSiblingElement( "Font" ))
    {
      if (i == language_idx)
      {
        m_subTitleFontName = pObjElem->Attribute("Name");
        break;
      }
      
      ++i;
    }

    if ( a_kpcFilename )
    {
      // load xml.
      TiXmlDocument document;
      if (document.LoadFile(a_kpcFilename))
      {
        // this function could potentionally be used a number of times during gameplay.
        // make sure our subtitle list is clear.
        m_listSubtitles.Clear();
        
        // grab the root node.
        TiXmlElement* root = document.FirstChildElement("subtitles");
        
        // grab the first subtitle
        TiXmlElement* subtitle = root->FirstChildElement("text");
        while (subtitle)    // while subtitle is valid
        {
          // create our subtitle data.
          tSubtitleInfo data;
          
          // grab data.
          data.strText = subtitle->Attribute("string");
          subtitle->QueryFloatAttribute("timestamp", &data.fTimestamp);
          subtitle->QueryFloatAttribute("duration", &data.fDuration);
          data.bDisplayed = false;
          
          // add to list
          m_listSubtitles.Append(data);
          
          // get next data set.
          subtitle = subtitle->NextSiblingElement("text");
        }
        return true;
      }
      return false;
    }
    return false;
  }
  
  void Platform::SetVideoDownloadOverlay(const char* sLocalisedTextKey, int progress)
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    if (appdel)
    {
      const wchar_t* pWideText = GAMELOFT_TR(sLocalisedTextKey).CreateDecodedBufferWCHAR();
      [appdel.moviePlayer setMovieDownloadOverlayText:pWideText withPercentage:progress];
    }
  }
  
  void Platform::SetVideoSkipButton(const char *sLocalisedTextKey, const char *sBtnUpImage, const char *sBtnDownImage, int x, int y, int w, int h)
  {
    CasualAppDelegate* appdel = (CasualAppDelegate*)[[UIApplication sharedApplication] delegate];
    if (appdel)
    {
      const wchar_t* pWideText = GAMELOFT_TR(sLocalisedTextKey).CreateDecodedBufferWCHAR();
      // set the video skip button parameters
      [appdel.moviePlayer setSkipButton:pWideText
                          buttonUpImage:sBtnUpImage
                        buttonDownImage:sBtnDownImage
                                   xPos:x
                                   yPos:y
                                  width:w
                                 height:h];
    }
  }
  
  
  void Platform::SetIconBadgeNumer(int number)
  {
    [[UIApplication sharedApplication] setApplicationIconBadgeNumber:number];
  }
  
  
  void Platform::PrintStringFromLocale(float amount, const char* localeID, char* buffer, int& bufferLen) const
  {
    NSLocale* locale = [[[NSLocale alloc] initWithLocaleIdentifier:[NSString stringWithUTF8String:localeID]] autorelease];
    if (locale == nil) {
      // Safety check
      NSLog(@"Unable to find %@. Using current locale.", [NSString stringWithUTF8String:localeID]);
      locale = [NSLocale currentLocale];
    }
    
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [numberFormatter setLocale:locale];
    
    NSString *formattedString = [numberFormatter stringFromNumber:[NSNumber numberWithFloat:amount]];
    
    const int formattedStringLen = strlen([formattedString UTF8String]);
    assert(bufferLen >= formattedStringLen); // Assert before writing the value in bufferLen
    
    // Write the length and the buffer
    bufferLen = formattedStringLen;
    memcpy(buffer, [formattedString UTF8String], bufferLen);
    
#if defined(GAME_DEBUG)
    // Convert the buffer back to an NSString and log.
    // Please note we use a NSData because otherwise it apparently won't work otherwise.
    // It won't display the unicode characters, apparently because of a limitation in NSLog.
    NSString* stringFromBuffer = [[[NSString alloc] initWithData:[[[NSData alloc] initWithBytes:buffer length:bufferLen] autorelease]
                                                        encoding:NSUTF8StringEncoding] autorelease];
    NSLog(@"PrintStringFromCurrency: %@", stringFromBuffer);
#endif // GAME_DEBUG
    
    [numberFormatter release];
  }
  
  // For some reason, apparently NSLog() does not support UTF-8 so please use this in the debugger (lldb)
  //   po stringFromBuffer
  //
  // The parameter currency can be one of the following (for the moment). They also match the currency identified returned by In-App Purchase (IAP)
  //
  //   "AUD"
  //   "CAD"
  //   "CHF"
  //   "CNY"
  //   "DKK"
  //   "EUR"
  //   "GBP"
  //   "JPY"
  //   "MXN"
  //   "NOK"
  //   "NZD"
  //   "SEK"
  //   "USD"
  
  void Platform::PrintStringFromCurrency(float amount, const char* currency,const char* defaultCode, char* buffer, int& bufferLen) const
  {
    NSDictionary* localeDictionary = [[NSDictionary alloc] initWithObjectsAndKeys:
                                      @"en_AU", @"AUD",
                                      @"en_CA", @"CAD",
                                      @"fr_CH", @"CHF",
                                      @"zh_CN", @"CNY",
                                      @"da_DK", @"DKK",
                                      @"fr_FR", @"EUR",
                                      @"en_GB", @"GBP",
                                      @"ja_JP", @"JPY",
                                      @"es_MX", @"MXN",
                                      @"nn_NO", @"NOK",
                                      @"en_NZ", @"NZD",
                                      @"sv_SE", @"SEK",
                                      @"en_US", @"USD",
                                      nil];
    
    NSString* localeIDAsString = [NSString stringWithUTF8String:currency];
    NSString* localeValue = [localeDictionary objectForKey:localeIDAsString];
    NSLocale* locale = nil;
    
    // Try to find a matching locale, otherwise use the current
    if (localeValue != nil) {
      locale = [[[NSLocale alloc] initWithLocaleIdentifier:localeValue] autorelease];
    }
    else {
      NSLog(@"Unable to find %@. Using current locale.", localeIDAsString);
      locale = [NSLocale currentLocale];
    }
    
    PrintStringFromLocale(amount, [localeValue UTF8String], buffer, bufferLen);
    
    [localeDictionary release];
  }
  
  void Platform::GetMacAddress(const size_t stringLength, char* outMacAddress) const
  {
    
    int                 mib[6];
    size_t              len;
    char                *buf;
    unsigned char       *ptr;
    struct if_msghdr    *ifm;
    struct sockaddr_dl  *sdl;
    
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_LINK;
    mib[4] = NET_RT_IFLIST;
    
    if ((mib[5] = if_nametoindex("en0")) == 0) {
      printf("Error: if_nametoindex error\n");
      return;
    }
    
    if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
      printf("Error: sysctl, take 1\n");
      return;
    }
    
    if ((buf = (char*)malloc(len)) == NULL) {
      printf("Could not allocate memory. error!\n");
      return;
    }
    
    if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
      printf("Error: sysctl, take 2");
      return;
    }
    
    ifm = (struct if_msghdr *)buf;
    sdl = (struct sockaddr_dl *)(ifm + 1);
    ptr = (unsigned char *)LLADDR(sdl);
    NSString *outstring = [NSString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X",
                           *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
    free(buf);
    
    strncpy(outMacAddress, [outstring UTF8String], stringLength*sizeof(char));
  }
  
  void Platform::GetMD5MacAddress(const size_t stringLength, char* outMd5MacAddress, bool tolower) const
  {
    const int STR_MAC_LEN = 64;
    char mac[STR_MAC_LEN];
    memset(mac, 0, STR_MAC_LEN);
    GetMacAddress(STR_MAC_LEN, mac);
    
    if(tolower)
    {
      const int STR_LEN_MACADDRESS = strlen(mac);
      const int OFFSET = 'Z' - 'z';
      for(int i=0; i<STR_LEN_MACADDRESS; ++i)
      {
        if(mac[i] >= 'A' && mac[i] <= 'Z')
        {
          mac[i] = mac[i] - OFFSET;
        }
      }
    }
    
    unsigned char result[16];
    CC_MD5( mac, strlen(mac), result ); // This is the md5 call
    if(stringLength>32)
    {
      sprintf(outMd5MacAddress, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
              result[0], result[1], result[2], result[3],
              result[4], result[5], result[6], result[7],
              result[8], result[9], result[10], result[11],
              result[12], result[13], result[14], result[15]);
    }
  }
  
  std::string Platform::GetBundleVersion() const
  {
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#if defined(CC_GAME_VERSION)
    std::string sVersion = ""STRINGIZE(CC_GAME_VERSION);
    return sVersion;
#else
    NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleVersionKey];
    return std::string([version UTF8String]);
#endif
  }
  
  /**
   * Returns device model, os or firewire version information based on parameter infoKey
   */
  void Platform::GetDeviceInfo(const DeviceInfo infoKey, const size_t stringLength, char* outInfo)
  {
   NSString* info = @"";
    switch (infoKey)
    {
      case DEVICE_INFO_MODEL:
      {
        // The model function only gives the model. e.g both iPad2 and iPad3 has model name iPad
        //info = [[UIDevice currentDevice] model];
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = (char*)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        info = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
        free(machine);
      }
        break;
      case DEVICE_INFO_OS:
        info = [[UIDevice currentDevice] systemName];
        break;
      case DEVICE_INFO_OS_VERSION:
      {
            NSString* osVersion;
            osVersion = [[UIDevice currentDevice] systemVersion];
            
            
        int mib[2] = {CTL_KERN, KERN_OSVERSION};
                u_int namelen = sizeof(mib) / sizeof(mib[0]);
                size_t bufferSize = 0;
                
                NSString *osBuildVersion = nil;
                
                // Get the size for the buffer
                sysctl(mib, namelen, NULL, &bufferSize, NULL, 0);
                
                u_char buildBuffer[bufferSize];
                int result = sysctl(mib, namelen, buildBuffer, &bufferSize, NULL, 0);
                
                if (result >= 0)
                {
                    osBuildVersion = [[[NSString alloc] initWithBytes:buildBuffer length:bufferSize encoding:NSUTF8StringEncoding] autorelease]; 
                }
                
          
            info = [NSString stringWithFormat:@"%@ (%@", osVersion, osBuildVersion];
            
            
            
        //   info = [[UIDevice currentDevice] systemVersion];
      }
        break;
      case DEVICE_INFO_IOS_TYPE:
      {
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char* machine = (char*)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        strncpy(outInfo, machine, stringLength);
        free(machine);
        return;
      }
      case DEVICE_INFO_IOS_FIRMWARE:
      {
        NSString* version = [[UIDevice currentDevice] systemVersion];
        strncpy(outInfo, [version UTF8String], stringLength);
        return;
      }
      case DEVICE_INFO_IOS_IAP:
      {
        if([SKPaymentQueue canMakePayments])
        {
          info = @"y";
        }
        else
        {
          info = @"n";
        }
        break;
      }
      default:
        info = @"-";
        break;
    }
    memset(outInfo, 0, stringLength);
    strncpy(outInfo, [info UTF8String], stringLength);
  }
  
  
  long long Platform::GetFreeDiskSpace()
  {
    uint64_t totalSpace = 0;
    uint64_t totalFreeSpace = 0;
    
    __autoreleasing NSError *error = nil;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error: &error];
    
    if (dictionary) {
      NSNumber *fileSystemSizeInBytes = [dictionary objectForKey: NSFileSystemSize];
      NSNumber *freeFileSystemSizeInBytes = [dictionary objectForKey:NSFileSystemFreeSize];
      totalSpace = [fileSystemSizeInBytes unsignedLongLongValue];
      totalFreeSpace = [freeFileSystemSizeInBytes unsignedLongLongValue];
      //RKLOG(@"Memory Capacity of %llu MiB with %llu MiB Free memory available.", ((totalSpace/1024ll)/1024ll), ((totalFreeSpace/1024ll)/1024ll));
    } else {
      //RKLOG(@"Error Obtaining System Memory Info: Domain = %@, Code = %@", [error domain], [error code]);
    }
    
    return totalFreeSpace
    ;
  }
  
  /*bool Platform::IsDeviceMusicPlaying() const
   {
   return [MPMusicPlayerController iPodMusicPlayer].playbackState == MPMusicPlaybackStatePlaying;
   }*/
  
  void Platform::showDialog(const char* a_UTF8Msg, const char* a_UTF8Titlemsg,const char* a_UTF8BtnText, const char* a_UTF8Btn2Text, DialogCallback callback)
  {
    RKString titleString = ValidateNSStringConvertion(a_UTF8Titlemsg);
    
    NSString* sTitle = [NSString stringWithUTF8String: titleString.GetString()];
    NSString* sMsg = [NSString stringWithUTF8String: a_UTF8Msg];
    NSString* sBtnText = [NSString stringWithUTF8String: a_UTF8BtnText];

    
    UIAlertView * dlg = [[UIAlertView alloc] initWithTitle:sTitle
                                                   message:sMsg
                                                  delegate:nil
                                         cancelButtonTitle:sBtnText
                                         otherButtonTitles:nil];
    
    if (a_UTF8Btn2Text)
    {
      NSString* sBtn2 = [NSString stringWithUTF8String: a_UTF8Btn2Text];
      [dlg addButtonWithTitle: sBtn2];
    }

    g_dialogcallback = callback;
    dlg.delegate = [[UIApplication sharedApplication] delegate];

    [dlg show];
    [dlg release];
  }
  
  void Platform::showMessage(const char* a_UTF8Msg)
  {
    RKASSERT(s_pCurrMessage == NULL, "Platform::showMessage called while another message is still active!");
      
    RKString titleString (a_UTF8Msg);
    const wchar_t* wsMessage = titleString.ToUnicode();
    RKString convertedMessage(wsMessage);
      
    NSString* sTitle = [NSString stringWithUTF8String: convertedMessage.GetString()];
    UIAlertView * dlg = [[UIAlertView alloc] initWithTitle:sTitle
                                                     message:nil
                                                    delegate:nil
                                           cancelButtonTitle:nil
                                           otherButtonTitles:nil];
      
      
    [dlg show];
    [dlg release];
    s_pCurrMessage = dlg;
    RKDELETEARRAY(wsMessage);
  }
  
  void Platform::dismissMessage()
  {
    if (s_pCurrMessage)
    {
      // just going to have to take your word for it
      UIAlertView* alert = (UIAlertView*)s_pCurrMessage;
      [alert dismissWithClickedButtonIndex:0 animated:YES];
      s_pCurrMessage = NULL;
    }
  }
  
  void Platform::GetIOSIds()
  {
    if(gaia::ios_utils::isAdvertisingTrackingEnable())
    {
      m_IDFA = gaia::ios_utils::getAdvertisingIdentifier();
    }
    
    m_IDFV = gaia::ios_utils::getIdentifierForVendor();
    if (m_IDFA.Length()<=0)
    {
      m_IDFA = "00000000-0000-0000-0000-000000000000";
    }
    if (m_IDFV.Length()<=0)
    {
      m_IDFV = "00000000-0000-0000-0000-000000000000";
    }
  }
  
  const char* Platform::GetIDFA()
  {
    if(m_IDFA.Length()<=0)
    {
      return NULL;
    }
    return m_IDFA.GetString();
  }
  const char* Platform::GetIDFV()
  {
    if(m_IDFV.Length()<=0)
    {
      return NULL;
    }
    return m_IDFV.GetString();
  }
  
  void Platform::CopyToClipboard(const char* a_string)
  {
    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string = [NSString stringWithUTF8String:(a_string)];
  }

#if CC_USE_IN_GAME_BROWSER==1
  void Platform::SetInGameBrowserLanguage(const int a_iLanguageIndex)
  {
    int idx = a_iLanguageIndex; //CasualCore::Game::GetInstance()->GetLanguage();
    idx = (idx < 0 ||  idx >= CasualCore::LANGUAGE::INVALID ) ? 0 : idx;
    [InGameBrowser sharedInGameBrowser].languageIndex = idx;
  }

  bool GetUserID(gaia::BaseServiceManager::Credentials user_credential, std::string& user_id)
  {
    gaia::Gaia* gaia_instance = gaia::Gaia::GetInstance();
    
    int iError = gaia_instance->GetCredentialDetails(user_credential, gaia::BaseServiceManager::USER_IDENTIFIER, user_id);
    
    if(gaia::OPERATION_SUCCESSFULL != iError)
    {
      user_id.clear();
      
      return false;
    }
    else
    {
      
      // SeanF: Remove the leading "gameloft:" or "facebook:" as per 6014689
      unsigned int pos = user_id.find(":");
      if (pos > 0)
      {
        user_id = user_id.substr(pos+1);
      }
    }
    
    return true;
  }
  
  void Platform::UpdateInGameBrowserSettings( )
  {
    // sets the language index
    int language_idx = CasualCore::Game::GetInstance()->GetLanguage();
    
    language_idx = (language_idx < 0 ||  language_idx >= CasualCore::LANGUAGE::INVALID ) ? 0 : language_idx;
    [InGameBrowser sharedInGameBrowser].languageIndex = language_idx;
    
    // set the UserID settings
    gaia::Gaia* gaia_instance = gaia::Gaia::GetInstance();
    
    if(gaia_instance->IsLoggedIn(gaia::BaseServiceManager::FACEBOOK))
    {
      std::string accountUID = "";
      GetUserID(gaia::BaseServiceManager::FACEBOOK, accountUID);
      [InGameBrowser sharedInGameBrowser].facebookID = [NSString stringWithUTF8String: accountUID.c_str()];

    }
    
    if (gaia_instance->IsLoggedIn(gaia::BaseServiceManager::GAMECENTER))
    {
      std::string accountUID = "";
      GetUserID(gaia::BaseServiceManager::GAMECENTER, accountUID);
      [InGameBrowser sharedInGameBrowser].gameCenterUID = [NSString stringWithUTF8String: accountUID.c_str()];
    }
    
    if (gaia_instance->IsLoggedIn(gaia::BaseServiceManager::GLLIVE))
    {
      std::string accountUID = "";
      GetUserID(gaia::BaseServiceManager::GLLIVE, accountUID);
      [InGameBrowser sharedInGameBrowser].gliveAccount = [NSString stringWithUTF8String: accountUID.c_str()];
    }
    
    if (gaia_instance->IsLoggedIn(gaia::BaseServiceManager::ANONYMOUS))
    {
      std::string accountUID = "";
      GetUserID(gaia::BaseServiceManager::ANONYMOUS, accountUID);
      [InGameBrowser sharedInGameBrowser].anonymousAccount = [NSString stringWithUTF8String: accountUID.c_str()];
    }
  }
  
#endif
    
  void Platform::ActivateGyroscope( bool enable )
  {
    CMMotionManager* manager = [(CasualAppDelegate*)[[UIApplication sharedApplication] delegate] motionManager];
    if ( enable )
    {
      if ( [manager isGyroAvailable] == YES )
      {
        [manager setGyroUpdateInterval: 0.1f];
        [manager startGyroUpdatesToQueue:[NSOperationQueue mainQueue] withHandler: ^(CMGyroData* gyroData, NSError* error ){
            CGAME->GetCurrentState()->OnGyroscopeUpdate( gyroData.rotationRate.x, gyroData.rotationRate.y, gyroData.rotationRate.z);
        }];
      }
    }
    else
    {
      if ( [manager isGyroActive] == YES )
      {
        [manager stopGyroUpdates];
      }
    }
  }
}

#if defined(TARGET_OS_IPHONE)
void RetLog(const char* logmsg)
{
  NSLog(@"%s", logmsg);
}
#endif

#if defined(RKRETAIL) && defined(TARGET_OS_IPHONE)

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int inline ptrTime(int a, int b, int c, int d)
{
  int ret = syscall(SYS_ptrace, a, b, c, d);
  
  return ret;
}

#define crash {         \
int* ptr = NULL;   \
*ptr = 0;          \
}                      \

#define DEBUGGER_CHECK {                        \
size_t size = sizeof(struct kinfo_proc);    \
struct kinfo_proc info;                     \
int ret, name[4];                           \
\
memset(&info, 0, sizeof(struct kinfo_proc));    \
\
name[0] = CTL_KERN;                             \
name[1] = KERN_PROC;                            \
name[2] = KERN_PROC_PID;                        \
name[3] = getpid();                             \
\
if ((ret = (sysctl(name, 4, &info, &size, NULL, 0)))) { \
crash;                               \
}                                                     \
\
if (info.kp_proc.p_flag & P_TRACED)                   \
{                                                     \
/* Code to react to debugging goes here */        \
crash;                                            \
}                                                     \
}
#endif

int main(int argc, char** argv)
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  {
#if defined(RKRETAIL) && defined(TARGET_OS_IPHONE)
    ptrTime(31/*PT_DENY_ATTACH*/, 0, 0, 0);
    
    DEBUGGER_CHECK;
#endif
    UIApplicationMain(argc, argv, nil, NSStringFromClass([CasualAppDelegate class]));
  }
  [pool drain];
}
