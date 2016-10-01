#include "../Platform.h"
#include "Game.h"
#include "Scene.h"
#include "RK.h"
#include "RKCore.h"
#include "RKDevice.h"
#include "RKHeap.h"
#include "RKProfileManager.h"
#include "SWF/SWFManager.h"

#include <android/log.h>
#include "AdServerManager.h"
#include "AdServerManagerAndroid.h"
#include "config_Android.h"
#include "PlatformAndroid.h"

// GLOT include file
#include "../../../lib/GLOTonGLWT/include/TrackingManager.h"

#include <glwebtools/internal/glwebtools_memory.h>

// For using the function in Game.java
#include "../../../prj/android/GameSpecific/jni/jni_base.h"

#if USE_IN_APP_BILLING
  #include "IABilling.h"
#endif

#if USE_PUSH_NOTIFICATION_C2DM
  #include "APushNotification.h"
#endif

#if USE_WELCOME_SCREEN
  extern "C" void nativeLaunchWelcomeScreen(int lang);
#endif

extern "C" void nativeReviewGame();

// GLOT variables 
static jmethodID jGetM;
static jmethodID jGetI;
// end GLOT vars
JNIEnv* env;
JavaVM* ptrVM;
extern int s_windowWidth;
extern int s_windowHeight;
extern char* m_gAppPath;

struct Touch
{
  int x;
  int y;
};

#define MAXANDROIDTOUCHES 10
Touch* touches = 0;

// These are needed by GLF through FreemiumLib (Gaia)
namespace glf
{
  class App;
}



// Memory allocation functions
void* Glwt2Alloc(std::size_t size, glwebtools::MemHint memhint, const char* filename, const char* function, int line)
{
  return RKHeap_Alloc(size); 
}

void* Glwt2Realloc(void* ptr, std::size_t size, glwebtools::MemHint memhint, const char* filename, const char* function, int line)
{
  return RKHeap_Realloc(ptr,size); 
}

void Glwt2Free(void* ptr)
{
  if (ptr) RKHeap_Free(ptr); 
}

void GetSaveFilePath(char* filePath, const char* filename)
{
  sprintf(filePath, "%s/%s", m_gAppPath, filename);
  __android_log_print(ANDROID_LOG_INFO, "CasualCore", filePath);
}

void GetConfigFilePath(char* filePath, const char* filename)
{
  sprintf(filePath, "%s/%s", m_gAppPath, filename);
  __android_log_print(ANDROID_LOG_INFO, "CasualCore", filePath);
}

void GetSystemInfo(char** deviceName,char** sysVersion)
{
  __android_log_print(ANDROID_LOG_INFO, "CasualCore", "Skipping GetSystemInfo()");
}

#if USE_IN_APP_BILLING

extern "C" const char* InAppBilling_GetGLLiveUser()
{
  return "0"; //default parameter to send to GL server
}

#endif // USE_IN_APP_BILLING

namespace CasualCore
{
  Platform::Platform()
    : m_bKeyboardOpen(false)
    , m_pKeyboardCallback(0)
    , devicetype_(DEVICE_GALAXY2S)
  {
    touches = RKNEWARRAY(Touch, MAXANDROIDTOUCHES);
    SetViewportScale(1);    
  }

  Platform::~Platform()
  {
    RKDELETEARRAY(touches);
  }

  void Platform::OpenURL(const char* url)
  {
  }
  
  void Platform::ShowWelcomeScreen(const char* language) const
  {
#if USE_WELCOME_SCREEN
    nativeLaunchWelcomeScreen(0); // TODO(Diego): hard-coded, to be fixed later
#endif
  }

  ScreenOrientation Platform::GetScreenOrientation() const
  {
    return ORIENTATION_LANDSCAPE_LEFT; // TODO(Diego): hard-coded
  }

