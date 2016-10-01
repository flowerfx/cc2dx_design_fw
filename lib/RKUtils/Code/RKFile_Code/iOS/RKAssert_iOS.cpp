//**************************************************************************************************************************************
//*
//* RKAssert_iOS.cpp
//*
//**************************************************************************************************************************************
#ifdef OS_IOS
#include <stdlib.h>

#include <RKAssert.h>
#include <RKLog.h>
namespace RKUtils
{
//**************************************************************************************************************
void RKErrorDisplay_Platform(int errType, const char *cond, const char *file, const char *fn, int line, const char *bufferMessage)
{
  RKLOG("RKError, type %d, cond %s, file %s, fn %s, line %d, msg %s", errType, cond, file, fn, line, bufferMessage);
}
}
#endif
