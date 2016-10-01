#ifndef __CASUALCORE_APPLICATIONREVIEWERIOS_H_
#define __CASUALCORE_APPLICATIONREVIEWERIOS_H_

namespace CasualCore
{
  class ApplicationReviewerIOS
  {
  public:
    static void Review(const char* gameCode, const char* gameVersion, const char* language, const char* country);
    static void Release();
  };
}

#endif // __CASUALCORE_APPLICATIONREVIEWERIOS_H_