  void Platform::DirectToExternalLink(EXTERNAL_LINK_TYPE linkType, const char* gameCode) const
  {
    char url[128] = {0};
    switch(linkType)
    {
    case ELT_REVIEW:
      {
        // Based on Android guide https://docs.gameloft.org/rating-feature/
        // For the PLATFORM
        // Android marketplace: ANMP
        // For Amazon : AMAZ
        // For GLShop : FVGL
        // url format:
        // ingameads.gameloft.com/redir/?from=[IGPCODE]&game=[IGPCODE]&op=[PLATFORM]&ctg=GAME_REVIEW
        // Added the prefix "http://" based on http://stackoverflow.com/questions/5882656/no-activity-found-to-handle-intent-android-intent-action-view
        sprintf(url, "http://ingameads.gameloft.com/redir/?from=%s&game=%s&op=ANMP&ctg=GAME_REVIEW", gameCode, gameCode);
      }
      break;
    case ELT_CUSTOMER_CARE:
      // Based on Android guide https://docs.gameloft.org/customer-care-link-check-procedure/
      nativeLaunchCustomerSupport();
      break;
    }
    LOG(url);
    nativeOpenBrowser(url);
  }

  // Based on the documentation in https://docs.gameloft.org/push-notification-for-androiddev-doc/
  void Platform::PostLocalNotification(const wchar_t* message, const wchar_t* action, int delayInSeconds, int a_iTrackingId, bool showBadge)
  {
#if USE_PUSH_NOTIFICATION_C2DM
    char tmp[512] = {0};

    jobject bundle = ABundle_New();
    wcstombs(tmp, action, sizeof(tmp));
    ABundle_PutString("subject", tmp, bundle);
    wcstombs(tmp, message, sizeof(tmp));
    ABundle_PutString("body", tmp, bundle);
    ABundle_PutString("type", "launch", bundle);

    // TODO(Diego): add custom values for notification.

    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%d", delayInSeconds);
    const char* remotepush = APushNotification_SendPushToMyself(bundle, tmp, "0", true);

    if (strcmp(remotepush, PN_SEND_PUSH_CODE_PENDING) == 0)
    {
      // Request is sending to server using asyc mode, APushNotification_CallBack will receive this data.
      // For pending result, remotepush is your send push request id, use it for identify each request.
      const char* pending_request_id = ABundle_ReadString(PN_REQUEST_ID, bundle);
    }
    else if (strcmp(remotepush, PN_SEND_PUSH_CODE_ERROR) == 0)
    {
      // TODO(Diego): malformed/invalid request
    }
    else
    {
      // TODO(Diego): push queued locally or using blocking call, remotepush is your push id.
    }
#endif
  }

  void Platform::CancelLocalNotifications()
  {
    // TODO: not done
  }

  // are we connected to a network defined by NETWORK_TYPE enum
  // parameter is a bitmask field so can pass NET_3G | NET_WIFI | NET_INTERNET etc
  bool Platform::IsConnected(int networkType)
  {
    // <TODO> implement android platform
    return true;
  }

  void Platform::OpenKeyboard(KeyboardCallback pCallback, void* pCallingObject, const char* sInitialTextValue, bool bMultiLine, bool bNumeric)
  {
    // <TODO> not done
  }

  void Platform::CloseKeyboard()
  {
    // <TODO> not done
  }

  /**
  * Keyboard Updating, Native code can call this via the singleton
  */
  void Platform::OnKeyboardEvent(int keyboardStatus, const char* sTextValue)
  {
    if (m_pKeyboardCallback)
    {
      m_pKeyboardCallback(keyboardStatus, sTextValue, NULL);
    }
  }
  
  bool Platform::IsRetinaDisplayEnabled() const
  {
    return false;
  }

  bool Platform::IsPadIdiom() const
  {
    return false;
  }
  
  bool Platform::IsPhoneIdiom() const
  {
    return false;
  }
  
  bool Platform::IsAndroidIdiom() const
  {
    return true;
  }

  void Platform::PrintStringFromLocale(float amount, const char* localeID, char* buffer, int& bufferLen) const
  {
    // TODO(Diego): improve
    bufferLen = snprintf(buffer, bufferLen, "$%.2f", amount);
  }
  
  void Platform::PrintStringFromCurrency(float amount, const char* currency, const char* defaultCode, char* buffer, int& bufferLen) const
  {
    // TODO(Diego): improve
    bufferLen = snprintf(buffer, bufferLen, "$%.2f", amount);
  }

  extern "C" void AndroidIdle()
  {
    RKCore_OnDraw();

#ifdef USE_GAMESWF
    // Render GameSWF objects
    SWFManager* pFlashMgr = Game::GetInstance()->GetFlashManager();
    if (pFlashMgr)
    {
      pFlashMgr->Render();
    }
#endif
//#ifndef RKRETAIL
//    Game::GetInstance()->DrawProfileText();
//#endif
    Game::GetInstance()->Update();
  }

