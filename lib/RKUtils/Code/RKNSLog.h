#ifndef _RK_NSLOG_H
#define _RK_NSLOG_H

#include "RKLog.h"

#ifndef RKLOG_DISABLE_LOGGING
#define RKNSLOG(...) NSLog(__VA_ARGS__)
#else
#define RKNSLOG(...) _RKLog_NOP_StubFunction()
#endif

#endif // _RK_NSLOG_H
