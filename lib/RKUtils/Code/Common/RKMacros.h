#ifndef _RKMACROS_H
#define _RKMACROS_H

#include <assert.h>
#include "../RKLog_Code/RKLog.h"
#include "RKHeap.h"
#include "RKAssert.h"
namespace RKUtils
{
#if defined GAME_IOS || defined GAME_ANDROID// there is already an ARRAYSIZE macro on windows
#define ARRAYSIZE(a) ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

#undef SAFE_RELEASE
#define SAFE_RELEASE(p)		{ if(p) { (p)->Release(); (p)=0x00; } }

#undef SAFE_DELETE
#define RK_SAFE_DELETE(p)		{ if(p) { RKDELETE(p); (p)=0x00; } }

#undef SAFE_FREE
#define SAFE_FREE(p)		{ if(p) { RKHeap_Free(p); (p)=0x00; } }

#undef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)  { if(p) { RKDELETEARRAY(p); (p)=0x00; } }

	// warning if you change this length ensure you change the length in matching tools (RKConverter)
#define RKPATH_LENGTH 260

#define RKMAX(a,b)           (((a) > (b)) ? (a) : (b))
#define RKMIN(a,b)           (((a) < (b)) ? (a) : (b))
#define RKSQUARE(a)          (a*a)
#define RKCLAMP(MIN,MAX,VAL) (RKMAX(MIN,RKMIN(MAX,VAL)))

#ifdef _DEBUG
#define RKASSERT(a, message, ...) //{ RKAssert(a, message, ##__VA_ARGS__); }
#else
#define RKASSERT(a, message, ...)
#endif

#if defined(GAME_WIN32) && _MSC_VER >= 1600 // VS 2010
#define RKSTATICASSERT(a, msg) static_assert(a, msg)
#else
#define __RKSTATICASSERT_JOIN2(a, b) a ## b
#define __RKSTATICASSERT_JOIN(a, b) __RKSTATICASSERT_JOIN2(a, b)
#define RKSTATICASSERT(a, msg) enum { __RKSTATICASSERT_JOIN(__static_assert_value, __LINE__) = (1 / (!!(a))) };
#endif

#define RKISVALID(a)  if (a)
#define RKPI          3.1415926536f
#define RKTWO_PI      6.2831853072f
#define RKPI_ON_TWO   1.5707963268f
#define RKPI_ON_FOUR  0.7853981634f
#define RKPI_ON_EIGHT 0.3392699081f
#define RKSQRT_TWO    1.4142135623f
#define RKSQRT_THREE  1.7320508075f
#define RKFLT_MAX     3.402823466e+38F
#define RKEPSILON     1.0e-10f
#define RKSMALLEREPSILON 1.0e-16f
#define RKBIT(b)      (1<<(b))
#define RKTIMEDELTA   RKCore_GetElapsedTime()
#define RKDEGREES_TO_RADIANS  0.0174532925f // RKPI/180.0f
#define RKRADIANS_TO_DEGREES  57.2957795128f // 180.0f/RKPI
#define RKALIGNDATA(alignment) __declspec(align(alignment)) 
#define RKALIGN(a, alignment) (((size_t)(a)+(alignment)-1 ) & (~((alignment)-1)))

	template <typename TARGET, typename SOURCE>
	inline TARGET* guarded_pointer_cast(SOURCE* pSrc) {
		TARGET* res = static_cast<TARGET*>(pSrc);
		RKASSERT(dynamic_cast<TARGET*>(pSrc) == res, "Invalid typecast, cast to wrong class or cast to multiple inherited class happened");
		return res;
	}
	template <typename TARGET>
	inline TARGET* guarded_pointer_cast(void* pSrc) {
		return static_cast<TARGET*>(pSrc); // can't check void*, it's illegal to dynamic_cast it
	}

#ifdef STATIC_CAST
#undef STATIC_CAST
#endif
#define STATIC_CAST(ClassDef, pObj) guarded_pointer_cast<ClassDef>(pObj)
}
#endif
