///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKInterlock.h
///
/// Description :	RKInterlock Module
///----------------------------------------------------------------------
#ifndef RKINTERLOCK_H
#define RKINTERLOCK_H

#include "../Common/RKHeap.h"
#include "../Common/RKTypes.h"
namespace RKUtils
{
#if defined (_MSVC_) || defined(_WIN32)
	// for windows interlocks
#include <process.h>
#elif !defined( __GNUC__ ) && defined( __APPLE__ )
	// LLVM compatible version
#include "libkern/OSAtomic.h"
#endif

	///----------------------------------------------------------------------
	/// RKInterlock
	///----------------------------------------------------------------------
#if defined( _MSC_VER ) || defined(_WIN32)
	__declspec(align(4))
	struct RKInterlock
#else
	struct __attribute__((aligned (4))) RKInterlock
#endif
	{
#if defined (_MSC_VER) || defined(_WIN32)
		volatile long lockCount;
#else
		volatile int32_t lockCount;
#endif
	};

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline RKInterlock* RKInterlock_Create(const char* pName)
	{
		// The variable pointed to by the Addend parameter must be aligned on a 32-bit boundary;
		RKInterlock* pInterlock = (RKInterlock*)RKHeap_AllocAligned(sizeof(RKInterlock), 32, "RKInterlock");
		pInterlock->lockCount = 0;

		return pInterlock;
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline void RKInterlock_Destroy(RKInterlock** ppInterlock)
	{
		RKInterlock* pInterlock = (RKInterlock*)*ppInterlock;
		*ppInterlock = 0;

		RKHeap_FreeAligned(pInterlock);
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline int RKInterlock_Lock(RKInterlock* pInterlock)
	{
#if defined( _MSC_VER ) || defined(_WIN32)
		return InterlockedIncrementAcquire(&pInterlock->lockCount);
#elif defined( __GNUC__ )
		// for iOS and Android GCC
		return __sync_add_and_fetch(&pInterlock->lockCount,1);
#elif defined( __APPLE__ )
		// for iOS LLVM
		return OSAtomicIncrement32(&pInterlock->lockCount);
#else
		RKSTATICASSERT(0,"These Atomic functions may not be compatible with your compiler, check.");
#endif
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline int RKInterlock_Unlock(RKInterlock* pInterlock)
	{
#if defined( _MSC_VER ) || defined(_WIN32)
		return InterlockedDecrementAcquire(&pInterlock->lockCount);
#elif defined( __GNUC__ )
		// for iOS and Android GCC
		return __sync_sub_and_fetch(&pInterlock->lockCount,1);
#elif defined( __APPLE__ )
		// for iOS LLVM
		return OSAtomicDecrement32(&pInterlock->lockCount);
#else
		RKSTATICASSERT(0,"These Atomic functions may not be compatible with your compiler, check.");
#endif
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline int RKInterlock_Query(RKInterlock* pInterlock)
	{
#if defined( _MSC_VER ) || defined(_WIN32)
		volatile unsigned long valueOut;
		InterlockedExchange(&valueOut, pInterlock->lockCount);
		return (int)valueOut;
#elif defined( __GNUC__ )
		// for iOS and Android GCC
		return __sync_fetch_and_add(&pInterlock->lockCount,0);
#elif defined( __APPLE__ )
		// for iOS LLVM
		return OSAtomicAdd32(0, &((RKInterlockInternal*)pInterlock)->lockCount);
#else
		RKSTATICASSERT(0,"These Atomic functions may not be compatible with your compiler, check.");
#endif
	}

	///----------------------------------------------------------------------
	///----------------------------------------------------------------------
	inline void RKInterlock_Reset(RKInterlock* pInterlock)
	{
#if defined( _MSC_VER ) || defined(_WIN32)
		InterlockedExchange(&pInterlock->lockCount, 0);
#else
		// for iOS and Android GCC
		pInterlock->lockCount=0;
#endif
	}
}
#endif