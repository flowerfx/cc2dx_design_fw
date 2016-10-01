#ifndef __CASUALCORE_LIVEANDROID_H_
#define __CASUALCORE_LIVEANDROID_H_

#include <jni.h>

namespace CasualCore
{
  /**
  * This class is a wrapper for things related to GL Live on Android.
  */
  class LiveAndroid
  {
  public:
    // trophiesList "0,1,2,3" means that user unlocked the first 4 trophies as they were setup on online side for your game.
    static void Show(int language, int ggi, const char* gameCode, const char* gameVersion, const char* trophyList, const char* deviceToken);
    static void ShowWelcome(int language, int ggi, const char* gameCode, const char* gameVersion);

    static bool AreUserCredentialsSaved();
    static bool IsLoggedIn();
    static void Logout();

    // Interface matching the one of GLIVE2, which forces the user to pass a pointer and a buffer size (e.g. nameLen).
    // Refer to the document entitled "HowToIntegrate.doc"
    static void ReadUserInfo(char* username, int& nameLen, char* password, int& pwdLen);
    static void SaveUserInfo(const char* username, const char* password);
  };
}

#endif // __CASUALCORE_LIVEANDROID_H_