  static int touchcount = 0;
  extern "C" void resetTouchReg()
  {
    touchcount = 0;
  }

  extern "C" void AndroidOnTouch(int a, int x, int y, int id)
  {
    if (!Game::GetInstance())
    {
      return;
    }
    static float oldpinch = 0;
    int width, height;
    x -= RKDevice_GetWidth() / 2;
    y -= RKDevice_GetHeight() / 2;

    switch(a)
    {
    case 0: //Touch up
      for (int i = id; i < touchcount - 1 && i < MAXANDROIDTOUCHES-1; ++i)
      {
        touches[i].x = touches[i+1].x;
        touches[i].y = touches[i+1].y;
      }
      --touchcount;
      if (touchcount == 0)
      {
        Game::GetInstance()->GetScene()->InjectTouchUp(x, y);
      }
      else if (touchcount == 1)
      {
        Game::GetInstance()->GetScene()->InjectTouchDown(touches[0].x, touches[0].y);
      }
      else if (touchcount == 2)
      {
        touches[id].x = x;
        touches[id].y = y;
        float diffx = touches[0].x - touches[1].x;
        float diffy = touches[0].y - touches[1].y;
        float dist = sqrt(diffx*diffx + diffy*diffy);
        oldpinch = dist;
      }
      else if (touchcount < 0)
      {
        touchcount = 0;
      }
      break;
    case 1: //Touch down
      touches[touchcount].x = x;
      touches[touchcount].y = y;
      ++touchcount;

      if (touchcount == 1)
      {
        Game::GetInstance()->GetScene()->InjectTouchDown(x, y);
      }
      else if (touchcount == 2)
      {
        touches[id].x = x;
        touches[id].y = y;
        float diffx = touches[0].x - touches[1].x;
        float diffy = touches[0].y - touches[1].y;
        float dist = sqrt(diffx*diffx + diffy*diffy);
        oldpinch = dist;
      }
      break;
    case 2: //Touch move
      touches[id].x = x;
      touches[id].y = y;
      if (touchcount == 1)
      {
        Game::GetInstance()->GetScene()->InjectTouchDrag(x, y);
      }
      else if (touchcount == 2)
      {
        touches[id].x = x;
        touches[id].y = y;
        float diffx = touches[0].x - touches[1].x;
        float diffy = touches[0].y - touches[1].y;
        float dist = sqrt(diffx*diffx + diffy*diffy);
        float midx = touches[0].x - (diffx * 0.5f);
        float midy = touches[0].y - (diffy * 0.5f);

        Game::GetInstance()->GetScene()->InjectPinch(midx, midy, dist, oldpinch);
        oldpinch = dist;
      }
      break;
    }
  }

  void Platform::Start(int argc, char** argv)
  {
    RKLOG("Casual Core Started");
    LOG("ScreenDimensions: " << s_windowWidth << "x" << s_windowHeight);
    RKCore_SetWidth(s_windowWidth);
    RKCore_SetHeight(s_windowHeight);
    Initialize();
  }
  
  void Platform::SetIconBadgeNumer(int number)
  {
	// Does nothing on Android
  }

  void Platform::SubscribeGesture(unsigned int gesture)
  {
    // Don't know if there's an alternative in Android
    // If you are an Android dev and know how to implement
    // this, please contact AKL-Core-Feedback@gameloft.com
    // to integrate your changes.
  }

  void Platform::UnsubscribeGesture(unsigned int gesture)
  {
    // Don't know if there's an alternative in Android
    // If you are an Android dev and know how to implement
    // this, please contact AKL-Core-Feedback@gameloft.com
    // to integrate your changes.
  }

  void Platform::UnsubscribeAllGestures()
  {
    // Don't know if there's an alternative in Android
    // If you are an Android dev and know how to implement
    // this, please contact AKL-Core-Feedback@gameloft.com
    // to integrate your changes.
  }
  
  void Platform::PlayVideo(const char* pFilename, const char* pSubTitles, VideoCallback pCallback, void* pCallingObject)
  {
    // NOT IMPLEMENTED ON Android
    m_eVideoState = VIDEO_COMPLETE;
    if (pCallback)
      pCallback(m_eVideoState, pCallingObject);
  }

