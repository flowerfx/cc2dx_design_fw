
#include "LiveAndroid.h"
#include "config_Android.h"

#include <android/log.h>

#if USE_GLLIVE || USE_GLLIVE_HTML5
extern "C" void nativelaunchGLLive_new(int createAccount);
extern "C" void nativelaunchGLLive(int lang);

extern "C" void nativeGetGLLiveUsername(const char* buffer, int* bufferLen);
extern "C" void nativeGetGLLivePassword(const char* buffer, int* bufferLen);
#endif

namespace CasualCore
{
  void LiveAndroid::Show(int language, int ggi, const char* gameCode, const char* gameVersion, const char* trophyList, const char* deviceToken)
  {
#if (USE_GLLIVE_HTML5)
    nativelaunchGLLive_new(0);
#else
    nativelaunchGLLive(language);
#endif
  }

  void LiveAndroid::ShowWelcome(int language, int ggi, const char* gameCode, const char* gameVersion)
  {
    // To be completed
  }

  bool LiveAndroid::AreUserCredentialsSaved()
  {
    char username[128] = {0};
    int nameLen = sizeof(username) / sizeof(username[0]);

    char password[128] = {0};
    int pwdLen = sizeof(password) / sizeof(password[0]);

    ReadUserInfo(username, nameLen, password, pwdLen);

    return nameLen > 0 && pwdLen > 0;
  }

  bool LiveAndroid::IsLoggedIn()
  {
    bool result = AreUserCredentialsSaved();

    if (result)
    {
      __android_log_print(ANDROID_LOG_INFO, "CasualCore", "LiveAndroid::IsLoggedIn()");
    }
    else
    {
      __android_log_print(ANDROID_LOG_INFO, "CasualCore", "LiveAndroid::IsLoggedIn() NOT");
    }

    return result;
  }

  void LiveAndroid::Logout()
  {
    // To be completed
  }

  void LiveAndroid::ReadUserInfo(char* username, int& nameLen, char* password, int& pwdLen)
  {
    nativeGetGLLiveUsername(username, &nameLen);
    nativeGetGLLivePassword(password, &pwdLen);
  }

  void LiveAndroid::SaveUserInfo(const char* username, const char* password)
  {
    // To be completed
  }
}
