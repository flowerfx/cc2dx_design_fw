//**************************************************************************************************************************************
//*
//* RKAssert_Andro.cpp
//*
//**************************************************************************************************************************************
#if defined(ANDROID_OS)
#include <RKAssert.h>
#include <RKLog.h>

//**************************************************************************************************************
void RKErrorDisplay_Platform(int errType, const char *cond, const char *file, const char *fn, int line, const char *bufferMessage)
{
  RKLOG("RKError, type %d, cond %s, file %s, fn %s, line %d, msg %s", errType, cond, file, fn, line, bufferMessage);
}
#endif