  void Platform::StopVideo()
  {
    // NOT IMPLEMENTED ON Android
    m_eVideoState = VIDEO_STOPPED;
  }

  void Platform::SetVideoSkipButton(const char* sTextKey, const char* sBtnUpImage, const char* sBtnDownImage, int x, int y, int w, int h)
  {
    // NOT IMPLEMENTED ON Android
  }
  
  void Platform::SetVideoDownloadOverlay(const char* sLocalisedText, int progress)
  {
	// NOT IMPLEMENTED ON Android
  }
  
  std::string Platform::GetBundleVersion() const
  {
    return GAME_VERSION_NAME; // this define automatically generated, look for config_Android.h
  }

  void Platform::GetMacAddress(const size_t stringLength, char* outMacAddress) const
  {
	nativeGetMacAddress((int)stringLength, outMacAddress);
  }
} // End of CasualCore
// ----------------------------------------------------------------------------------------------------

JavaVM* PlatformAndroid::ptrVM = NULL;

void PlatformAndroid::GetEnv(JNIEnv*& env)
{
  PlatformAndroid::ptrVM->AttachCurrentThread(&env, NULL);
}

#ifdef  __cplusplus
extern "C" {
#endif
  // Neet to get the JNIEnv all the time
  void GetCurrentEnv(JNIEnv*& env)
  {
    (PlatformAndroid::ptrVM)->AttachCurrentThread(&env, NULL);
  }

  // For getting the JavaVM
/*
  jint JNI_OnLoad(JavaVM* vm, void* reserved)
  {
    JNIEnv* env;
    LOG("$****** HAd VM: " << vm);
    assert(NULL != vm);
    PlatformAndroid::ptrVM = vm;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) 
    {
      return -1;
    }

#if USE_IN_APP_BILLING
    InAppBilling_setJavaVM(vm);
#endif
    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return JNI_VERSION_1_6;
  }
*/


#ifdef  __cplusplus
}
#endif

