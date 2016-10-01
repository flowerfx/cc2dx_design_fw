//**************************************************************************************************************************************
//*
//* RKAssert.h
//*
//* -safe parameters passing, with a printf style
//* -dialog box with many options, including Ignore once/always, breakpoint, copy/paste error, etc.. [Win32]
//* -log errors, with double click access from Win32 Output window to source code
//* -3 levels of assert (RKWarning, RKAssert, RKFatal)
//*
/*
------ USAGE SAMPLES: ------

RKAssert(a<b, ERK("Bad:a=%d, b=%d, txt=%s", a,b, someMessage));

----

int i=15; .. #define IMAX 10 ..   int myArray[IMAX];
RKAssert(i<m, ERK("ooppss, i=%d should be less than IMAX=%d !", x,IMAX));
myArray[i] = 99;    //access anyway, will crash if user select 'ignore'

----
if (i>=m)
{
  RKWarning(i<m, ERK("ooppss, i=%d should be less than IMAX=%d !", x,IMAX));
  return;
}
myArray[i] = 99;    //access ok

*/
//*
//* [Christophe Le Bouil]
//**************************************************************************************************************************************
#ifndef _RK_ASSERT_H_
#define _RK_ASSERT_H_

#include "RKAssertPrivate.h"

#if defined _DEBUG && defined WP8
#define RKQA_ERR_ENABLE_RKWARNING  1      //1 or 0: set 1 to enable RKWarning - Rem: the idea of a 'warning' is that the code should handle the problem without crashing
#define RKQA_ERR_ENABLE_RKASSERT   1      //1 or 0: set 1 to enable RKAssert
#define RKQA_ERR_ENABLE_RKFATAL    1      //1 or 0: set 1 to enable RKFatal
#else
#define RKQA_ERR_ENABLE_RKWARNING  0      //1 or 0: set 1 to enable RKWarning - Rem: the idea of a 'warning' is that the code should handle the problem without crashing
#define RKQA_ERR_ENABLE_RKASSERT   0      //1 or 0: set 1 to enable RKAssert
#define RKQA_ERR_ENABLE_RKFATAL    1      //1 or 0: set 1 to enable RKFatal
#endif


	//----- The breakpoint ----
	extern bool gRKAssertWantBreakpoint;

#if RKTARGET==RKTARGET_WIN32
#ifdef _M_ARM
#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { __debugbreak();		} }		//real breakpoint
#else
#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { __asm { int 3 };		} }		//real breakpoint
#endif
#elif RKTARGET==RKTARGET_ANDRO
#include <signal.h>
#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { raise(SIGINT); } }    //[to check] + need #include <signal.h> ...
#elif RKTARGET==RKTARGET_IOS
#include <signal.h>
#include <unistd.h>
#if defined (__arm64__)
#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { raise(SIGINT); } }
#else
#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { do { int signal=SIGINT; __asm__ __volatile__("mov r0, %0\nmov r1, %1\nmov r12, %2\nswi 128\nnop\n" : : "r"(getpid()), "r"(signal), "r"(37) : "r12", "r0", "r1", "cc"); } while(0); }}
	//#define RKA_BREAKPT_ { if (gRKAssertWantBreakpoint) { raise(SIGINT); } }
#endif
#else
#include ("RKTARGET TODO!")
#endif


	//***********************************************************************************************************************************
	//
	//***********************************************************************************************************************************
	namespace RKUtils
	{
#if RKQA_ERR_ENABLE_RKWARNING == 1
#define RKWarning(bCondition, ...) { if (!(bCondition)) {RKErrorDisplay(RKERRT::RKERRT_WARNING, #bCondition, MakeRKAL(__VA_ARGS__), __FILE__,__FUNCTION__,__LINE__); RKA_BREAKPT_ } }
#else
#define RKWarning(bCondition, ...) 
#endif

#if RKQA_ERR_ENABLE_RKASSERT == 1
#define RKAssert(bCondition, ...) { if (!(bCondition)) { RKErrorDisplay(RKERRT::RKERRT_ASSERT, #bCondition, MakeRKAL(__VA_ARGS__), __FILE__,__FUNCTION__,__LINE__); RKA_BREAKPT_ } }
#else
#define RKAssert(bCondition, ...) 
#endif

#if RKQA_ERR_ENABLE_RKFATAL == 1
#define RKFatal(bCondition, ...) { if (!(bCondition)) { RKErrorDisplay(RKERRT::RKERRT_FATAL, #bCondition, MakeRKAL(__VA_ARGS__), __FILE__,__FUNCTION__,__LINE__); RKA_BREAKPT_ } }
#else
#define RKFatal(bCondition, ...) 
#endif
	}


#endif //_RK_ASSERT_H_