// ----------------------------------------------------------------------------------------------------
// TODO(Diego): this file needs refactoring
/*
#include "PlatformAndroid.h"
#include "AndroidInput.h"
#include "AndroidSocket.h"
#include "FacebookAndroid.h"
#include "LiveAndroid.h"
#include "Graphics/Scene.h"
#include "Game.h"
#include "config_Android.h"
#include "Tools/Localiser.h"
#include "../../RewardReceiver/RewardReceiver.h"

#include <orx.h>
#include <jni.h>
#include <iostream>
#include <android/log.h>
#include <cassert>

// ----------------------------------------------------------------------------------------------------
class PlatformAndroidBridge
{
public:
static void Init(jclass game);
static void GetScreenDimensions(int &width, int &height);
static void OpenURL(const char* url);


#if USE_ADS_SERVER
static jmethodID mMethodShowAds;
static jmethodID mMethodHideAds;
static jmethodID mMethodLoadFreeCash;
static jmethodID mMethodCheckFreeCashReady;
static jmethodID mMethodShowFreeCash;
static jmethodID mMethodGetRewardItems;

static void InitizlizeAdServer(JNIEnv* env, jclass game);
static void nativeShowAds(int languageIndex);
static void nativeHideAds();
static void nativeLoadFreeCash(int languageIndex);
static int nativeCheckFreeCashReady();
static void nativeShowFreeCash(int lang);
static void nativeGetRewardItems(const char* urlRetrieve, const char* urlClear);
#endif // USE_ADS_SERVER

private:
static jclass mClass;
static jmethodID mGetScreenWidth;
static jmethodID mGetScreenHeight;
static jmethodID mOpenURL;

};

jclass PlatformAndroidBridge::mClass = 0;
jmethodID PlatformAndroidBridge::mGetScreenWidth = 0;
jmethodID PlatformAndroidBridge::mGetScreenHeight = 0;
jmethodID PlatformAndroidBridge::mOpenURL = 0;


#if USE_ADS_SERVER
jmethodID PlatformAndroidBridge::mMethodShowAds = 0;
jmethodID PlatformAndroidBridge::mMethodHideAds = 0;
jmethodID PlatformAndroidBridge::mMethodLoadFreeCash = 0;
jmethodID PlatformAndroidBridge::mMethodCheckFreeCashReady = 0;
jmethodID PlatformAndroidBridge::mMethodShowFreeCash = 0;
// Get reward items
jmethodID PlatformAndroidBridge::mMethodGetRewardItems = 0;
#endif

void PlatformAndroidBridge::Init(jclass game)
{
DEBUG_OUT("PlatformAndroidBridge::Init()");

JNIEnv* mEnv = NULL; GetEnv(mEnv);
mClass = (jclass) mEnv->NewGlobalRef(game);

mGetScreenWidth = mEnv->GetStaticMethodID(mClass, "GetScreenWidth", "()I");
mGetScreenHeight = mEnv->GetStaticMethodID(mClass, "GetScreenHeight", "()I");
mOpenURL = mEnv->GetStaticMethodID(mClass, "openURL", "(Ljava/lang/String;)V");


if (globalVM)
VoxSetJavaVM(globalVM);
else
DEBUG_OUT("PlatformAndroidBridge::Init() - globalVM is NULL");
}

void PlatformAndroidBridge::GetScreenDimensions(int& width, int& height)
{
JNIEnv* env = NULL; 
GetEnv(env);
width = env->CallStaticIntMethod(mClass, mGetScreenWidth);
height = env->CallStaticIntMethod(mClass, mGetScreenHeight);
}

void PlatformAndroidBridge::OpenURL(const char* url)
{
jstring strUrl = mEnv->NewStringUTF(url);

JNIEnv* env = NULL; 
GetEnv(env);
env->CallStaticVoidMethod(mClass, mOpenURL, strUrl);

env->DeleteLocalRef(strUrl);
}


// ----------------------------------------------------------------------------------------------------
#if USE_ADS_SERVER
void PlatformAndroidBridge::InitizlizeAdServer(JNIEnv* env, jclass game)
{
mMethodShowAds = env->GetStaticMethodID (game, "showAds" , "(I)V" ); 
mMethodHideAds = env->GetStaticMethodID (game, "hideAds", "()V" );
mMethodLoadFreeCash = env->GetStaticMethodID (game, "loadFreeCash", "(I)V" );	
mMethodCheckFreeCashReady = env->GetStaticMethodID (game, "checkFreeCashReady" , "()Z" );
mMethodShowFreeCash = env->GetStaticMethodID (game, "showFreeCash", "()V" );
mMethodGetRewardItems = env->GetStaticMethodID(game, "getRewardItems", "(Ljava/lang/String;Ljava/lang/String;)V");
}

void PlatformAndroidBridge::nativeShowAds(int languageIndex)
{
if(mMethodShowAds)
mEnv->CallStaticVoidMethod(mClass, mMethodShowAds, languageIndex);
}

void PlatformAndroidBridge::nativeHideAds()
{
if (mMethodHideAds)
{
mEnv->CallStaticVoidMethod(mClass, mMethodHideAds);
}
}

void PlatformAndroidBridge::nativeLoadFreeCash(int languageIndex)
{
if (mMethodLoadFreeCash)
{
mEnv->CallStaticVoidMethod(mClass, mMethodLoadFreeCash, languageIndex);
}
}

int PlatformAndroidBridge::nativeCheckFreeCashReady()
{
if (mMethodCheckFreeCashReady)
{
if (mEnv->CallStaticBooleanMethod(mClass, mMethodCheckFreeCashReady) == 0)
return 0;
else
return 1;
}
return 0;
}

void PlatformAndroidBridge::nativeShowFreeCash(int lang)
{
if (mMethodShowFreeCash)
{
mEnv->CallStaticVoidMethod(mClass, mMethodShowFreeCash);
}
}

void PlatformAndroidBridge::nativeGetRewardItems(const char* urlRetrieve, const char* urlClear)
{
if (mMethodGetRewardItems)
{
jstring jstrUrlRetrieveItems = mEnv->NewStringUTF(urlRetrieve);
jstring jstrUrlClearItems = mEnv->NewStringUTF(urlClear);
mEnv->CallStaticVoidMethod(mClass, mMethodGetRewardItems, jstrUrlRetrieveItems, jstrUrlClearItems);
}
}
#endif // USE_ADS_SERVER

#ifdef  __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------------------------------

JNIEXPORT void JNICALL JNI_FUNCTION(GameActivity_nativeInitializeActivity) (JNIEnv* env, jclass game)
{
//PlatformAndroidBridge::Init(game);
//CasualCore::FacebookAndroid::InitializeJNI(game);
//CasualCore::LiveAndroid::InitializeJNI(game);

#if USE_ADS_SERVER
AdServerAndroidBridge::Init(env, game);
#endif
}

JNIEXPORT void JNICALL JNI_FUNCTION(GameActivity_nativeFacebookProcessFriends) (JNIEnv* env, jclass game, jobjectArray ids, jobjectArray names, jobjectArray emails, jbooleanArray areAppUsers)
{
CasualCore::FacebookAndroid::ProcessFriendsFromJNI(ids, names, emails, areAppUsers);
}

JNIEXPORT void JNICALL JNI_FUNCTION(GameActivity_nativeFacebookProcessRecommendations) (JNIEnv* env, jclass game, jobjectArray ids, jobjectArray names)
{
CasualCore::FacebookAndroid::ProcessRecommendationsFromJNI(ids, names);
}


// For getting the primitive data ot the reward itmes from java code
// the data include 4 arraies, ids, amounts, types and create times, the arraies have same size, number of the elements is the number of reward items we got

JNIEXPORT void JNICALL JNI_FUNCTION(GameActivity_nativeRetrieveRewardItems) (JNIEnv* env, jclass game, jobjectArray ids, jobjectArray amounts, jobjectArray types, jobjectArray times)
{
CasualCore::RewardItem item;
int id;
int amount; 
char type[CasualCore::REWARD_ITEM_TYPE_SIZE];
char time[CasualCore::REWARD_ITEM_TIME_SIZE];

const int NUM_ITEM = env->GetArrayLength(ids);

RKList<CasualCore::RewardItem> listRewardedItem;
for(int i = 0; i< NUM_ITEM; ++i)
{
// id
jobject joid = env->GetObjectArrayElement(ids, i);
const char* id = env->GetStringUTFChars(static_cast<jstring>(joid), 0);

// amount
jobject joamount = env->GetObjectArrayElement(amounts, i);
const char* amount = env->GetStringUTFChars(static_cast<jstring>(joamount), 0);

// type
jobject jotype = env->GetObjectArrayElement(types, i);
const char* type = env->GetStringUTFChars(static_cast<jstring>(jotype), 0);

// create time
jobject jotime = env->GetObjectArrayElement(times, i);
const char* time = env->GetStringUTFChars(static_cast<jstring>(jotime), 0);

item.id = atoi(id);
item.amount = atoi(amount);
strncpy(item.type, type, CasualCore::REWARD_ITEM_TYPE_SIZE); 
strncpy(item.type, type, CasualCore::REWARD_ITEM_TIME_SIZE); 

listRewardedItem.Append(item);
}

CasualCore::RewardReceiver::ProcessRewardItem(listRewardedItem);
}
#ifdef  __cplusplus
}
#endif

// ----------------------------------------------------------------------------------------------------
// CasualCore Platform Android

namespace CasualCore
{

PlatformAndroid::PlatformAndroid() 
{
}

PlatformAndroid::~PlatformAndroid()
{
}

orxSTATUS orxFASTCALL Init()
{
//chdir("assets"); // Diego: we do not need to change directory on Android
Game::GetInstance()->GetPlatform()->Initialize();
return orxSTATUS_SUCCESS;
}

orxSTATUS orxFASTCALL Run()
{
Game::GetInstance()->Update();
return orxSTATUS_SUCCESS;
}

void orxFASTCALL Exit()
{
delete Game::GetInstance();
CAndroidSocket::Clearup();
}

void PlatformAndroid::Initialize()
{
//input_->Initialize();
Platform::Initialize();
CAndroidSocket::Startup();
}

void PlatformAndroid::GetScreenDimensions(int& width, int& height)
{
PlatformAndroidBridge::GetScreenDimensions(width, height);
}

void PlatformAndroid::SetDefaultScreenOrientation(ScreenOrientation orientation)
{
}

ScreenOrientation PlatformAndroid::GetScreenOrientation()
{
return ORIENTATION_PORTRAIT;
}

bool GetLocalEnv()
{
JNIEnv* env = NULL;
if (globalVM->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK)
{
mLocalEnv = env;
return true;
}
return false;
}

bool PlatformAndroid::SetupAlternatePaths(const char* _URL)
{
Debug(_URL);

mClassAnOrxActivity_checkForExternalURL = mEnv->GetStaticMethodID(mActivityInstance, "checkForExternalURL", "(Ljava/lang/String;)Z");

// Call the function 
jstring strURL = mEnv->NewStringUTF(_URL);
mEnv->CallStaticBooleanMethod(mActivityInstance, mClassAnOrxActivity_checkForExternalURL, strURL);
mEnv->DeleteLocalRef(strURL);

}

void PlatformAndroid::GetFile(const char* fileName, CasualFile** outputCasualFile){}

void PlatformAndroid::ShowBanner(const int x, const int y)
{
#if USE_ADS_SERVER
int language = Game::GetInstance()->GetLocaliser()->GetRegion();
language = GonvertToAdServerLanguage(language);
PlatformAndroidBridge::nativeShowAds(language);
#endif
}

void PlatformAndroid::RemoveBanner()
{
#if USE_ADS_SERVER
PlatformAndroidBridge::nativeHideAds();
#endif
}

void PlatformAndroid::LoadFreeCash()
{
#if USE_ADS_SERVER
int language = Game::GetInstance()->GetLocaliser()->GetRegion();
language = GonvertToAdServerLanguage(language);
PlatformAndroidBridge::nativeLoadFreeCash(language);
#endif
}

bool PlatformAndroid::CheckFreeCashReady()
{
#if USE_ADS_SERVER
return PlatformAndroidBridge::nativeCheckFreeCashReady();
#else
return false;
#endif
}

void PlatformAndroid::ShowFreeCash()
{
#if USE_ADS_SERVER
int language = Game::GetInstance()->GetLocaliser()->GetRegion();
language = GonvertToAdServerLanguage(language);
PlatformAndroidBridge::nativeShowFreeCash(language);
#endif
}

void PlatformAndroid::RetrieveTapJoyRewardItem(const int ggi, const char* userinfo)
{
#if USE_ADS_SERVER
//const char* urlRetrieveItems = "http://gllive-beta.gameloft.com/ope/tapjoy.php?action=retrieveitems&game=25497&user=gllive:hjd3igkdut2241xbwqoj-1251726103";
//const char* urlClearItems = "http://gllive-beta.gameloft.com/ope/tapjoy.php?action=clearItems&game=25497&user=gllive:hjd3igkdut2241xbwqoj-1251726103";
char urlRetrieveItems[256]; 
char urlClearItems[256];

urlRetrieveItems[0] = 0;
urlClearItems[0] = 0;

sprintf(urlRetrieveItems, "http://gllive-beta.gameloft.com/ope/tapjoy.php?action=retrieveitems&game=%d&user=%s", ggi, userinfo);
sprintf(urlClearItems, "http://gllive-beta.gameloft.com/ope/tapjoy.php?action=clearItems&game=%d&user=%s", ggi, userinfo);

__android_log_print(ANDROID_LOG_INFO, "CasualCore", urlRetrieveItems);
__android_log_print(ANDROID_LOG_INFO, "CasualCore", urlClearItems);

PlatformAndroidBridge::nativeGetRewardItems(urlRetrieveItems, urlClearItems);
#endif
}

void PlatformAndroid::LoadIGPView(){}



int PlatformAndroid::GonvertToAdServerLanguage(int idxLocalisationLanguage)
{
// Localisation	{"EN", "FR", "SP", "IT", "DE", "JP", "BR", "CN", "KR" };
// Ad server	{"EN", "FR", "DE", "IT", "SP", "JP", "KR", "CN", "BR" };
//				  0     1     2     3     4     5     6     7     8

switch (idxLocalisationLanguage)
{
//case 0: // EN
//case 1: // FR
//case 3: // IT
//case 5: // JP
//case 7: // CN
//	break;
case 2: // SP
return 4;
case 4: // DE
return 2;
break;
case 6: // BR
return 8;
case 8: // KR
return 6;
default:
break;
}
return idxLocalisationLanguage;
}

void PlatformAndroid::OpenURL(const char* url)
{
PlatformAndroidBridge::OpenURL(url);
}

}; // end namespace Casua
*/
